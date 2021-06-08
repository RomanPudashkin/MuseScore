/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "docktoolbar.h"

#include <QTimer>

#include "thirdparty/KDDockWidgets/src/DockWidgetQuick.h"
#include "thirdparty/KDDockWidgets/src/private/TitleBar_p.h"
#include "thirdparty/KDDockWidgets/src/private/DragController_p.h"
#include "thirdparty/KDDockWidgets/src/private/FloatingWindow_p.h"

#include "log.h"

using namespace mu::dock;

const int DockToolBar::MIN_SIDE_SIZE = 48;
const int DockToolBar::MAX_SIDE_SIZE = std::numeric_limits<int>::max();

class DockToolBar::DraggableArea : public KDDockWidgets::QWidgetAdapter, public KDDockWidgets::Draggable
{
public:
    DraggableArea()
        : KDDockWidgets::QWidgetAdapter(),
        KDDockWidgets::Draggable(this)
    {
    }

    std::unique_ptr<KDDockWidgets::WindowBeingDragged> makeWindow() override
    {
        if (!m_dockWidget) {
            return {};
        }

        KDDockWidgets::FloatingWindow* floatingWindow = m_dockWidget->floatingWindow();
        if (floatingWindow) {
            return std::unique_ptr<KDDockWidgets::WindowBeingDragged>(new KDDockWidgets::WindowBeingDragged(floatingWindow, this));
        }

        m_dockWidget->setFloating(true);
        floatingWindow = m_dockWidget->floatingWindow();

        auto draggable = static_cast<KDDockWidgets::Draggable*>(this);
        return std::unique_ptr<KDDockWidgets::WindowBeingDragged>(new KDDockWidgets::WindowBeingDragged(floatingWindow, draggable));
    }

    KDDockWidgets::DockWidgetBase* singleDockWidget() const override
    {
        return m_dockWidget;
    }

    bool isMDI() const override
    {
        return false;
    }

    bool isWindow() const override
    {
        return false;
    }

    QPoint mapToWindow(QPoint pos) const override
    {
        if (!m_mouseArea) {
            return pos;
        }

        QPointF result = m_mouseArea->mapToItem(m_dockWidget, QPointF(pos));
        result.setY(result.y() + m_dockWidget->titleBar()->height());
        return QPoint(result.x(), result.y());
    }

    void setDockWidget(KDDockWidgets::DockWidgetBase* dockWidget)
    {
        IF_ASSERT_FAILED(dockWidget) {
            return;
        }

        m_dockWidget = dockWidget;
        setObjectName(dockWidget->objectName() + "_draggableArea");
    }

    void setMouseArea(QQuickItem* mouseArea)
    {
        m_mouseArea = mouseArea;
        redirectMouseEvents(mouseArea);
    }

private:
    KDDockWidgets::DockWidgetBase* m_dockWidget = nullptr;
    QQuickItem* m_mouseArea = nullptr;
};

DockToolBar::DockToolBar(QQuickItem* parent)
    : DockBase(parent),
    //! NOTE: parent (MouseArea) will be set later
    m_draggableArea(new DraggableArea())
{
    setAllowedAreas(Qt::TopDockWidgetArea);
    setLocation(DockLocation::Top);

    setMinimumWidth(MIN_SIDE_SIZE);
    setMaximumWidth(MAX_SIDE_SIZE);
    setMinimumHeight(MIN_SIDE_SIZE);
    setMaximumHeight(MIN_SIDE_SIZE);

    setWidth(MAX_SIDE_SIZE);
    setHeight(MIN_SIDE_SIZE);

    connect(this, &DockToolBar::floatingChanged, this, &DockToolBar::applyFloatingSizeConstraints);
}

Qt::Orientation DockToolBar::orientation() const
{
    return m_orientation;
}

void DockToolBar::setDraggableMouseArea(QQuickItem* mouseArea)
{
    IF_ASSERT_FAILED(m_draggableArea) {
        return;
    }

    m_draggableArea->setParent(mouseArea);
    m_draggableArea->setMouseArea(mouseArea);
}

int DockToolBar::horizontalFloatingLength() const
{
    return m_horizontalFloatingLength;
}

int DockToolBar::verticalFloatingLength() const
{
    return m_verticalFloatingLength;
}

void DockToolBar::setOrientation(Qt::Orientation orientation)
{
    bool isChangingAllowed = isOrientationChangingAllowed();

    if (orientation == m_orientation || !isChangingAllowed) {
        return;
    }

    m_orientation = orientation;
    emit orientationChanged(orientation);

    applyFloatingSizeConstraints();
}

void DockToolBar::setHorizontalFloatingLength(int length)
{
    if (m_horizontalFloatingLength == length) {
        return;
    }

    m_horizontalFloatingLength = length;
    emit horizontalFloatingLengthChanged(length);
}

void DockToolBar::setVerticalFloatingLength(int length)
{
    if (m_verticalFloatingLength == length) {
        return;
    }

    m_verticalFloatingLength = length;
    emit verticalFloatingLengthChanged(length);
}

void DockToolBar::setMinimumWidth(int width)
{
    width = std::max(MIN_SIDE_SIZE, width);
    DockBase::setMinimumWidth(width);
}

void DockToolBar::setMinimumHeight(int height)
{
    height = std::max(MIN_SIDE_SIZE, height);
    DockBase::setMinimumHeight(height);
}

void DockToolBar::setMaximumWidth(int width)
{
    width = std::max(MIN_SIDE_SIZE, width);
    DockBase::setMaximumWidth(width);
}

void DockToolBar::setMaximumHeight(int height)
{
    height = std::max(MIN_SIDE_SIZE, height);
    DockBase::setMaximumHeight(height);
}

void DockToolBar::componentComplete()
{
    DockBase::componentComplete();

    m_draggableArea->setDockWidget(dockWidget());
}

DockType DockToolBar::type() const
{
    return DockType::ToolBar;
}

void DockToolBar::applyFloatingSizeConstraints()
{
    if (!floating()) {
        return;
    }

    KDDockWidgets::FloatingWindow* window = dockWidget()->floatingWindow();
    if (!window) {
        return;
    }

    TRACEFUNC;

    int minWidth = m_horizontalFloatingLength > 0 ? m_horizontalFloatingLength : minimumWidth();

    QSize minSize(minWidth, minimumHeight());
    QSize maxSize(minWidth, minimumHeight());

    if (orientation() == Qt::Vertical) {
        int minHeight = m_verticalFloatingLength > 0 ? m_horizontalFloatingLength : minimumHeight();

        minSize = QSize(minimumWidth(), minHeight);
        maxSize = QSize(minimumWidth(), maximumHeight());
    }

    window->setMinimumSize(minSize);
    window->setMaximumSize(maxSize);
}

bool DockToolBar::isOrientationChangingAllowed() const
{
    return allowedAreas().testFlag(Qt::LeftDockWidgetArea)
           || allowedAreas().testFlag(Qt::RightDockWidgetArea);
}
