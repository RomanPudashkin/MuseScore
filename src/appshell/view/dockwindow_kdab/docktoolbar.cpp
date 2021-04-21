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

#include "docktoolbar.h"

using namespace mu::dock;

DockToolBar::DockToolBar(QQuickItem* parent)
    : DockBase(parent)
{
    setAllowedAreas(Qt::TopDockWidgetArea);
}

Qt::Orientation DockToolBar::orientation() const
{
    return m_orientation;
}

void DockToolBar::setOrientation(Qt::Orientation orientation)
{
    if (orientation == m_orientation) {
        return;
    }

    m_orientation = orientation;
    emit orientationChanged(orientation);
}

DockType DockToolBar::type() const
{
    return DockType::ToolBar;
}
