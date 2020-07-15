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
           QT_TRANSLATE_NOOP("action", ""),
           ShortcutContext::Any
           ),
    Action("vraster",
           QT_TRANSLATE_NOOP("action", ""),
           ShortcutContext::Any
           ),
    Action("config-raster",
           QT_TRANSLATE_NOOP("action", ""),
           ShortcutContext::Any
           ),
    Action("show-articulation-properties",
           QT_TRANSLATE_NOOP("action", ""),
           ShortcutContext::Any
           ),
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
