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
#include "workspacetoolbarstream.h"

#include "libmscore/xml.h"
#include "log.h"

using namespace mu::workspace;

AbstractDataPtr WorkspaceToolbarStream::read(Ms::XmlReader& xml) const
{
    std::shared_ptr<ToolbarData> data = std::make_shared<ToolbarData>();

    data->tag = WorkspaceTag::Toolbar;
    data->name = xml.attributes().value("name").toString().toStdString();

    while (xml.readNextStartElement()) {
        QStringRef tag(xml.name());
        if ("action" == tag) {
            data->actions.push_back(xml.readElementText().toStdString());
        } else {
            xml.skipCurrentElement();
        }
    }

    return data;
}

void WorkspaceToolbarStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    UNUSED(xml)
    UNUSED(data)
    NOT_IMPLEMENTED;
}
