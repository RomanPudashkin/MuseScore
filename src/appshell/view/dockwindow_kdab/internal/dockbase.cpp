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

#include "dockbase.h"

#include "log.h"

using namespace mu::dock;

static const char* ALLOWED_AREAS_KEY("allowedAreas");
static const char* DOCK_TYPE_KEY("dockType");

DockBase::DockBase(QQuickItem *parent)
    : KDDockWidgets::DockWidgetInstantiator(parent)
{
    m_properties = new QObject();
    m_properties->setObjectName("properties");

    connect(this, &DockBase::minimumSizeChanged, this, &DockBase::resize);
    connect(this, &DockBase::maximumSizeChanged, this, &DockBase::resize);
}

int DockBase::minimumWidth() const
{
    return m_minimumWidth;
}

int DockBase::minimumHeight() const
{
    return m_minimumHeight;
}

int DockBase::maximumWidth() const
{
    return m_maximumWidth;
}

int DockBase::maximumHeight() const
{
    return m_maximumHeight;
}

QSize DockBase::preferredSize() const
{
    return QSize(width(), height());
}

Qt::DockWidgetAreas DockBase::allowedAreas() const
{
    return static_cast<Qt::DockWidgetAreas>(m_properties->property(ALLOWED_AREAS_KEY).toInt());
}

void DockBase::setMinimumWidth(int width)
{
    if (width == minimumWidth()) {
        return;
    }

    m_minimumWidth = width;
    emit minimumSizeChanged();
}

void DockBase::setMinimumHeight(int height)
{
    if (height == minimumHeight()) {
        return;
    }

    m_minimumHeight = height;
    emit minimumSizeChanged();
}

void DockBase::setMaximumWidth(int width)
{
    if (width == maximumWidth()) {
        return;
    }

    m_maximumWidth = width;
    emit maximumSizeChanged();
}

void DockBase::setMaximumHeight(int height)
{
    if (height == maximumHeight()) {
        return;
    }

    m_maximumHeight = height;
    emit maximumSizeChanged();
}

void DockBase::setAllowedAreas(Qt::DockWidgetAreas areas)
{
    if (areas == allowedAreas()) {
        return;
    }

    m_properties->setProperty(ALLOWED_AREAS_KEY, static_cast<int>(areas));
    emit allowedAreasChanged();
}

void DockBase::resize()
{
    applyMinimumSize();
    applyMaximumSize();

    if (dockWidget()) {
        dockWidget()->frame()->layoutItem()->parentBoxContainer()->layoutEqually();
    }
}

void DockBase::init()
{
    applyMinimumSize();
    applyMaximumSize();

    m_properties->setProperty(DOCK_TYPE_KEY, static_cast<int>(type()));
}

void DockBase::componentComplete()
{
    KDDockWidgets::DockWidgetInstantiator::componentComplete();

    IF_ASSERT_FAILED(dockWidget()) {
        return;
    }

    m_properties->setParent(dockWidget());
}

void DockBase::applyMinimumSize()
{
    auto dock = dockWidget();
    if (!dock) {
        return;
    }

    int minWidth = m_minimumWidth > 0 ? m_minimumWidth : dock->minimumSize().width();
    int minHeight = m_minimumHeight > 0 ? m_minimumHeight : dock->minimumSize().height();

    QSize minimumSize = QSize(minWidth, minHeight);
    if (minimumSize == dock->minimumSize()) {
        return;
    }
    
    dock->setMinimumSize(minimumSize);
    
    if (auto frame = dock->frame()) {
        frame->setMinimumSize(minimumSize);
    }
}

void DockBase::applyMaximumSize()
{
    auto dock = dockWidget();
    if (!dock) {
        return;
    }

    int maxWidth = m_maximumWidth > 0 ? m_maximumWidth : dock->maximumSize().width();
    int maxHeight = m_maximumHeight > 0 ? m_maximumHeight : dock->maximumSize().height();

    QSize maximumSize = QSize(maxWidth, maxHeight);
    if (maximumSize == dock->maximumSize()) {
        return;
    }
    
    dock->setMaximumSize(maximumSize);
    
    if (auto frame = dock->frame()) {
        frame->setMaximumSize(maximumSize);
    }
}
