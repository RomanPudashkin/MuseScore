//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "templatesrepository.h"

#include "log.h"

using namespace mu;
using namespace mu::domain::notation;
using namespace mu::userscores;
using namespace mu::framework;

RetVal<Templates> TemplatesRepository::templates() const
{
    Templates result;

    for (const QString& dirPath: configuration()->templatesDirPaths()) {
        QStringList filters { "*.mscz", "*.mscx" };
        RetVal<QStringList> files = fsOperations()->scanFiles(dirPath, filters, IFsOperations::ScanMode::IncludeSubdirs);

        if (!files.ret) {
            LOGE() << files.ret.toString();
        }

        result << loadTemplates(files.val);
    }

    return RetVal<Templates>::make_ok(result);
}

Templates TemplatesRepository::loadTemplates(const QStringList& filePaths) const
{
    Templates result;

    for (const QString& pathToFile: filePaths) {
        RetVal<Meta> meta = msczReader()->readMeta(io::pathFromQString(pathToFile));

        if (!meta.ret) {
            LOGE() << meta.ret.toString();
            continue;
        }

        Template templ(meta.val);
        templ.categoryTitle = correctedTitle(fsOperations()->dirName(pathToFile));

        result << templ;
    }

    return result;
}

QString TemplatesRepository::correctedTitle(const QString& title) const
{
    QString corrected = title;

    if (!corrected.isEmpty() && corrected[0].isNumber()) {
        constexpr int NUMBER_LENGTH = 3;
        corrected = corrected.mid(NUMBER_LENGTH);
    }

    return corrected.replace('_', ' ');
}
