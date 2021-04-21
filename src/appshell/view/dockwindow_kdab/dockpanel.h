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
#ifndef MU_DOCK_DOCKPANEL_H
#define MU_DOCK_DOCKPANEL_H

#include "internal/dockbase.h"

#include "framework/uicomponents/view/qmllistproperty.h"

namespace mu::dock {
class DockPanel : public DockBase
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<mu::dock::DockPanel> tabs READ tabsProperty)

public:
    explicit DockPanel(QQuickItem* parent = nullptr);

    void init() override;

    QQmlListProperty<mu::dock::DockPanel> tabsProperty();

private:
    DockType type() const override;

    uicomponents::QmlListProperty<DockPanel> m_tabs;
};
}

#endif // MU_DOCK_DOCKPANEL_H
