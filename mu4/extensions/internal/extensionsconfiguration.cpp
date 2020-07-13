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
#include "extensionsconfiguration.h"

#include <QDir>
#include <QVariant>

#include "log.h"
#include "settings.h"
#include "extensionserrors.h"

using namespace mu;
using namespace mu::framework;
using namespace mu::extensions;

static std::string module_name("extensions");
static const Settings::Key EXTENSION_JSON(module_name, "extensions/extensionJson");

void ExtensionsConfiguration::init()
{
    settings()->valueChanged(EXTENSION_JSON).onReceive(nullptr, [this](const Val& val) {
        LOGD() << "EXTENSION_Json changed: " << val.toString();

        ExtensionHash extensionHash = parseExtensionConfig(io::pathToQString(val.toString()).toLocal8Bit());
        m_extensionHashChanged.send(extensionHash);
    });
}

QUrl ExtensionsConfiguration::extensionsUpdateUrl() const
{
    return QUrl("http://extensions.musescore.org/4.0/extensions/details.json");
}

QUrl ExtensionsConfiguration::extensionsFileServerUrl() const
{
    return QUrl("http://extensions.musescore.org/4.0/extensions/");
}

ValCh<ExtensionHash> ExtensionsConfiguration::extensions() const
{
    ValCh<ExtensionHash> result;
    result.val = parseExtensionConfig(io::pathToQString(settings()->value(EXTENSION_JSON).toString()).toLocal8Bit());
    result.ch = m_extensionHashChanged;

    return result;
}

Ret ExtensionsConfiguration::setExtensions(const ExtensionHash& extensions) const
{
    QJsonArray jsonArray;
    for (const Extension& extension: extensions) {
        QJsonObject obj;
        obj[extension.code] = extension.toJson();

        jsonArray << obj;
    }

    QJsonDocument jsonDoc(jsonArray);

    Val value(jsonDoc.toJson(QJsonDocument::Compact).constData());
    settings()->setValue(EXTENSION_JSON, value);

    return make_ret(Err::NoError);
}

ExtensionHash ExtensionsConfiguration::parseExtensionConfig(const QByteArray& json) const
{
    ExtensionHash result;

    QJsonParseError err;
    QJsonDocument jsodDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !jsodDoc.isArray()) {
        return ExtensionHash();
    }

    QVariantList extensions = jsodDoc.array().toVariantList();
    for (const QVariant& extensionObj: extensions) {
        QMap<QString, QVariant> value = extensionObj.toMap();
        QVariantMap extMap = value.first().toMap();

        Extension extension;
        extension.code = value.keys().first();
        extension.name = extMap.value("name").toString();
        extension.description = extMap.value("description").toString();
        extension.fileName = extMap.value("fileName").toString();
        extension.fileSize = extMap.value("fileSize").toDouble();
        extension.version = QVersionNumber::fromString(extMap.value("version").toString());
        extension.status = static_cast<ExtensionStatus::Status>( extMap.value("status").toInt());

        result.insert(extension.code, extension);
    }

    return result;
}

QString mu::extensions::ExtensionsConfiguration::extensionsSharePath() const
{
    return io::pathToQString(globalConfiguration()->sharePath() + "/extensions");
}

QString ExtensionsConfiguration::extensionsDataPath() const
{
    return io::pathToQString(globalConfiguration()->dataPath() + "/extensions");
}

QStringList ExtensionsConfiguration::workspacesPaths() const
{
    QStringList paths;

    ExtensionHash extensions = this->extensions().val;

    QString extensionsPath = extensionsSharePath();
    for (const Extension& extension: extensions.values()) {
        QDir extensionWorkspaceDir(extensionsPath + "/" + extension.code + "/workspaces");
        if (extensionWorkspaceDir.exists()) {
            paths << extensionWorkspaceDir.absolutePath();
        }
    }

    return paths;
}
