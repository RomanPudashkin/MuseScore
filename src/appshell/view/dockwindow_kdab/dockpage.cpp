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

#include "dockwindow.h"
#include "docktoolbar.h"
#include "dockcentral.h"
#include "dockpanel.h"
#include "dockstatusbar.h"

#include "log.h"

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

void DockPage::init(DockWindow& window)
{
    TRACEFUNC;

    window.addDock(m_central, KDDockWidgets::Location_OnRight);

    for (DockPanel* panel : m_panels.list()) {
        window.addDock(panel, KDDockWidgets::Location_OnLeft);
    }

    DockToolBar* prevToolBar = nullptr;
    for (DockToolBar* toolBar : m_toolBars.list()) {
        auto location = prevToolBar ? KDDockWidgets::Location_OnRight : KDDockWidgets::Location_OnTop;
        window.addDock(toolBar, location, prevToolBar);
        prevToolBar = toolBar;
    }

    DockStatusBar* prevStatusBar = nullptr;
    for (DockStatusBar* statusBar : m_statusBars.list()) {
        auto location = prevStatusBar ? KDDockWidgets::Location_OnRight : KDDockWidgets::Location_OnBottom;
        window.addDock(statusBar, location, prevStatusBar);
        prevStatusBar = statusBar;
    }
}

void DockPage::close()
{
    TRACEFUNC;

    auto toolbars = m_toolBars.list();
    auto panels = m_panels.list();
    auto statusBars = m_statusBars.list();

    QList<DockBase*> allDocks;
    allDocks << QList<DockBase*>(toolbars.begin(), toolbars.end());
    allDocks << m_central;
    allDocks << QList<DockBase*>(panels.begin(), panels.end());
    allDocks << QList<DockBase*>(statusBars.begin(), statusBars.end());

    for (DockBase* dock : allDocks) {
        dock->close();
    }
}

void DockPage::componentComplete()
{
    QQuickItem::componentComplete();

    Q_ASSERT(!m_uniqueName.isEmpty());
    Q_ASSERT(!m_uri.isEmpty());
}
