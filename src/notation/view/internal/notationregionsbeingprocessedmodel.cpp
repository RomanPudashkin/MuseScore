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
#include "audio/audioutils.h"

using namespace mu::notation;
using namespace muse::audio;
using namespace mu::playback;

NotationRegionsBeingProcessedModel::NotationRegionsBeingProcessedModel(QObject* parent)
    : QObject(parent)
{
}

void NotationRegionsBeingProcessedModel::load()
{
    onMasterNotationChanged();
    globalContext()->currentMasterNotationChanged().onNotify(this, [this]() {
        onMasterNotationChanged();
    });

    onNotationChanged();
    globalContext()->currentNotationChanged().onNotify(this, [this]() {
        onNotationChanged();
    });

    playbackController()->trackAdded().onReceive(this, [this](const TrackId trackId) {
        onTrackAdded(trackId);
    });

    playbackController()->trackRemoved().onReceive(this, [this](const TrackId trackId) {
        onTrackRemoved(trackId);
    });

    playbackController()->isPlayingChanged().onNotify(this, [this]() {
        if (configuration()->optionB()) {
            updateRegionsBeingProcessed();
            emit regionsChanged();
        }
    });
}

QRectF NotationRegionsBeingProcessedModel::notationViewRect() const
{
    return m_notationViewRect;
}

QVariant NotationRegionsBeingProcessedModel::notationViewMatrix() const
{
    return m_notationViewMatrix;
}

QVariantList NotationRegionsBeingProcessedModel::regions() const
{
    QVariantList result;
    for (const auto& pair : m_regionsBeingProcessed) {
        result << pair.second.rects;
    }

    return result;
}

QColor NotationRegionsBeingProcessedModel::regionColor() const
{
    return configuration()->notationUnderOnlineProcessingColor();
}

void NotationRegionsBeingProcessedModel::setNotationViewRect(const QRectF& rect)
{
    if (m_notationViewRect == rect) {
        return;
    }

    m_notationViewRect = rect;
    emit notationViewRectChanged();

    if (!m_regionsBeingProcessed.empty()) {
        updateRegionsBeingProcessed();
        emit regionsChanged();
    }
}

void NotationRegionsBeingProcessedModel::setNotationViewMatrix(const QVariant& matrix)
{
    if (m_notationViewMatrix == matrix) {
        return;
    }

    m_notationViewMatrix = matrix;
    emit notationViewMatrixChanged();

    if (!m_regionsBeingProcessed.empty()) {
        updateRegionsBeingProcessed();
        emit regionsChanged();
    }
}

void NotationRegionsBeingProcessedModel::onMasterNotationChanged()
{
    clear();

    const IMasterNotationPtr master = globalContext()->currentMasterNotation();
    if (!master) {
        return;
    }

    master->playback()->aboutToBeChanged().onReceive(this, [this](const ChangesRange& range) {
        m_lastChangedRange = range;
    });
}

void NotationRegionsBeingProcessedModel::onNotationChanged()
{
    if (!m_regionsBeingProcessed.empty()) {
        updateRegionsBeingProcessed();
        emit regionsChanged();
    }
}

void NotationRegionsBeingProcessedModel::onTrackAdded(const TrackId trackId)
{
    const TrackSequenceId sequenceId = playbackController()->currentTrackSequenceId();

    playback()->tracks()->inputParams(sequenceId, trackId)
    .onResolve(this, [this, sequenceId, trackId](const AudioInputParams& inputParams) {
        const bool isOnline = muse::audio::isOnlineSound(inputParams);
        if (!isOnline) {
            return;
        }

        const IPlaybackController::InstrumentTrackIdMap& instrumentTrackIdMap = playbackController()->instrumentTrackIdMap();
        const InstrumentTrackId instrumentTrackId = muse::key(instrumentTrackIdMap, trackId);
        if (!instrumentTrackId.isValid()) {
            return;
        }

        playback()->tracks()->inputProcessingProgress(sequenceId, trackId)
        .onResolve(this, [this, instrumentTrackId](muse::audio::InputProcessingProgress inputProgress) {
            inputProgress.progress.started().onNotify(this, [this, instrumentTrackId]() {
                onProcessStarted(instrumentTrackId);
            });

            inputProgress.progress.finished().onReceive(this, [this, instrumentTrackId](const muse::ProgressResult&) {
                onProcessFinished(instrumentTrackId);
            });
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

    onProcessFinished(instrumentTrackId);
}

void NotationRegionsBeingProcessedModel::onProcessStarted(const InstrumentTrackId& instrumentTrackId)
{
    auto it = m_regionsBeingProcessed.find(instrumentTrackId);
    if (it != m_regionsBeingProcessed.end()) {
        ChangesRange& range = it->second.range;
        range.tickFrom = std::min(range.tickFrom, m_lastChangedRange.tickFrom);
        range.tickTo = std::max(range.tickTo, m_lastChangedRange.tickTo);
        range.staffIdxFrom = std::min(range.staffIdxFrom, m_lastChangedRange.staffIdxFrom);
        range.staffIdxTo = std::max(range.staffIdxTo, m_lastChangedRange.staffIdxTo);
    } else {
        RegionInfo& region = m_regionsBeingProcessed[instrumentTrackId];
        region.range = m_lastChangedRange;
    }

    updateRegionsBeingProcessed();
    emit regionsChanged();
}

void NotationRegionsBeingProcessedModel::onProcessFinished(const InstrumentTrackId& instrumentTrackId)
{
    const bool removed = muse::remove(m_regionsBeingProcessed, instrumentTrackId);
    if (!removed) {
        return;
    }

    if (m_regionsBeingProcessed.empty()) {
        m_lastChangedRange.clear();
    }

    updateRegionsBeingProcessed();
    emit regionsChanged();
}

void NotationRegionsBeingProcessedModel::clear()
{
    const bool wasEmpty = m_regionsBeingProcessed.empty();
    m_lastChangedRange.clear();
    m_regionsBeingProcessed.clear();

    if (!wasEmpty) {
        emit regionsChanged();
    }
}

void NotationRegionsBeingProcessedModel::updateRegionsBeingProcessed()
{
    if (configuration()->optionB() && !playbackController()->isPlaying()) {
        return;
    }

    const INotationPtr notation = globalContext()->currentNotation();
    if (!notation) {
        return;
    }

    const QTransform matrix = m_notationViewMatrix.value<QTransform>();

    for (auto& pair : m_regionsBeingProcessed) {
        const Part* part = notation->parts()->part(pair.first.partId);
        if (!part) {
            continue;
        }

        RegionInfo& region = pair.second;
        region.rects = calculateRegionRects(part, matrix, region.range);
    }
}

QVariantList NotationRegionsBeingProcessedModel::calculateRegionRects(const Part* part, const QTransform& matrix,
                                                                      const ChangesRange& range) const
{
    const mu::engraving::Score* score = part->score();
    const mu::engraving::MeasureBase* lastMeasure = score->last();
    const Segment* segmentFrom = score->tick2segment(Fraction::fromTicks(range.tickFrom));
    const Segment* segmentTo = score->tick2segment(Fraction::fromTicks(range.tickTo));

    const double lastMeasureEndX = lastMeasure ? lastMeasure->canvasPos().x() + lastMeasure->width() : 0.0;
    const double segmentFromStartX = segmentFrom ? segmentFrom->canvasPos().x() : 0.0;
    const double segmentToEndX = segmentTo ? segmentTo->canvasPos().x() + segmentTo->width() : 0.0;

    QVariantList result;

    for (const Staff* staff : part->staves()) {
        const staff_idx_t staffIdx = staff->idx();

        if (range.staffIdxFrom != muse::nidx && staffIdx < range.staffIdxFrom) {
            continue;
        }

        if (range.staffIdxTo != muse::nidx && staffIdx > range.staffIdxTo) {
            continue;
        }

        for (const System* system : score->systems()) {
            if (system->measures().empty()) {
                continue;
            }

            if (segmentFrom) {
                if (system->last()->tick() < segmentFrom->tick()) {
                    continue;
                }
            }

            if (segmentTo) {
                if (system->first()->tick() > segmentTo->tick()) {
                    continue;
                }
            }

            const mu::engraving::SysStaff* sysStaff = system->staff(staffIdx);
            if (!sysStaff) {
                continue;
            }

            const muse::PointF systemPos = system->canvasPos();
            muse::RectF logicRect = sysStaff->bbox().translated(systemPos);

            if (segmentFrom) {
                const double startX = std::max(segmentFromStartX, logicRect.x());
                logicRect.setLeft(startX);
            }

            if (segmentTo) {
                const double endX = std::min(systemPos.x() + system->width(), segmentToEndX);
                logicRect.setRight(endX);
            } else if (lastMeasure) {
                const double width = std::min(logicRect.width(), lastMeasureEndX - logicRect.x());
                logicRect.setWidth(width);
            }

            result << matrix.mapRect(logicRect.toQRectF()).intersected(m_notationViewRect);
        }
    }

    return result;
}
