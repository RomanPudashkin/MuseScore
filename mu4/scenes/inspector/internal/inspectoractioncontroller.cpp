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

#include "inspectoractioncontroller.h"

using namespace mu::scene::inspector;
using namespace mu::actions;

void InspectorActionController::init()
{
    auto regCheckableAction = [this](const char *actionName) {
        dispatcher()->reg(this, actionName, this, &InspectorActionController::setChecked);
    };

    auto regTriggerableAction = [this](const char* actionName) {
        dispatcher()->reg(this, actionName, this, &InspectorActionController::triggerAction);
    };

    regCheckableAction("hraster");
    regCheckableAction("vraster");
    regCheckableAction("show-invisible");
    regCheckableAction("show-unprintable");
    regCheckableAction("show-frames");
    regCheckableAction("show-pageborders");
    regCheckableAction("show-keys");

    regTriggerableAction("config-raster");
    regTriggerableAction("show-articulation-properties");
    regTriggerableAction("show-time-signature-properties");
    regTriggerableAction("page-settings");
    regTriggerableAction("edit-style");
    regTriggerableAction("show-staff-text-properties");
}

void InspectorActionController::triggerAction(const actions::ActionName& actionName)
{
    if (interaction()) {
        interaction()->triggerAction(actionName);
    }
}

void InspectorActionController::setChecked(const actions::ActionName& actionName, const actions::ActionData &args)
{
    if (interaction()) {
        interaction()->setChecked(actionName, args.arg<bool>(0));
    }
}

bool InspectorActionController::canReceiveAction(const ActionName&) const
{
    return true;
}
