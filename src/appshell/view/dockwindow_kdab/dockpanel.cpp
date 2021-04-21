//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2021 MuseScore BVBA and others
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

#include "dockpanel.h"

#include "log.h"

using namespace mu::dock;
using namespace mu::uicomponents;

DockPanel::DockPanel(QQuickItem* parent)
    : DockBase(parent), m_tabs(this)
{
}

void DockPanel::init()
{
    TRACEFUNC;

    DockBase::init();

    for (const DockPanel* panel : m_tabs.list()) {
        addDockWidgetAsTab(panel->dockWidget());
    }
}

QQmlListProperty<DockPanel> DockPanel::tabsProperty()
{
    return m_tabs.property();
}

DockType DockPanel::type() const
{
    return DockType::Panel;
}
