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

#include <QDir>

#include "log.h"
#include "mscore/downloadUtils.h"
#include "extensionserrors.h"

using namespace mu;
using namespace mu::extensions;

Ret ExtensionsController::refreshExtensions()
{
    Ms::DownloadUtils* js = new Ms::DownloadUtils();
    js->setTarget(configuration()->extensionsUpdateUrl().toString());
    js->download();

    QByteArray json = js->returnData();
    RetVal<ExtensionHash> actualExtensions = parseExtensionConfig(json);

    if (!actualExtensions.ret) {
        return actualExtensions.ret;
    }

    ExtensionHash savedExtensions = configuration()->extensions().val;

    ExtensionHash resultExtensions = savedExtensions;

    for (Extension& extension : actualExtensions.val) {
        if (savedExtensions.contains(extension.code)) {
            Extension& savedExtension = savedExtensions[extension.code];

            if (!isExtensionExists(extension.code)) {
                savedExtension.status = ExtensionStatus::Status::NoInstalled;
                continue;
            }

            if (savedExtension.version < extension.version) {
                savedExtension.status = ExtensionStatus::Status::NeedUpdate;
            }

            savedExtension.status = ExtensionStatus::Status::Installed;
        } else {
            extension.status = ExtensionStatus::Status::NoInstalled;
            resultExtensions.insert(extension.code, extension);
        }
    }

    Ret ret = configuration()->setExtensionHash(resultExtensions);
    return ret;
}

ValCh<ExtensionHash> ExtensionsController::extensions()
{
    return configuration()->extensions();
}

Ret ExtensionsController::install(const QString &extensionCode)
{
    ValCh<ExtensionHash> extensions = configuration()->extensions();
    QString fileName = extensions.val.value(extensionCode).fileName;

    QDir extensionsDir = io::pathToQString(globalConfiguration()->dataPath()) + "/extensions";
    if (!extensionsDir.exists()) {
        extensionsDir.mkpath(extensionsDir.absolutePath());
    }

    QString extensionArchivePath = extensionsDir.absolutePath() + "/" + fileName;

    Ms::DownloadUtils* js = new Ms::DownloadUtils();
    js->setTarget(configuration()->extensionsFileServerUrl().toString() + fileName);
    js->setLocalFile(extensionArchivePath);
    js->download(true);

    if (!js->saveFile()) {
        LOGE() << "Error save file";
        return make_ret(Err::ErrorLoadingExtension);
    }

    QDir extensionsShareDir(io::pathToQString(globalConfiguration()->sharePath()) + "/extensions");
    if (!extensionsShareDir.exists()) {
        extensionsShareDir.mkpath(extensionsShareDir.absolutePath());
    }

    Ret unpack = extensionUnpacker()->unpack(extensionArchivePath, extensionsShareDir.absolutePath());
    if (!unpack) {
        LOGE() << "Error unpack" << unpack.code();
        return unpack;
    }

    ExtensionHash extensionHash = this->extensions().val;

    extensionHash[extensionCode].status = ExtensionStatus::Status::Installed;

    Ret ret = configuration()->setExtensionHash(extensionHash);
    if (!ret) {
        return ret;
    }

    m_extensionChanged.send(extensionHash[extensionCode]);

    return make_ret(Err::NoError);
}

RetCh<Extension> ExtensionsController::extensionChanged()
{
    RetCh<Extension> result;
    result.ret = make_ret(Err::NoError);
    result.ch = m_extensionChanged;
    return result;
}

RetVal<ExtensionHash> ExtensionsController::parseExtensionConfig(const QByteArray &json) const
{
    RetVal<ExtensionHash> result;

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
        extension.version = QVersionNumber::fromString(value.value("version").toString());
        extension.status = ExtensionStatus::Status::Undefined;

        result.val.insert(key, extension);
    }

    return result;
}

bool ExtensionsController::isExtensionExists(const QString &extensionCode) const
{
    QDir extensionDir = io::pathToQString(globalConfiguration()->sharePath()) + "/extensions/" + extensionCode;
    return extensionDir.exists();
}
