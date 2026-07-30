/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2026 MuseScore Limited and others
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

#include "importfiletoscoredevtoolsmodel.h"

#include "actions/actiontypes.h"
#include "project/projecterrors.h"

using namespace mu::project;
using namespace muse;
using namespace muse::toast;
using namespace muse::actions;

static io::paths_t toFileNames(const io::paths_t& paths)
{
    io::paths_t fileNames;
    fileNames.reserve(paths.size());
    for (const io::path_t& path : paths) {
        fileNames.push_back(io::filename(path));
    }
    return fileNames;
}

ImportFileToScoreDevToolsModel::ImportFileToScoreDevToolsModel(QObject* parent)
    : QObject(parent), muse::Contextable(muse::iocCtxForQmlObject(this))
{
}

void ImportFileToScoreDevToolsModel::init()
{
    importFileToScoreScenario()->importFinished().onReceive(this, [this](const Ret& ret, const io::path_t& path) {
        if (!ret) {
            std::string message = ret.toString();
            std::string failedFiles = io::pathsToString(toFileNames(ret.data<io::paths_t>(IMPORT_FAILED_FILES_KEY, {})), "\n");
            if (!failedFiles.empty()) {
                message += "\n\nFailed to import the following files:\n" + failedFiles;
            }

            toastService()->show("Import failed", message, muse::ui::IconCode::Code::ERROR, true, { ToastAction { "Dismiss" } });
            return;
        }

        toastService()->show("Your score is ready!", "", muse::ui::IconCode::Code::TICK, true, {
            ToastAction { "Open score", ToastActionCode::Custom },
            ToastAction { "Dismiss" }
        }).onResolve(this, [this, path](ToastActionCode code) {
            if (code == ToastActionCode::Custom) {
                dispatcher()->dispatch("file-open", ActionData::make_arg1<QUrl>(path.toQUrl()));
            }
        });
    });
}

void ImportFileToScoreDevToolsModel::selectAndImportFiles()
{
    importFileToScoreScenario()->selectFilesToImport()
    .onResolve(this, [this](const ImportSelection& selection) {
        bool started = importFileToScoreScenario()->importFiles(selection.type, selection.paths);
        if (started) {
            std::string message = "Importing the following files:\n" + io::pathsToString(toFileNames(selection.paths), "\n");
            toastService()->show("Import started", message, muse::ui::IconCode::Code::INFO, true, { ToastAction { "Dismiss" } });
        }
    });
}
