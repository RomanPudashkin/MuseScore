/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited
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

#include "notationregionsbeingprocessedmodel.h"

#include "audio/itracks.h"

#include "engraving/playback/utils/arrangementutils.h"

using namespace mu::notation;
using namespace muse::audio;
using namespace mu::playback;

static const Segment* findSegmentFrom(const mu::engraving::Score* score, const System* system, const int tickFrom)
{
    const Segment* segment = score->tick2segment(Fraction::fromTicks(tickFrom), true, mu::engraving::SegmentType::ChordRest);
    if (!segment) {
        return nullptr;
    }

    if (segment->system() != system) {
        return nullptr;
    }

    return segment;
}

static const Segment* findSegmentTo(const mu::engraving::Score* score, const System* system, const Segment* segmentFrom,
                                    const int tickTo, const staff_idx_t staffIdx)
{
    const Segment* segment = score->tick2segment(Fraction::fromTicks(tickTo), true, mu::engraving::SegmentType::ChordRest);
    if (!segment) {
        return nullptr;
    }

    if (segment->system() != system) {
        return nullptr;
    }

    if (segment == segmentFrom) {
        return segment;
    }

    const track_idx_t trackIdx = mu::engraving::staff2track(staffIdx);
    const EngravingItem* item = segment->elementAt(trackIdx);
    if (!item) {
        return nullptr;
    }

    if (!item->isRest()) {
        return segment;
    }

    while (segment && item && item->isRest()) {
        segment = segment->prev(mu::engraving::SegmentType::ChordRest);
        item = segment ? segment->elementAt(trackIdx) : nullptr;
    }

    return segment;
}

NotationRegionsBeingProcessedModel::NotationRegionsBeingProcessedModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

QVariant NotationRegionsBeingProcessedModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount()) {
        return QVariant();
    }

    const RegionInfo& region = m_regions.at(index.row());

    switch (role) {
    case RectRole: return region.rect;
    case ProgressRole: {
        auto it = m_tracksBeingProcessed.find(region.trackId);
        return it != m_tracksBeingProcessed.end() ? it->second.progress : 0;
    }
    }

    return QVariant();
}

int NotationRegionsBeingProcessedModel::rowCount(const QModelIndex&) const
{
    return m_regions.size();
}

QHash<int, QByteArray> NotationRegionsBeingProcessedModel::roleNames() const
{
    static const QHash<int, QByteArray> roles {
        { RectRole, "rect" },
        { ProgressRole, "progress" },
    };

    return roles;
}

void NotationRegionsBeingProcessedModel::load()
{
    onMasterNotationChanged();
    globalContext()->currentMasterNotationChanged().onNotify(this, [this]() {
        onMasterNotationChanged();
    });

    onOnlineSoundsChanged();
    playbackController()->onlineSoundsChanged().onNotify(this, [this]() {
        onOnlineSoundsChanged();
    });

    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        updateRegionsBeingProcessed(m_tracksBeingProcessed);
    });

    playbackController()->isPlayingChanged().onNotify(this, [this]() {
        if (configuration()->optionB()) {
            updateRegionsBeingProcessed(m_tracksBeingProcessed);

            if (!m_tracksBeingProcessed.empty()) {
                emit isEmptyChanged();
            }
        }
    });
}

QVariant NotationRegionsBeingProcessedModel::notationViewMatrix() const
{
    return m_notationViewMatrix;
}

bool NotationRegionsBeingProcessedModel::isEmpty() const
{
    return m_regions.empty();
}

QColor NotationRegionsBeingProcessedModel::progressBackgroundColor() const
{
    return configuration()->selectionColor(mu::engraving::VOICES); // "all voices" color
}

QColor NotationRegionsBeingProcessedModel::progressFontColor() const
{
    return configuration()->notationColor();
}

void NotationRegionsBeingProcessedModel::setNotationViewMatrix(const QVariant& matrix)
{
    if (m_notationViewMatrix == matrix) {
        return;
    }

    m_notationViewMatrix = matrix;
    emit notationViewMatrixChanged();

    updateRegionsBeingProcessed(m_tracksBeingProcessed);
}

void NotationRegionsBeingProcessedModel::onMasterNotationChanged()
{
    clear();

    const IMasterNotationPtr master = globalContext()->currentMasterNotation();
    if (!master) {
        return;
    }

    master->playback()->aboutToBeChanged().onReceive(this, [this](const ChangesRange& range) {
        m_lastChangedRange = TickRange { range.tickFrom, range.tickTo };
    });
}

void NotationRegionsBeingProcessedModel::onOnlineSoundsChanged()
{
    const std::set<TrackId>& newSounds = playbackController()->onlineSounds();

    for (const TrackId& trackId : newSounds) {
        if (!muse::contains(m_onlineSounds, trackId)) {
            onTrackAdded(trackId);
        }
    }

    for (const TrackId trackId : m_onlineSounds) {
        if (!muse::contains(newSounds, trackId)) {
            onTrackRemoved(trackId);
        }
    }

    m_onlineSounds = newSounds;
}

void NotationRegionsBeingProcessedModel::onTrackAdded(const TrackId trackId)
{
    const IPlaybackController::InstrumentTrackIdMap& instrumentTrackIdMap = playbackController()->instrumentTrackIdMap();
    const InstrumentTrackId instrumentTrackId = muse::key(instrumentTrackIdMap, trackId);
    if (!instrumentTrackId.isValid()) {
        return;
    }

    const TrackSequenceId sequenceId = playbackController()->currentTrackSequenceId();

    playback()->tracks()->inputProcessingProgress(sequenceId, trackId)
    .onResolve(this, [this, instrumentTrackId](InputProcessingProgress inputProgress) {
        if (inputProgress.progress.isStarted()) {
            onProgressStarted(instrumentTrackId);
        }

        inputProgress.progress.started().onNotify(this, [this, instrumentTrackId]() {
            onProgressStarted(instrumentTrackId);
        });

        inputProgress.chunksBeingProcessedChannel.onReceive(this, [this, instrumentTrackId]
                                                                (const std::vector<InputProcessingProgress::ChunkInfo>& chunks) {
            onChunksReceived(instrumentTrackId, chunks);
        });

        inputProgress.progress.progressChanged().onReceive(this, [this, instrumentTrackId](int64_t current, int64_t, std::string) {
            onProgressChanged(instrumentTrackId, current);
        });

        inputProgress.progress.finished().onReceive(this, [this, instrumentTrackId](const muse::ProgressResult& res) {
            onProgressFinished(instrumentTrackId, res.ret);
        });
    });
}

void NotationRegionsBeingProcessedModel::onTrackRemoved(const muse::audio::TrackId trackId)
{
    const IPlaybackController::InstrumentTrackIdMap& instrumentTrackIdMap = playbackController()->instrumentTrackIdMap();
    const InstrumentTrackId instrumentTrackId = muse::key(instrumentTrackIdMap, trackId);
    if (!instrumentTrackId.isValid()) {
        return;
    }

    onProgressFinished(instrumentTrackId, muse::make_ok());
}

void NotationRegionsBeingProcessedModel::onProgressStarted(const InstrumentTrackId& instrumentTrackId)
{
    if (!muse::contains(m_tracksBeingProcessed, instrumentTrackId)) {
        m_tracksBeingProcessed[instrumentTrackId] = TrackInfo();
    }
}

void NotationRegionsBeingProcessedModel::onChunksReceived(const InstrumentTrackId& instrumentTrackId,
                                                          const std::vector<InputProcessingProgress::ChunkInfo>& chunks)
{
    const INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    auto it = m_tracksBeingProcessed.find(instrumentTrackId);
    if (it == m_tracksBeingProcessed.end()) {
        return;
    }

    const bool wasEmpty = m_regions.empty();

    TrackInfo& info = it->second;
    info.ranges.clear();

    const mu::engraving::Score* score = notation->elements()->msScore();
    bool shouldUpdate = false;

    for (const InputProcessingProgress::ChunkInfo& chunk : chunks) {
        TickRange range;
        range.tickFrom = mu::engraving::timestampToTick(score, secsToMicrosecs(chunk.start));
        range.tickTo = mu::engraving::timestampToTick(score, secsToMicrosecs(chunk.end));

        if (!muse::contains(info.ranges, range)) {
            info.ranges.push_back(range);
            shouldUpdate = true;
        }
    }

    if (shouldUpdate) {
        updateRegionsBeingProcessed({ { instrumentTrackId, info } });
    }

    if (wasEmpty != m_regions.empty()) {
        emit isEmptyChanged();
    }
}

void NotationRegionsBeingProcessedModel::onProgressChanged(const InstrumentTrackId& instrumentTrackId, int progress)
{
    auto trackIt = m_tracksBeingProcessed.find(instrumentTrackId);
    if (trackIt == m_tracksBeingProcessed.end()) {
        return;
    }

    if (trackIt->second.progress == progress) {
        return;
    }

    trackIt->second.progress = progress;

    for (int i = 0; i < m_regions.size(); ++i) {
        RegionInfo& region = m_regions[i];
        if (region.trackId != instrumentTrackId) {
            continue;
        }

        QModelIndex modelIdx = index(i);
        emit dataChanged(modelIdx, modelIdx, { ProgressRole });
    }
}

void NotationRegionsBeingProcessedModel::onProgressFinished(const InstrumentTrackId& instrumentTrackId, const muse::Ret& ret)
{
    auto trackIt = m_tracksBeingProcessed.find(instrumentTrackId);
    if (trackIt == m_tracksBeingProcessed.end()) {
        return;
    }

    m_tracksBeingProcessed.erase(trackIt);

    const QList<RegionInfo> regionsCopy = m_regions;
    const bool wasEmpty = m_regions.empty();

    for (const RegionInfo& region : regionsCopy) {
        if (region.trackId != instrumentTrackId) {
            continue;
        }

        const int idx = m_regions.indexOf(region);
        if (idx < 0) {
            continue;
        }

        beginRemoveRows(QModelIndex(), idx, idx);
        m_regions.removeAt(idx);
        endRemoveRows();
    }

    if (m_tracksBeingProcessed.empty() && ret.code() != (int)muse::Ret::Code::Cancel) {
        m_lastChangedRange.clear();
    }

    if (wasEmpty != m_regions.empty()) {
        emit isEmptyChanged();
    }
}

void NotationRegionsBeingProcessedModel::clear()
{
    m_tracksBeingProcessed.clear();
    m_lastChangedRange.clear();

    if (m_regions.empty()) {
        return;
    }

    beginResetModel();
    m_regions.clear();
    endResetModel();

    emit isEmptyChanged();
}

void NotationRegionsBeingProcessedModel::updateRegionsBeingProcessed(const TracksBeingProcessed& tracks)
{
    if (tracks.empty()) {
        return;
    }

    if (configuration()->optionB() && !playbackController()->isPlaying()) {
        return;
    }

    const INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    QList<RegionInfo> oldRegions = m_regions;

    m_regions.removeIf([&tracks](const RegionInfo& r) {
        return muse::contains(tracks, r.trackId);
    });

    for (const auto& pair : tracks) {
        const Part* part = notation->parts()->part(pair.first.partId);
        if (!part) {
            continue;
        }

        const std::vector<QRectF> newRects = calculateRects(part, pair.second.ranges);

        for (const QRectF& rect : newRects) {
            RegionInfo region;
            region.trackId = pair.first;
            region.rect = rect;

            m_regions.push_back(region);
        }
    }

    if (m_regions != oldRegions) {
        emit layoutChanged();
    }
}

std::vector<QRectF> NotationRegionsBeingProcessedModel::calculateRects(const Part* part, const std::vector<TickRange>& ranges) const
{
    const mu::engraving::Score* score = part->score();
    const mu::engraving::MeasureBase* lastMeasure = score->last();
    const double lastMeasureEndX = lastMeasure ? lastMeasure->canvasPos().x() + lastMeasure->width() : 0.0;
    const QTransform matrix = m_notationViewMatrix.value<QTransform>();

    std::vector<QRectF> result;

    for (const System* system : score->systems()) {
        if (system->measures().empty() || system->staves().empty()) {
            continue;
        }

        const mu::engraving::staff_idx_t staffIfx = system->firstVisibleSysStaffOfPart(part);
        if (staffIfx == muse::nidx) {
            continue;
        }

        const mu::engraving::SysStaff* sysStaff = system->staff(staffIfx);
        if (!sysStaff) {
            continue;
        }

        std::vector<QRectF> systemRects;

        for (const TickRange& range : ranges) {
            if (system->last()->tick().ticks() < range.tickFrom) {
                continue;
            }

            if (system->first()->tick().ticks() > range.tickTo) {
                continue;
            }

            const muse::PointF systemPos = system->canvasPos();
            muse::RectF logicRect = sysStaff->bbox().translated(systemPos);

            const Segment* segmentFrom = findSegmentFrom(score, system, range.tickFrom);
            if (segmentFrom) {
                const double segmentFromStartX = segmentFrom->canvasPos().x();
                const double startX = std::max(segmentFromStartX, logicRect.x());
                logicRect.setLeft(startX);
            } else {
                const double firstNoteRestX = systemPos.x() + system->firstNoteRestSegmentX();
                logicRect.setLeft(firstNoteRestX);
            }

            const Segment* segmentTo = findSegmentTo(score, system, segmentFrom, range.tickTo, staffIfx);
            if (segmentTo) {
                const double segmentToEndX = segmentTo->canvasPos().x() + segmentTo->width();
                const double endX = std::min(systemPos.x() + system->width(), segmentToEndX);
                logicRect.setRight(endX);
            } else if (lastMeasure) {
                const double width = std::min(logicRect.width(), lastMeasureEndX - logicRect.x());
                logicRect.setWidth(width);
            }

            const QRectF newRect = matrix.mapRect(logicRect.toQRectF());
            bool shouldAdd = true;

            for (QRectF& rect: systemRects) {
                if (rect.intersects(newRect)) {
                    rect = rect.united(newRect);
                    shouldAdd = false;
                    break;
                }
            }

            if (shouldAdd) {
                systemRects.push_back(newRect);
            }
        }

        result.insert(result.end(), std::make_move_iterator(systemRects.begin()),
                      std::make_move_iterator(systemRects.end()));
    }

    return result;
}
