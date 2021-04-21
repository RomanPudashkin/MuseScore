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

#include <QTimer>

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

void DockToolBar::updateOrientation()
{
    if (dockWidget()->isFloating()) {
        return;
    }

    Layouting::ItemBoxContainer* container = dockWidget()->frame()->layoutItem()->parentBoxContainer();
    if (!container) {
        return;
    }

    if (container->isVertical()) {
        setOrientation(Qt::Horizontal);
        return;
    }

    Qt::Orientation newOrientation = Qt::Horizontal;

    for (const Layouting::Item* containerItem: container->childItems()) {
        auto frame = static_cast<KDDockWidgets::Frame*>(containerItem->guestAsQObject());
        if (!frame || frame->dockWidgets().empty()) {
            continue;
        }

        if (frame && dockWidgetType(frame->dockWidgets().first()) == DockType::Central) {
            newOrientation = Qt::Vertical;
            break;
        }
    }

    setOrientation(newOrientation);
}

void DockToolBar::componentComplete()
{
    DockBase::componentComplete();

    connect(dockWidget(), &KDDockWidgets::DockWidgetQuick::parentChanged, [this]() {
        if (!dockWidget()) {
            return;
        }

        KDDockWidgets::Frame* frame = dockWidget()->frame();
        if (!frame) {
            return;
        }

        connect(frame, &KDDockWidgets::Frame::isInMainWindowChanged, this, [this]() {
            QTimer::singleShot(0, this, &DockToolBar::updateOrientation);
        }, Qt::UniqueConnection);
    });
}

DockType DockToolBar::type() const
{
    return DockType::ToolBar;
}

DockType DockToolBar::dockWidgetType(const KDDockWidgets::DockWidgetBase *widget) const
{
    QObject* properties = dockWidgetProperties(widget);

    if (!properties) {
        return DockType::Undefined;
    }

    return static_cast<DockType>(properties->property("dockType").toInt());
}

QObject *DockToolBar::dockWidgetProperties(const KDDockWidgets::DockWidgetBase *widget) const
{
    if (!widget) {
        return nullptr;
    }

    return widget->findChild<QObject*>("properties");
}
