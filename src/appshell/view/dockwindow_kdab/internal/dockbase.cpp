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

DockBase::DockBase(QQuickItem* parent)
    : KDDockWidgets::DockWidgetInstantiator(parent)
{
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
    return m_allowedAreas;
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

    m_allowedAreas = areas;
    emit allowedAreasChanged();
}

void DockBase::resize()
{
    applySizeConstraints();

    if (dockWidget()) {
        dockWidget()->frame()->layoutItem()->parentBoxContainer()->layoutEqually();
    }
}

void DockBase::init()
{
    applySizeConstraints();
}

void DockBase::componentComplete()
{
    KDDockWidgets::DockWidgetInstantiator::componentComplete();

    IF_ASSERT_FAILED(dockWidget()) {
        return;
    }

    DockProperties properties;
    properties.type = type();
    properties.allowedAreas = allowedAreas();

    writePropertiesTobject(properties, *dockWidget());
}

void DockBase::applySizeConstraints()
{
    auto dock = dockWidget();
    if (!dock) {
        return;
    }

    int minWidth = m_minimumWidth > 0 ? m_minimumWidth : dock->minimumWidth();
    int minHeight = m_minimumHeight > 0 ? m_minimumHeight : dock->minimumHeight();
    int maxWidth = m_maximumWidth > 0 ? m_maximumWidth : dock->maximumWidth();
    int maxHeight = m_maximumHeight > 0 ? m_maximumHeight : dock->maximumHeight();

    QSize minimumSize(minWidth, minHeight);
    QSize maximumSize(maxWidth, maxHeight);

    dock->setMinimumSize(minimumSize);
    dock->setMaximumSize(maximumSize);

    if (auto frame = dock->frame()) {
        frame->setMinimumSize(minimumSize);
        frame->setMaximumSize(maximumSize);
    }
}
