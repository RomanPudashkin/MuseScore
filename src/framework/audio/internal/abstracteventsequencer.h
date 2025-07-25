/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2022 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MUSE_AUDIO_ABSTRACTEVENTSEQUENCER_H
#define MUSE_AUDIO_ABSTRACTEVENTSEQUENCER_H

#include <map>
#include <set>

#include "global/async/asyncable.h"
#include "mpe/events.h"

#include "audiosanitizer.h"
#include "../audiotypes.h"

namespace muse::audio {
template<class ... Types>
class AbstractEventSequencer : public async::Asyncable
{
public:
    using EventType = std::variant<Types...>;
    using EventSequence = std::set<EventType>;
    using EventSequenceMap = std::map<msecs_t, EventSequence>;

    typedef typename EventSequenceMap::const_iterator SequenceIterator;
    typedef typename EventSequence::const_iterator EventIterator;

    virtual ~AbstractEventSequencer()
    {
        m_playbackData.mainStream.resetOnReceive(this);
        m_playbackData.offStream.resetOnReceive(this);
    }

    void load(const mpe::PlaybackData& data)
    {
        ONLY_AUDIO_WORKER_THREAD;

        m_playbackData = data;

        m_playbackData.mainStream.onReceive(this, [this](const mpe::PlaybackEventsMap& events,
                                                         const mpe::DynamicLevelLayers& dynamics) {
            m_playbackData.originEvents = events;
            m_playbackData.dynamics = dynamics;
            m_shouldUpdateMainStreamEvents = true;

            if (m_isActive || m_updateMainStreamWhenInactive) {
                updateMainStream();
            }
        });

        m_playbackData.offStream.onReceive(this, [this](const mpe::PlaybackEventsMap& events,
                                                        const mpe::DynamicLevelLayers& dynamics,
                                                        bool flush) {
            if (flush) {
                flushOffstream();
            }

            updateOffStreamEvents(events, dynamics);
        });

        updateMainStreamEvents(data.originEvents, data.dynamics);
    }

    const mpe::PlaybackData& playbackData() const
    {
        return m_playbackData;
    }

    void updateMainStream()
    {
        if (m_shouldUpdateMainStreamEvents) {
            updateMainStreamEvents(m_playbackData.originEvents, m_playbackData.dynamics);
            m_shouldUpdateMainStreamEvents = false;
        }
    }

    void setUpdateMainStreamWhenInactive(bool update)
    {
        m_updateMainStreamWhenInactive = update;
    }

    void flushOffstream()
    {
        if (m_offStreamEvents.empty()) {
            return;
        }

        m_offStreamEvents.clear();
        updateOffSequenceIterator();

        if (m_onOffStreamFlushed) {
            m_onOffStreamFlushed();
        }
    }

    void setActive(const bool active)
    {
        if (m_isActive == active) {
            return;
        }

        m_isActive = active;

        if (m_isActive) {
            updateMainStream();
        }
    }

    bool isActive() const
    {
        return m_isActive;
    }

    void setPlaybackPosition(const msecs_t newPlaybackPosition)
    {
        ONLY_AUDIO_WORKER_THREAD;

        m_playbackPosition = newPlaybackPosition;
        resetAllIterators();
    }

    msecs_t playbackPosition() const
    {
        ONLY_AUDIO_WORKER_THREAD;

        return m_playbackPosition;
    }

    using OnFlushedCallback = std::function<void ()>;

    void setOnOffStreamFlushed(OnFlushedCallback flushed)
    {
        ONLY_AUDIO_WORKER_THREAD;

        m_onOffStreamFlushed = flushed;
    }

    void setOnMainStreamFlushed(OnFlushedCallback flushed)
    {
        ONLY_AUDIO_WORKER_THREAD;

        m_onMainStreamFlushed = flushed;
    }

    mpe::dynamic_level_t dynamicLevel(const msecs_t position) const
    {
        for (const auto& layer : m_playbackData.dynamics) {
            const mpe::DynamicLevelMap& dynamics = layer.second;
            auto it = muse::findLessOrEqual(dynamics, position);
            if (it != dynamics.end()) {
                return it->second;
            }
        }

        return mpe::dynamicLevelFromType(muse::mpe::DynamicType::Natural);
    }

    EventSequenceMap movePlaybackForward(const msecs_t nextMsecs)
    {
        ONLY_AUDIO_WORKER_THREAD;

        EventSequenceMap result;

        if (!m_isActive) {
            result.emplace(m_offstreamPosition, EventSequence());

            if (m_currentOffSequenceIt == m_offStreamEvents.cend()) {
                return result;
            }

            m_offstreamPosition += nextMsecs;
            handleOffStream(result);

            return result;
        }

        // Empty sequence means to continue the previous sequence
        result.emplace(m_playbackPosition, EventSequence());
        m_playbackPosition += nextMsecs;

        if (m_currentMainSequenceIt == m_mainStreamEvents.cend()) {
            return result;
        }

        handleMainStream(result);

        return result;
    }

protected:
    virtual void updateOffStreamEvents(const mpe::PlaybackEventsMap& events, const mpe::DynamicLevelLayers& dynamics) = 0;
    virtual void updateMainStreamEvents(const mpe::PlaybackEventsMap& events, const mpe::DynamicLevelLayers& dynamics) = 0;

    void resetAllIterators()
    {
        updateMainSequenceIterator();
        updateOffSequenceIterator();
    }

    void updateMainSequenceIterator()
    {
        m_currentMainSequenceIt = m_mainStreamEvents.lower_bound(m_playbackPosition);
    }

    void updateOffSequenceIterator()
    {
        m_currentOffSequenceIt = m_offStreamEvents.cbegin();
        m_offstreamPosition = 0;
    }

    void handleMainStream(EventSequenceMap& result)
    {
        while (m_currentMainSequenceIt != m_mainStreamEvents.end()
               && m_currentMainSequenceIt->first <= m_playbackPosition) {
            EventSequence& sequence = result[m_currentMainSequenceIt->first];
            sequence.insert(m_currentMainSequenceIt->second.cbegin(),
                            m_currentMainSequenceIt->second.cend());
            m_currentMainSequenceIt = std::next(m_currentMainSequenceIt);
        }
    }

    void handleOffStream(EventSequenceMap& result)
    {
        if (m_offStreamEvents.empty()) {
            return;
        }

        while (m_currentOffSequenceIt != m_offStreamEvents.end()
               && m_currentOffSequenceIt->first <= m_offstreamPosition) {
            EventSequence& sequence = result[m_currentOffSequenceIt->first];
            sequence.insert(m_currentOffSequenceIt->second.cbegin(),
                            m_currentOffSequenceIt->second.cend());
            m_currentOffSequenceIt = std::next(m_currentOffSequenceIt);
        }

        if (m_currentOffSequenceIt == m_offStreamEvents.end()) {
            m_offStreamEvents.clear();
            updateOffSequenceIterator();
        }
    }

    mutable msecs_t m_playbackPosition = 0;
    mutable msecs_t m_offstreamPosition = 0;

    SequenceIterator m_currentMainSequenceIt;
    SequenceIterator m_currentOffSequenceIt;
    EventSequenceMap m_mainStreamEvents;
    EventSequenceMap m_offStreamEvents;

    mpe::PlaybackData m_playbackData;

    bool m_isActive = false;

    OnFlushedCallback m_onOffStreamFlushed;
    OnFlushedCallback m_onMainStreamFlushed;

private:
    bool m_shouldUpdateMainStreamEvents = false;
    bool m_updateMainStreamWhenInactive = false;
};
}

#endif // MUSE_AUDIO_ABSTRACTEVENTSEQUENCER_H
