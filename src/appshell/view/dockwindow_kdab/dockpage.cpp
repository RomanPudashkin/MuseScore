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

#include "dockpage.h"

#include "log.h"

#include "thirdparty/KDDockWidgets/src/private/quick/MainWindowInstantiator_p.h"

using namespace mu::dock;

DockPage::DockPage(QQuickItem* parent)
    : QQuickItem(parent),
      m_toolBars(this),
      m_panels(this),
      m_statusBars(this)
{
}

QString DockPage::uniqueName() const
{
    return m_uniqueName;
}

QString DockPage::uri() const
{
    return m_uri;
}

QQmlListProperty<DockToolBar> DockPage::toolBarsProperty()
{
    return m_toolBars.property();
}

QQmlListProperty<DockPanel> DockPage::panelsProperty()
{
    return m_panels.property();
}

DockCentral* DockPage::centralDock() const
{
    return m_central;
}

QQmlListProperty<DockStatusBar> DockPage::statusBarsProperty()
{
    return m_statusBars.property();
}

void DockPage::setUniqueName(const QString& name)
{
    if (name == m_uniqueName) {
        return;
    }

    m_uniqueName = name;
    emit uniqueNameChanged(name);
}

void DockPage::setUri(const QString& uri)
{
    if (uri == m_uri) {
        return;
    }

    m_uri = uri;
    emit uriChanged(uri);
}

void DockPage::setCentralDock(DockCentral* central)
{
    if (central == m_central) {
        return;
    }

    m_central = central;
    emit centralDockChanged(central);
}

void DockPage::init(QQuickItem* mainLayout)
{
    auto layout = dynamic_cast<KDDockWidgets::MainWindowInstantiator*>(mainLayout);
    IF_ASSERT_FAILED(layout) {
        return;
    }

    layout->addDockWidget(m_central, KDDockWidgets::Location_OnRight);

    for (DockPanel* panel : m_panels.list()) {
        panel->setParent(mainLayout);
        layout->addDockWidget(panel->dockWidget(), KDDockWidgets::Location_OnLeft, nullptr, QSize(panel->width(), panel->height()));
        panel->init();
    }

    DockToolBar* prevToolBar = nullptr;
    for (DockToolBar* toolBar : m_toolBars.list()) {
        toolBar->setParent(mainLayout);

        if (!prevToolBar) {
            layout->addDockWidget(toolBar->dockWidget(), KDDockWidgets::Location_OnTop, nullptr, QSize(toolBar->width(), toolBar->height()));
        } else {
            layout->addDockWidget(toolBar->dockWidget(), KDDockWidgets::Location_OnRight, prevToolBar->dockWidget(), QSize(toolBar->width(), toolBar->height()));
        }

        toolBar->init();
        prevToolBar = toolBar;
    }

    DockStatusBar* prevStatusBar = nullptr;
    for (DockStatusBar* statusBar : m_statusBars.list()) {
        statusBar->setParent(mainLayout);

        if (!prevStatusBar) {
            layout->addDockWidget(statusBar->dockWidget(), KDDockWidgets::Location_OnBottom, nullptr, QSize(statusBar->width(), statusBar->height()));
        } else {
            layout->addDockWidget(statusBar->dockWidget(), KDDockWidgets::Location_OnRight, prevStatusBar->dockWidget(), QSize(statusBar->width(), statusBar->height()));
        }

        statusBar->init();
        prevStatusBar = statusBar;
    }
}
