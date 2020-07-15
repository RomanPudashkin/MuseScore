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

#include "inspectorsactioncontroller.h"

using namespace mu::inspectors;
using namespace mu::actions;

void InspectorsActionController::init()
{
    dispatcher()->reg(this, "hraster", [this](const ActionName&, const ActionData&) {});
    dispatcher()->reg(this, "vraster", [this](const ActionName&, const ActionData&) {});
    dispatcher()->reg(this, "config-raster", [this]() {});
    dispatcher()->reg(this, "show-articulation-properties", [this]() {});
}

bool InspectorsActionController::canReceiveAction(const ActionName& action) const
{
    Q_UNUSED(action)

    return true;
}
