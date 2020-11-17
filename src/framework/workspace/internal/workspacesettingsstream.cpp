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
#include "workspacesettingsstream.h"

#include <QXmlStreamReader>

#include "../workspacetypes.h"

#include "libmscore/xml.h"

#include "log.h"

using namespace mu::workspace;

static const QString SETTINGS_TAG("Preference");

AbstractDataPtr WorkspaceSettingsStream::read(Ms::XmlReader& xml) const
{
    SettingsDataPtr settings = std::make_shared<SettingsData>();
    settings->tag = WorkspaceTag::Settings;

    while (xml.readNextStartElement()) {
        QStringRef tag(xml.name());
        if (tag == SETTINGS_TAG) {
            std::string key = xml.attributes().value("name").toString().toStdString();
            Val val(xml.readElementText().toStdString());
            settings->vals.insert({ key, val });
        } else {
            xml.skipCurrentElement();
        }
    }

    return settings;
}

void WorkspaceSettingsStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    SettingsDataPtr settings = std::dynamic_pointer_cast<SettingsData>(data);
    IF_ASSERT_FAILED(settings) {
        return;
    }

    xml.stag(SETTINGS_TAG);

    for (auto it = settings->vals.begin(); it != settings->vals.end(); ++it) {
        QString settingKey = "Preference name=\"" + QString::fromStdString(it->first) + "\"";
        QVariant settingValue = it->second.toQVariant();

        if (settingValue.isNull()) {
            continue;
        }

        xml.tag(settingKey, settingValue);
    }

    xml.etag();
}
