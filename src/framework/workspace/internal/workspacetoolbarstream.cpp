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

static const QString ACTION_TAG("action");

AbstractDataPtr WorkspaceToolbarStream::read(Ms::XmlReader& xml) const
{
    ToolbarDataPtr toolbar = std::make_shared<ToolbarData>();

    toolbar->tag = WorkspaceTag::Toolbar;
    toolbar->name = xml.attributes().value("name").toString().toStdString();

    while (xml.readNextStartElement()) {
        QStringRef tag(xml.name());
        if (tag == ACTION_TAG) {
            toolbar->actions.push_back(xml.readElementText().toStdString());
        } else {
            xml.skipCurrentElement();
        }
    }

    return toolbar;
}

void WorkspaceToolbarStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    ToolbarDataPtr toolbar = std::dynamic_pointer_cast<ToolbarData>(data);
    IF_ASSERT_FAILED(toolbar) {
        return;
    }

    xml.stag(QString("Toolbar name=\"%1\"").arg(QString::fromStdString(toolbar->name)));

    for (const std::string& actionName : toolbar->actions) {
        xml.tag(ACTION_TAG, QString::fromStdString(actionName));
    }

    xml.etag();
}
