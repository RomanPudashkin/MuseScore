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

#ifndef MU_INSPECTOR_IINSPECTORSCONFIGURATION_H
#define MU_INSPECTOR_IINSPECTORSCONFIGURATION_H

#include "modularity/imoduleexport.h"

namespace mu {
namespace scene {
namespace inspector {
class IInspectorConfiguration : MODULE_EXPORT_INTERFACE
{
    INTERFACE_ID(IInspectorConfiguration)

public:
    enum class ThemeType {
        Dark = 0,
        Light
    };

    virtual ~IInspectorConfiguration() = default;

    virtual ThemeType themeType() const = 0;
    virtual bool antialiasedDrawing() const = 0;
};
}
}
}

#endif // MU_INSPECTOR_IINSPECTORSCONFIGURATION_H
