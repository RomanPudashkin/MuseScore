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

#include <QAbstractListModel>

#include "async/asyncable.h"

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"
#include "notation/inotationconfiguration.h"
#include "playback/iplaybackcontroller.h"
#include "audio/iplayback.h"

namespace mu::notation {
class NotationRegionsBeingProcessedModel : public QAbstractListModel, public muse::async::Asyncable
{
    Q_OBJECT

    muse::Inject<INotationConfiguration> configuration;
    muse::Inject<mu::context::IGlobalContext> globalContext;
    muse::Inject<mu::playback::IPlaybackController> playbackController;
    muse::Inject<muse::audio::IPlayback> playback;

    Q_PROPERTY(QVariant notationViewMatrix READ notationViewMatrix WRITE setNotationViewMatrix NOTIFY notationViewMatrixChanged)

    Q_PROPERTY(bool isEmpty READ isEmpty NOTIFY isEmptyChanged)

    Q_PROPERTY(QColor progressBackgroundColor READ progressBackgroundColor CONSTANT)
    Q_PROPERTY(QColor progressFontColor READ progressFontColor CONSTANT)

public:
    explicit NotationRegionsBeingProcessedModel(QObject* parent = nullptr);

    QVariant data(const QModelIndex& index, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load();

    QVariant notationViewMatrix() const;

    bool isEmpty() const;

    QColor progressBackgroundColor() const;
    QColor progressFontColor() const;

public slots:
    void setNotationViewMatrix(const QVariant& matrix);

signals:
    void notationViewMatrixChanged();

    void isEmptyChanged();

private:
    enum Roles {
        RectRole = Qt::UserRole + 1,
        ProgressRole,
    };

    struct RegionInfo {
        InstrumentTrackId trackId;
        QRectF rect;

        bool operator==(const RegionInfo& r) const
        {
            return trackId == r.trackId && rect == r.rect;
        }
    };

    struct TickRange {
        int tickFrom = -1;
        int tickTo = -1;

        bool operator==(const TickRange& r) const
        {
            return tickFrom == r.tickFrom && tickTo == r.tickTo;
        }

        void clear()
        {
            *this = TickRange();
        }
    };

    struct TrackInfo {
        std::vector<TickRange> ranges;
        int progress = 0; // 0 - 100%

        bool operator==(const TrackInfo& t) const
        {
            return ranges == t.ranges && progress == t.progress;
        }
    };

    using TracksBeingProcessed = std::map<InstrumentTrackId, TrackInfo>;

    void clear();

    void onMasterNotationChanged();

    void onOnlineSoundsChanged();

    void onTrackAdded(const muse::audio::TrackId trackId);
    void onTrackRemoved(const muse::audio::TrackId trackId);

    void onProgressStarted(const InstrumentTrackId& instrumentTrackId);
    void onChunksReceived(const InstrumentTrackId& instrumentTrackId,
                          const std::vector<muse::audio::InputProcessingProgress::ChunkInfo>& chunks);
    void onProgressChanged(const InstrumentTrackId& instrumentTrackId, int progress);
    void onProgressFinished(const InstrumentTrackId& instrumentTrackId, const muse::Ret& ret);

    void updateRegionsBeingProcessed(const TracksBeingProcessed& tracks);

    std::vector<QRectF> calculateRects(const Part* part, const std::vector<TickRange>& ranges) const;

    QVariant m_notationViewMatrix;

    std::set<muse::audio::TrackId> m_onlineSounds;
    TickRange m_lastChangedRange;
    TracksBeingProcessed m_tracksBeingProcessed;
    QList<RegionInfo> m_regions; // main model
};
}
