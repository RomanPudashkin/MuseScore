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
#include "extensionscontroller.h"

#include "mscore/downloadUtils.h"
#include "extensionserrors.h"

using namespace mu;
using namespace mu::extensions;

Ret ExtensionsController::refreshExtensionList()
{
    Ms::DownloadUtils* js = new Ms::DownloadUtils();
    js->setTarget(configuration()->extensionListUpdateUrl().toString());
    js->download();

    QByteArray json = js->returnData();
    RetVal<ExtensionList> extensions = parseExtensionConfig(json);

    if (!extensions.ret) {
        return extensions.ret;
    }

    Ret ret = configuration()->setExtensionList(extensions.val);
    return ret;
}

ValCh<ExtensionList> ExtensionsController::extensionList()
{
    return configuration()->extensionList();
}

RetVal<ExtensionList> ExtensionsController::parseExtensionConfig(const QByteArray &json) const
{
    RetVal<ExtensionList> result;

    // parse the json file
    QJsonParseError err;
    QJsonDocument jsodDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !jsodDoc.isObject()) {
        result.ret = make_ret(Err::ErrorParseConfig);
        return result;
    }

    result.ret = make_ret(Err::NoError);
    QStringList extensions = jsodDoc.object().keys();
    for (const QString& key : extensions) {
        if (!jsodDoc.object().value(key).isObject()) {
            continue;
        }

        QJsonObject value = jsodDoc.object().value(key).toObject();

        Extension extension;
        extension.code = key;
        extension.name = value.value("name").toString();
        extension.description = value.value("description").toString();
        extension.fileName = value.value("file_name").toString();
        extension.fileSize = value.value("file_size").toDouble();
        extension.version = value.value("version").toString();

        result.val << extension;
    }

    return result;
}
