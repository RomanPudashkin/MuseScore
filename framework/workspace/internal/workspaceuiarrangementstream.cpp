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
#include "workspaceuiarrangementstream.h"

#include "libmscore/xml.h"
#include "log.h"

using namespace mu::workspace;

static const QString STATE_TAG("State");

AbstractDataPtr WorkspaceUiArrangementStream::read(Ms::XmlReader& xml) const
{
    UiArrangementDataPtr uiArrangement = std::make_shared<UiArrangementData>();
    uiArrangement->tag = WorkspaceTag::UiArrangement;

    while (xml.readNextStartElement()) {
        QStringRef tag(xml.name());
        if (tag == STATE_TAG) {
            uiArrangement->mainWindowState = xml.readElementText().toStdString();
        } else {
            xml.skipCurrentElement();
        }
    }

    return uiArrangement;
}

void WorkspaceUiArrangementStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    UiArrangementDataPtr uiArrangement = std::dynamic_pointer_cast<UiArrangementData>(data);
    IF_ASSERT_FAILED(uiArrangement) {
        return;
    }

    xml.tag(STATE_TAG, QString::fromStdString(uiArrangement->mainWindowState));
}
