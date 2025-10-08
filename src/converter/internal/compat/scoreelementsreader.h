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

#include "project/inotationreader.h"

#include "modularity/ioc.h"

#include "global/io/ifilesystem.h"
#include "global/types/retval.h"

#include "engraving/dom/sig.h"

#include "backendtypes.h"

namespace mu::engraving {
class MasterScore;
}

namespace mu::converter {
class ScoreElementsReader : public project::INotationReader, public muse::Injectable
{
    muse::Inject<muse::io::IFileSystem> fileSystem = { this };

public:
    ScoreElementsReader(const muse::modularity::ContextPtr& iocCtx)
        : muse::Injectable(iocCtx) {}

    muse::Ret read(mu::engraving::MasterScore* score, const muse::io::path_t& path, const Options& options = Options()) override;

private:
    struct ScoreInfo {
        size_t measures = 0;
        std::map<muse::ID /*partId*/, size_t> partStaves;
        mu::engraving::TimeSigMap timeSigMap;
        ElementMap chordRestElements;
        ElementMap otherElements;
    };

    muse::RetVal<ScoreInfo> readScoreInfo(const muse::io::path_t& path);

    void addParts(const ScoreInfo& info, mu::engraving::MasterScore* score);
};
}
