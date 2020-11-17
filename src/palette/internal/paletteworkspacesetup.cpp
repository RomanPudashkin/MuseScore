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
#include "paletteworkspacesetup.h"

#include "log.h"
#include "workspacepalettestream.h"
#include "palette/paletteworkspace.h"
#include "palette/palettecreator.h"

using namespace mu::palette;
using namespace Ms;

void PaletteWorkspaceSetup::setup()
{
    if (!workspaceManager()) {
        return;
    }

    PaletteWorkspace* paletteWorkspace = adapter()->paletteWorkspace();
    auto applyWorkspaceData = [paletteWorkspace](workspace::IWorkspacePtr workspace) {
                                  workspace::AbstractDataPtr data = workspace->data(workspace::WorkspaceTag::Palettes);
                                  if (!data) {
                                      LOGW() << "no palette data in workspace: " << workspace->name();
                                      return false;
                                  }

                                  PaletteWorkspaceDataPtr palette = std::dynamic_pointer_cast<PaletteWorkspaceData>(data);
                                  IF_ASSERT_FAILED(palette) {
                                      return false;
                                  }

                                  paletteWorkspace->setDefaultPaletteTree(palette->tree);
                                  paletteWorkspace->setUserPaletteTree(palette->tree);
                                  return true;
                              };

    RetValCh<workspace::IWorkspacePtr> workspace = workspaceManager()->currentWorkspace();
    if (workspace.val) {
        bool ok = applyWorkspaceData(workspace.val);
        if (!ok) {
            PaletteTreePtr tree(PaletteCreator::newDefaultPaletteTree());
            paletteWorkspace->setUserPaletteTree(tree);
        }
    }

    workspace.ch.onReceive(nullptr, [paletteWorkspace, applyWorkspaceData](workspace::IWorkspacePtr w) {
        bool ok = applyWorkspaceData(w);
        if (!ok) {
            PaletteTreePtr tree(PaletteCreator::newDefaultPaletteTree());
            paletteWorkspace->setUserPaletteTree(tree);
        }
    });
}
