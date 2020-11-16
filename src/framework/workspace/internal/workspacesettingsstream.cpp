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

AbstractDataPtr WorkspaceSettingsStream::read(Ms::XmlReader& xml) const
{
    std::shared_ptr<SettingsData> data = std::make_shared<SettingsData>();
    data->tag = WorkspaceTag::Preferences;

    while (xml.readNextStartElement()) {
        QStringRef tag(xml.name());
        if ("Preference" == tag) {
            std::string key = xml.attributes().value("name").toString().toStdString();
            Val val(xml.readElementText().toStdString());
            data->vals.insert({ key, val });
        } else {
            xml.skipCurrentElement();
        }
    }

    return data;
}

void WorkspaceSettingsStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    UNUSED(xml)
    UNUSED(data)
    NOT_IMPLEMENTED;
}
