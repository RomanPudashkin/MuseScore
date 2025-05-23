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

#include "onlinesoundsstatusmodel.h"

using namespace mu::playback;
using namespace muse::audio;

OnlineSoundsStatusModel::OnlineSoundsStatusModel(QObject* parent)
    : QObject(parent)
{
}

void OnlineSoundsStatusModel::load()
{
    updateHasOnlineTracks();

    playbackController()->trackAdded().onReceive(this, [this](const TrackId) {
        updateHasOnlineTracks();
    });

    playbackController()->trackRemoved().onReceive(this, [this](const TrackId) {
        updateHasOnlineTracks();
    });

    muse::Progress progress = playbackController()->onlineTracksProcessingProgress();
    setProcessing(progress.isStarted());

    progress.started().onNotify(this, [this]() {
        setProcessing(true);
    });

    progress.finished().onReceive(this, [this](const muse::ProgressResult&) {
        setProcessing(false);
    });
}

bool OnlineSoundsStatusModel::hasOnlineSounds() const
{
    return m_hasOnlineSounds;
}

bool OnlineSoundsStatusModel::processing() const
{
    return m_processing;
}

void OnlineSoundsStatusModel::updateHasOnlineTracks()
{
    const bool value = !playbackController()->onlineTracks().empty();
    if (m_hasOnlineSounds == value) {
        return;
    }

    m_hasOnlineSounds = value;
    emit hasOnlineSoundsChanged();
}

void OnlineSoundsStatusModel::setProcessing(bool processing)
{
    if (m_processing == processing) {
        return;
    }

    m_processing = processing;
    emit processingChanged();
}
