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
#include "workspacepalettestream.h"

#include "log.h"

using namespace mu::palette;
using namespace mu::workspace;

AbstractDataPtr WorkspacePaletteStream::read(Ms::XmlReader& xml) const
{
    PaletteWorkspaceDataPtr palette = std::make_shared<PaletteWorkspaceData>();
    palette->tag = WorkspaceTag::Palettes;
    palette->tree = std::make_shared<Ms::PaletteTree>();
    palette->tree->read(xml);

    return palette;
}

void WorkspacePaletteStream::write(Ms::XmlWriter& xml, AbstractDataPtr data) const
{
    PaletteWorkspaceDataPtr palette = std::dynamic_pointer_cast<PaletteWorkspaceData>(data);
    IF_ASSERT_FAILED(palette) {
        return;
    }

    palette->tree->write(xml);
}
