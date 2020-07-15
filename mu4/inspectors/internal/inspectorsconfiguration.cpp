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

#include "inspectorsconfiguration.h"

#include "settings.h"

using namespace mu::inspectors;
using namespace mu::framework;

namespace {
const std::string moduleName("inspectors");
const Settings::Key THEME_TYPE_KEY(moduleName, "ui/application/globalStyle");
const Settings::Key ANTIALIASED_DRAWING_KEY(moduleName, "ui/canvas/misc/antialiasedDrawing");
}

InspectorsConfiguration::ThemeType InspectorsConfiguration::themeType() const
{
    return static_cast<ThemeType>(settings()->value(THEME_TYPE_KEY).toInt());
}

bool InspectorsConfiguration::antialiasedDrawing() const
{
    return settings()->value(ANTIALIASED_DRAWING_KEY).toBool();
}
