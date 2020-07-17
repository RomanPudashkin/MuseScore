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

#ifndef MU_INSPECTOR_INSPECTORSACTIONCONTROLLER_H
#define MU_INSPECTOR_INSPECTORSACTIONCONTROLLER_H

#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "modularity/ioc.h"

#include "iinspectorinteraction.h"

namespace mu {
namespace scene {
namespace inspector {
class InspectorActionController : public actions::Actionable
{
    INJECT(inspector, actions::IActionsDispatcher, dispatcher)
    INJECT(inspector, IInspectorInteraction, interaction)

public:
    void init();

private:
    void triggerAction(const actions::ActionName &actionName);
    void setChecked(const actions::ActionName &actionName, const actions::ActionData &args);

    bool canReceiveAction(const actions::ActionName& actionName) const override;
};
}
}
}

#endif // MU_INSPECTOR_INSPECTORSACTIONCONTROLLER_H
