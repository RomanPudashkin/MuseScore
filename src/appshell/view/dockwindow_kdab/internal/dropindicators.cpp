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

#include "dropindicators.h"
#include "indicatorswindow.h"

#include "thirdparty/KDDockWidgets/src/Config.h"
#include "thirdparty/KDDockWidgets/src/private/DragController_p.h"
#include "thirdparty/KDDockWidgets/src/private/Utils_p.h"
#include "thirdparty/KDDockWidgets/src/FrameworkWidgetFactory.h"

#include "log.h"

namespace mu::dock {
KDDockWidgets::Location locationToMultisplitterLocation(DropIndicators::DropLocation location)
{
    switch (location) {
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Left:
        return KDDockWidgets::Location_OnLeft;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Top:
        return KDDockWidgets::Location_OnTop;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Right:
        return KDDockWidgets::Location_OnRight;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_Bottom:
        return KDDockWidgets::Location_OnBottom;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterLeft:
        return KDDockWidgets::Location_OnLeft;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterTop:
        return KDDockWidgets::Location_OnTop;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterRight:
        return KDDockWidgets::Location_OnRight;
    case KDDockWidgets::DropIndicatorOverlayInterface::DropLocation_OutterBottom:
        return KDDockWidgets::Location_OnBottom;
    default:
        return KDDockWidgets::Location_None;
    }
}

static IndicatorsWindow* createIndicatorWindow(DropIndicators* dropIndicators)
{
    auto window = new IndicatorsWindow(dropIndicators);
    window->setObjectName(QStringLiteral("_docks_IndicatorWindow_Overlay"));
    return window;
}
}

using namespace mu::dock;

DropIndicators::DropIndicators(KDDockWidgets::DropArea* dropArea)
    : KDDockWidgets::DropIndicatorOverlayInterface(dropArea),
      m_rubberBand(KDDockWidgets::Config::self().frameworkWidgetFactory()->createRubberBand(dropArea)),
      m_indicatorsWindow(createIndicatorWindow(this))
{
}

DropIndicators::~DropIndicators()
{
    delete m_indicatorsWindow;
}

KDDockWidgets::DropIndicatorOverlayInterface::DropLocation DropIndicators::hover_impl(QPoint globalPos)
{
    return m_indicatorsWindow->hover(globalPos);
}

QPoint DropIndicators::posForIndicator(DropLocation loc) const
{
    return m_indicatorsWindow->posForIndicator(loc);
}

bool DropIndicators::outterLeftIndicatorVisible() const
{
    return isAreaAllowed(Qt::LeftDockWidgetArea);
}

bool DropIndicators::outterRightIndicatorVisible() const
{
    return isAreaAllowed(Qt::RightDockWidgetArea);
}

bool DropIndicators::outterTopIndicatorVisible() const
{
    return isAreaAllowed(Qt::TopDockWidgetArea);
}

bool DropIndicators::outterBottomIndicatorVisible() const
{
    return isAreaAllowed(Qt::BottomDockWidgetArea);
}

bool DropIndicators::centralIndicatorVisible() const
{
    if (isToolBar()) {
        return false;
    }

    return !hoveringOverCetralDock();
}

bool DropIndicators::innerLeftIndicatorVisible() const
{
    return isInnerLeftIndicatorVisible(Qt::LeftDockWidgetArea);
}

bool DropIndicators::innerRightIndicatorVisible() const
{
    return isInnerLeftIndicatorVisible(Qt::RightDockWidgetArea);
}

bool DropIndicators::innerTopIndicatorVisible() const
{
    return isInnerLeftIndicatorVisible(Qt::TopDockWidgetArea);
}

bool DropIndicators::innerBottomIndicatorVisible() const
{
    return isInnerLeftIndicatorVisible(Qt::BottomDockWidgetArea);
}

bool DropIndicators::hoveringOverCetralDock() const
{
    if (!m_hoveredFrame) {
        return false;
    }

    for (auto dock : m_hoveredFrame->dockWidgets()) {
        if (dockWidgetType(dock) == DockType::Types::Central) {
            return true;
        }
    }

    return false;
}

bool DropIndicators::hoveringOverToolBar() const
{
    if (!m_hoveredFrame) {
        return false;
    }

    for (auto dock : m_hoveredFrame->dockWidgets()) {
        if (dockWidgetType(dock) == DockType::Types::ToolBar) {
            return true;
        }
    }

    return false;
}

bool DropIndicators::isAreaAllowed(Qt::DockWidgetArea area) const
{
    return m_draggedDockAllowedAreas.testFlag(area);
}

bool DropIndicators::isInnerLeftIndicatorVisible(Qt::DockWidgetArea area) const
{
    if (isToolBar()) {
        if (hoveringOverToolBar()) {
            return true;
        } else {
            return isAreaAllowed(area);
        }
    }

    if (hoveringOverCetralDock()) {
        return isAreaAllowed(area);
    }

    return true;
}

bool DropIndicators::isToolBar() const
{
    return m_draggedDockType == DockType::Types::ToolBar;
}

QObject *DropIndicators::dockWidgetProperties(const KDDockWidgets::DockWidgetBase *widget) const
{
    if (!widget) {
        return nullptr;
    }

    return widget->findChild<QObject*>("properties");
}

DockType::Types DropIndicators::dockWidgetType(const KDDockWidgets::DockWidgetBase *widget) const
{
    QObject* properties = dockWidgetProperties(widget);
    if (!properties) {
        return DockType::Types::Undefined;
    }

    return static_cast<DockType::Types>(properties->property("dockType").toInt());
}

Qt::DockWidgetAreas DropIndicators::dockWidgetAllowedAreas(const KDDockWidgets::DockWidgetBase *widget) const
{
    QObject* properties = dockWidgetProperties(widget);
    if (!properties) {
        return Qt::AllDockWidgetAreas;
    }

    return static_cast<Qt::DockWidgetAreas>(properties->property("allowedAreas").toInt());
}

bool DropIndicators::onResize(QSize)
{
     m_indicatorsWindow->resize(window()->size());
     return false;
}

void DropIndicators::updateVisibility()
{
    if (isHovered()) {
        m_indicatorsWindow->setVisible(true);
        updateWindowPosition();
        m_indicatorsWindow->raise();
    } else {
        m_rubberBand->setVisible(false);
        m_indicatorsWindow->setVisible(false);
    }

    m_draggedDockAllowedAreas = Qt::AllDockWidgetAreas;

    auto windowBeingDragged = KDDockWidgets::DragController::instance()->windowBeingDragged();
    if (!windowBeingDragged) {
        return;
    }

    auto dock = windowBeingDragged->dockWidgets().first();
    m_draggedDockType = dockWidgetType(dock);
    m_draggedDockAllowedAreas = dockWidgetAllowedAreas(dock);

    emit indicatorsVisibilityChanged();
}

void DropIndicators::setDropLocation(DropLocation location)
{
    setCurrentDropLocation(location);

    if (location == DropLocation_None) {
        m_rubberBand->setVisible(false);
        return;
    }

    if (location == DropLocation_Center) {
        m_rubberBand->setGeometry(m_hoveredFrame ? m_hoveredFrame->QWidgetAdapter::geometry() : rect());
        m_rubberBand->setVisible(true);
        return;
    }

    KDDockWidgets::Location multisplitterLocation = locationToMultisplitterLocation(location);
    KDDockWidgets::Frame *relativeToFrame = nullptr;

    switch (location) {
    case DropLocation_Left:
    case DropLocation_Top:
    case DropLocation_Right:
    case DropLocation_Bottom:
        if (!m_hoveredFrame) {
            qWarning() << "DropIndicators::setCurrentDropLocation: frame is null. location=" << location
                       << "; isHovered=" << isHovered()
                       << "; dropArea->widgets=" << m_dropArea->items();
            Q_ASSERT(false);
            return;
        }
        relativeToFrame = m_hoveredFrame;
        break;
    case DropLocation_OutterLeft:
    case DropLocation_OutterTop:
    case DropLocation_OutterRight:
    case DropLocation_OutterBottom:
        break;
    default:
        break;
    }

    auto windowBeingDragged = KDDockWidgets::DragController::instance()->windowBeingDragged();

    QRect rect = m_dropArea->rectForDrop(windowBeingDragged, multisplitterLocation,
                                         m_dropArea->itemForFrame(relativeToFrame));
    m_rubberBand->setGeometry(rect);
    m_rubberBand->setVisible(true);
}

void DropIndicators::updateWindowPosition()
{
    QRect rect = this->rect();
    if (KDDockWidgets::isWindow(m_indicatorsWindow)) {
        // On all non-wayland platforms it's a top-level.
        QPoint pos = mapToGlobal(QPoint(0, 0));
        rect.moveTo(pos);
    }
    m_indicatorsWindow->setGeometry(rect);
}
