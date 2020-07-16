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

#include "inspectorsactions.h"

#include "shortcuts/shortcutstypes.h"

using namespace mu::inspectors;
using namespace mu::actions;
using namespace mu::shortcuts;

const std::vector<Action> InspectorsActions::m_actions = {
    Action("hraster",
           QT_TRANSLATE_NOOP("action", "Enable snap to horizontal grid"),
           ShortcutContext::Any
           ),
    Action("vraster",
           QT_TRANSLATE_NOOP("action", "Enable snap to vertical grid"),
           ShortcutContext::Any
           ),
    Action("config-raster",
           QT_TRANSLATE_NOOP("action", "Configure grid"),
           ShortcutContext::Any
           ),
    Action("show-articulation-properties",
           QT_TRANSLATE_NOOP("action", "Edit articulation properties"),
           ShortcutContext::Any
           ),
    Action("page-settings",
           QT_TRANSLATE_NOOP("action", "Page settings"),
           ShortcutContext::Any
           ),
    Action("edit-style",
           QT_TRANSLATE_NOOP("action", "Edit style"),
           ShortcutContext::Any
           ),
    Action("show-invisible",
           QT_TRANSLATE_NOOP("action", "Show invisible"),
           ShortcutContext::Any
           ),
    Action("show-unprintable",
           QT_TRANSLATE_NOOP("action", "Show unprintable"),
           ShortcutContext::Any
           ),
    Action("show-frames",
           QT_TRANSLATE_NOOP("action", "Show frames"),
           ShortcutContext::Any
           ),
    Action("show-pageborders",
           QT_TRANSLATE_NOOP("action", "Show page margins"),
           ShortcutContext::Any
           ),
    Action("show-keys",
           QT_TRANSLATE_NOOP("action", "Insert special characters"),
           ShortcutContext::Any
           ),
    Action("show-staff-text-properties",
           QT_TRANSLATE_NOOP("action", "Show staff text properties"),
           ShortcutContext::Any
           )
};

const Action& InspectorsActions::action(const ActionName& name) const
{
    for (const Action& a : m_actions) {
        if (a.name == name) {
            return a;
        }
    }

    static Action null;
    return null;
}
