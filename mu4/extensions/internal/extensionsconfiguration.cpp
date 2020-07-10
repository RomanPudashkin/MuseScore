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
static const Settings::Key EXTENSION_LIST(module_name, "extensions/extensionList");

void ExtensionsConfiguration::init()
{
    settings()->valueChanged(EXTENSION_LIST).onReceive(nullptr, [this](const Val& val) {
        LOGD() << "EXTENSION_LIST changed: " << val.toString();

        ExtensionList extensionList = parseExtensionConfig(io::pathToQString(val.toString()).toLocal8Bit());
        m_extensionListChanged.send(extensionList);
    });
}

QUrl ExtensionsConfiguration::extensionListUpdateUrl()
{
    return QUrl("http://extensions.musescore.org/4.0/extensions/details.json");
}

ValCh<ExtensionList> ExtensionsConfiguration::extensionList()
{
    ValCh<ExtensionList> result;
    result.val = parseExtensionConfig(io::pathToQString(settings()->value(EXTENSION_LIST).toString()).toLocal8Bit());
    result.ch = m_extensionListChanged;

    return result;
}

Ret ExtensionsConfiguration::setExtensionList(const ExtensionList& extensionList)
{
    QJsonArray jsonArray;
    for (const Extension& extension: extensionList) {
        QJsonObject obj;
        obj[extension.code] = extension.toJson();

        jsonArray << obj;
    }

    QJsonDocument jsonDoc(jsonArray);

    Val value(jsonDoc.toJson(QJsonDocument::Compact).constData());
    settings()->setValue(EXTENSION_LIST, value);

    return make_ret(Err::NoError);
}

ExtensionList ExtensionsConfiguration::parseExtensionConfig(const QByteArray& json) const
{
    ExtensionList result;

    QJsonParseError err;
    QJsonDocument jsodDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError || !jsodDoc.isArray()) {
        return ExtensionList();
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
        extension.version = extMap.value("version").toString();

        result << extension;
    }

    return result;
}
