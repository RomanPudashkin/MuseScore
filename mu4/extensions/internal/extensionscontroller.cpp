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

#include <QBuffer>
#include <QtConcurrent>

#include "log.h"
#include "translation.h"
#include "extensionserrors.h"

using namespace mu;
using namespace mu::extensions;
using namespace mu::framework;

void ExtensionsController::init()
{
    fsOperation()->makePath(configuration()->extensionsSharePath());
    fsOperation()->makePath(configuration()->extensionsDataPath());
}

Ret ExtensionsController::refreshExtensions()
{
    QBuffer buff;
    INetworkManagerPtr networkManagerPtr = networkManagerCreator()->newNetworkManager();

    Ret getExtensionsInfo = networkManagerPtr->get(configuration()->extensionsUpdateUrl(), &buff);

    if (!getExtensionsInfo) {
        LOGE() << "Error get extensions" << getExtensionsInfo.code() << getExtensionsInfo.text();
        return getExtensionsInfo;
    }

    QByteArray json = buff.data();
    RetVal<ExtensionsHash> actualExtensions = parseExtensionConfig(json);

    if (!actualExtensions.ret) {
        return actualExtensions.ret;
    }

    ExtensionsHash savedExtensions = configuration()->extensions().val;

    ExtensionsHash resultExtensions = savedExtensions;

    for (Extension& extension : actualExtensions.val) {
        if (!resultExtensions.contains(extension.code)) {
            extension.status = ExtensionStatus::Status::NoInstalled;
            resultExtensions.insert(extension.code, extension);
            continue;
        }

        Extension& savedExtension = resultExtensions[extension.code];

        if (!isExtensionExists(extension.code)) {
            savedExtension.status = ExtensionStatus::Status::NoInstalled;
            continue;
        }

        if (savedExtension.version < extension.version) {
            savedExtension.status = ExtensionStatus::Status::NeedUpdate;
        }

        savedExtension.status = ExtensionStatus::Status::Installed;
    }

    Ret ret = configuration()->setExtensions(resultExtensions);
    return ret;
}

ValCh<ExtensionsHash> ExtensionsController::extensions() const
{
    ValCh<ExtensionsHash> extensionHash = configuration()->extensions();
    extensionHash.val = correctExtensionsStates(extensionHash.val).val;

    return extensionHash;
}

RetCh<ExtensionProgressStatus> ExtensionsController::install(const QString& extensionCode)
{
    RetCh<ExtensionProgressStatus> result;
    result.ret = make_ret(Err::NoError);
    result.ch = m_extensionProgressStatus;

    QtConcurrent::run(this, &ExtensionsController::th_install, extensionCode, m_extensionProgressStatus,
                      [this](const QString& extensionCode, const Ret& ret) -> void {
        if (!ret) {
            return;
        }

        ExtensionsHash extensionHash = this->extensions().val;

        extensionHash[extensionCode].status = ExtensionStatus::Status::Installed;
        extensionHash[extensionCode].types = extensionTypes(extensionCode);

        Ret updateConfigRet = configuration()->setExtensions(extensionHash);
        if (!updateConfigRet) {
            return; // TODO
        }

        m_extensionChanged.send(extensionHash[extensionCode]);

        m_extensionProgressStatus.close();
    });

    return result;
}

RetCh<ExtensionProgressStatus> ExtensionsController::update(const QString& extensionCode)
{
    RetCh<ExtensionProgressStatus> result;
    result.ret = make_ret(Err::NoError);
    result.ch = m_extensionProgressStatus;

    QtConcurrent::run(this, &ExtensionsController::th_install, extensionCode, m_extensionProgressStatus,
                      [this](const QString& extensionCode, const Ret& ret) -> void {
        if (!ret) {
            return;
        }

        ExtensionsHash extensionHash = extensions().val;

        extensionHash[extensionCode].status = ExtensionStatus::Status::Installed;
        extensionHash[extensionCode].types = extensionTypes(extensionCode);

        Ret updateConfigRet = configuration()->setExtensions(extensionHash);
        if (!updateConfigRet) {
            return; // TODO
        }

        m_extensionChanged.send(extensionHash[extensionCode]);

        m_extensionProgressStatus.close();
    });

    return result;
}

Ret ExtensionsController::uninstall(const QString& extensionCode)
{
    ExtensionsHash extensionHash = extensions().val;

    if (!extensionHash.contains(extensionCode)) {
        return make_ret(Err::ErrorExtensionNotFound);
    }

    Ret remove = removeExtension(extensionCode);
    if (!remove) {
        return remove;
    }

    extensionHash[extensionCode].status = ExtensionStatus::Status::NoInstalled;
    Ret ret = configuration()->setExtensions(extensionHash);
    if (!ret) {
        return ret;
    }

    m_extensionChanged.send(extensionHash[extensionCode]);

    return make_ret(Err::NoError);
}

RetCh<Extension> ExtensionsController::extensionChanged() const
{
    RetCh<Extension> result;
    result.ret = make_ret(Err::NoError);
    result.ch = m_extensionChanged;
    return result;
}

RetVal<ExtensionsHash> ExtensionsController::parseExtensionConfig(const QByteArray& json) const
{
    RetVal<ExtensionsHash> result;

    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        result.ret = make_ret(Err::ErrorParseConfig);
        return result;
    }

    result.ret = make_ret(Err::NoError);
    QStringList extensions = jsonDoc.object().keys();
    for (const QString& key : extensions) {
        if (!jsonDoc.object().value(key).isObject()) {
            continue;
        }

        QJsonObject value = jsonDoc.object().value(key).toObject();

        Extension extension;
        extension.code = key;
        extension.name = value.value("name").toString();
        extension.description = value.value("description").toString();
        extension.fileName = value.value("file_name").toString();
        extension.fileSize = value.value("file_size").toDouble();
        extension.version = QVersionNumber::fromString(value.value("version").toString());
        extension.types = {};

        result.val.insert(key, extension);
    }

    return result;
}

bool ExtensionsController::isExtensionExists(const QString& extensionCode) const
{
    return fsOperation()->exists(configuration()->extensionsSharePath() + "/" + extensionCode);
}

RetVal<ExtensionsHash> ExtensionsController::correctExtensionsStates(ExtensionsHash& extensions) const
{
    RetVal<ExtensionsHash> result;
    bool isNeedUpdate = false;

    for (Extension& extension: extensions) {
        if (extension.status == ExtensionStatus::Status::Installed && !isExtensionExists(extension.code)) {
            extension.status = ExtensionStatus::Status::NoInstalled;
            isNeedUpdate = true;
        }

        extension.types = extensionTypes(extension.code);
    }

    if (isNeedUpdate) {
        Ret ret = configuration()->setExtensions(extensions);
        if (!ret) {
            result.ret = ret;
            return result;
        }
    }

    result.ret = make_ret(Err::NoError);
    result.val = extensions;
    return result;
}

RetVal<QString> ExtensionsController::downloadExtension(const QString& extensionCode,
                                                        async::Channel<ExtensionProgressStatus>& progressChannel) const
{
    RetVal<QString> result;

    ValCh<ExtensionsHash> extensions = configuration()->extensions();
    QString fileName = extensions.val.value(extensionCode).fileName;

    QString extensionArchivePath = configuration()->extensionsDataPath() + "/" + fileName;

    QBuffer buff;
    INetworkManagerPtr networkManagerPtr = networkManagerCreator()->newNetworkManager();

    async::Channel<Progress> downloadChannel = networkManagerPtr->downloadProgressChannel();
    downloadChannel.onReceive(new deto::async::Asyncable(), [&progressChannel](const Progress& progress) {
        progressChannel.send(ExtensionProgressStatus(qtrc("extensions", "Downloading..."), progress.current,
                                                     progress.total));
    });

    Ret getExtension = networkManagerPtr->get(configuration()->extensionsFileServerUrl().toString() + fileName, &buff);

    if (!getExtension) {
        LOGE() << "Error save file";
        result.ret = make_ret(Err::ErrorLoadingExtension);
        return result;
    }

    QFile file(extensionArchivePath);
    file.open(QIODevice::WriteOnly);
    file.write(buff.data());
    file.close();

    result.ret = make_ret(Err::NoError);
    result.val = extensionArchivePath;
    return result;
}

Ret ExtensionsController::removeExtension(const QString& extensionCode) const
{
    QString extensionPath = configuration()->extensionsSharePath() + "/" + extensionCode;
    Ret ret = fsOperation()->remove(extensionPath);
    if (!ret) {
        return make_ret(Err::ErrorRemoveExtensionDirectory);
    }

    return make_ret(Err::NoError);
}

Extension::ExtensionTypes ExtensionsController::extensionTypes(const QString& extensionCode) const
{
    Extension::ExtensionTypes result;
    QString workspacesPath(configuration()->extensionsSharePath() + "/" + extensionCode + "/workspaces");
    RetVal<QStringList> files
        = fsOperation()->directoryFileList(workspacesPath, { QString("*.workspace") }, QDir::Files);
    if (files.ret && !files.val.empty()) {
        result.setFlag(Extension::Workspaces);
    }

    return result;
}

void ExtensionsController::th_install(const QString& extensionCode,
                                      async::Channel<ExtensionProgressStatus> progressChannel,
                                      std::function<void(const QString&, const Ret&)> callback)
{
    progressChannel.send(ExtensionProgressStatus(qtrc("extensions", "Analysing..."), true));

    RetVal<QString> download = downloadExtension(extensionCode, progressChannel);
    if (!download.ret) {
        callback(extensionCode, download.ret);
        return;
    }

    progressChannel.send(ExtensionProgressStatus(qtrc("extensions", "Analysing..."), true));

    QString extensionArchivePath = download.val;

    Ret unpack = extensionUnpacker()->unpack(extensionArchivePath, configuration()->extensionsSharePath());
    if (!unpack) {
        LOGE() << "Error unpack" << unpack.code();
        callback(extensionCode, unpack);
        return;
    }

    fsOperation()->remove(extensionArchivePath);

    callback(extensionCode, make_ret(Err::NoError));
}

void ExtensionsController::th_update(const QString& extensionCode, async::Channel<ExtensionProgressStatus> progressChannel,
                                     std::function<void(const QString&, const Ret&)> callback)
{
    progressChannel.send(ExtensionProgressStatus(qtrc("extensions", "Analysing..."), true));

    RetVal<QString> download = downloadExtension(extensionCode, progressChannel);
    if (!download.ret) {
        callback(extensionCode, download.ret);
    }

    progressChannel.send(ExtensionProgressStatus(qtrc("extensions", "Analysing..."), true));

    QString extensionArchivePath = download.val;

    Ret remove = removeExtension(extensionCode);
    if (!remove) {
        callback(extensionCode, remove);
    }

    Ret unpack = extensionUnpacker()->unpack(extensionArchivePath, configuration()->extensionsSharePath());
    if (!unpack) {
        LOGE() << "Error unpack" << unpack.code();
        callback(extensionCode, unpack);
    }

    fsOperation()->remove(extensionArchivePath);

    callback(extensionCode, make_ret(Err::NoError));
}
