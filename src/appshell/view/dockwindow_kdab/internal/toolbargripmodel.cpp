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

#include "toolbargripmodel.h"

#include <QPointF>

#include "thirdparty/KDDockWidgets/src/private/DragController_p.h"
#include "thirdparty/KDDockWidgets/src/private/Utils_p.h"
#include "thirdparty/KDDockWidgets/src/Config.h"

using namespace mu::dock;
using namespace KDDockWidgets;

ToolBarGripModel::ToolBarGripModel(QQuickItem *parent)
    : KDDockWidgets::QWidgetAdapter(parent),
      KDDockWidgets::Draggable(this)
{
}

std::unique_ptr<KDDockWidgets::WindowBeingDragged> ToolBarGripModel::makeWindow()
{
    if (!m_toolbarWidget) {
        return {};
    }

    FloatingWindow* floatingWindow = m_toolbarWidget->floatingWindow();
    if (floatingWindow) {
        return std::unique_ptr<WindowBeingDragged>(new WindowBeingDragged(floatingWindow, this));
    }

    m_toolbarWidget->setFloating(true);
    floatingWindow = m_toolbarWidget->floatingWindow();

    auto draggable = KDDockWidgets::usesNativeTitleBar() ? static_cast<Draggable*>(floatingWindow)
                                                         : static_cast<Draggable*>(this);
    return std::unique_ptr<WindowBeingDragged>(new WindowBeingDragged(floatingWindow, draggable));
}

KDDockWidgets::DockWidgetBase *ToolBarGripModel::singleDockWidget() const
{
    return m_toolbarWidget;
}

bool ToolBarGripModel::isMDI() const
{
    return false;
}

bool ToolBarGripModel::isWindow() const
{
    return false;
}

QPoint ToolBarGripModel::mapToItem(const QPoint &pos) const
{
    if (!m_gripMouseArea) {
        return pos;
    }

    QPointF result = m_gripMouseArea->mapToItem(m_toolbarWidget, QPointF(pos));
    result.setY(result.y() + m_toolbarWidget->titleBar()->height());
    return QPoint(result.x(), result.y());
}

void ToolBarGripModel::setToolBarWidget(KDDockWidgets::DockWidgetBase *toolbar)
{
    m_toolbarWidget = toolbar;
}

QQuickItem *ToolBarGripModel::gripMouseArea() const
{
    return m_gripMouseArea;
}

void ToolBarGripModel::setGripMouseArea(QQuickItem *gripMouseArea)
{
    if (m_gripMouseArea == gripMouseArea)
        return;

    m_gripMouseArea = gripMouseArea;

    redirectMouseEvents(gripMouseArea);

    emit gripMouseAreaChanged(m_gripMouseArea);
}
