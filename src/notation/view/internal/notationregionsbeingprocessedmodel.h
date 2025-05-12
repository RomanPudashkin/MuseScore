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

#pragma once

#include <QObject>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"
#include "playback/iplaybackcontroller.h"
#include "audio/iplayback.h"

namespace mu::notation {
class NotationRegionsBeingProcessedModel : public QObject, public muse::async::Asyncable
{
    Q_OBJECT

    muse::Inject<INotationConfiguration> configuration;
    muse::Inject<mu::context::IGlobalContext> globalContext;
    muse::Inject<mu::playback::IPlaybackController> playbackController;
    muse::Inject<muse::audio::IPlayback> playback;

    Q_PROPERTY(QRectF notationViewRect READ notationViewRect WRITE setNotationViewRect NOTIFY notationViewRectChanged)
    Q_PROPERTY(QVariant notationViewMatrix READ notationViewMatrix WRITE setNotationViewMatrix NOTIFY notationViewMatrixChanged)

    Q_PROPERTY(QVariantList regions READ regions NOTIFY regionsChanged)
    Q_PROPERTY(QColor regionColor READ regionColor CONSTANT)

public:
    explicit NotationRegionsBeingProcessedModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    QRectF notationViewRect() const;
    QVariant notationViewMatrix() const;

    QVariantList regions() const;
    QColor regionColor() const;

public slots:
    void setNotationViewRect(const QRectF& rect);
    void setNotationViewMatrix(const QVariant& matrix);

signals:
    void notationViewRectChanged();
    void notationViewMatrixChanged();

    void regionsChanged();

private:
    struct RegionInfo {
        ChangesRange range;
        QVariantList rects;

        bool operator==(const RegionInfo& r) const
        {
            return range == r.range && rects == r.rects;
        }
    };

    using Regions = std::map<InstrumentTrackId, RegionInfo>;

    void clear();

    void onMasterNotationChanged();
    void onNotationChanged();

    void onTrackAdded(const muse::audio::TrackId trackId);
    void onTrackRemoved(const muse::audio::TrackId trackId);

    void onProcessStarted(const InstrumentTrackId& instrumentTrackId);
    void onProcessFinished(const InstrumentTrackId& instrumentTrackId);

    void updateRegionsBeingProcessed();
    QVariantList calculateRegionRects(const Part* part, const QTransform& matrix, const ChangesRange& range) const;

    QRectF m_notationViewRect;
    QVariant m_notationViewMatrix;

    ChangesRange m_lastChangedRange;
    Regions m_regionsBeingProcessed;
};
}
