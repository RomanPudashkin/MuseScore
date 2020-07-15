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

#ifndef MU_INSPECTORS_INSPECTORSACTIONCONTROLLER_H
#define MU_INSPECTORS_INSPECTORSACTIONCONTROLLER_H

#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "modularity/ioc.h"

namespace mu {
namespace inspectors {
class InspectorsActionController : public actions::Actionable
{
    INJECT(inspectors, actions::IActionsDispatcher, dispatcher)

public:
    void init();

private:
    bool canReceiveAction(const actions::ActionName& action) const override;
};
}
}

#endif // MU_INSPECTORS_INSPECTORSACTIONCONTROLLER_H
