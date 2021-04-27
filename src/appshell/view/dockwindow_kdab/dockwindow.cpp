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

#include "dockwindow.h"

#include "dockpage.h"
#include "docktoolbar.h"

#include "log.h"

#include "thirdparty/KDDockWidgets/src/MainWindowBase.h"
#include "thirdparty/KDDockWidgets/src/DockWidgetQuick.h"

namespace mu::dock {
class MainWindow : public KDDockWidgets::MainWindowBase
{
public:
    MainWindow(QQuickItem* parent)
        : KDDockWidgets::MainWindowBase("mainWindow", KDDockWidgets::MainWindowOption_None, parent)
    {
        setParentItem(parent);
    }

private:
    KDDockWidgets::SideBar* sideBar(KDDockWidgets::SideBarLocation) const override
    {
        return nullptr;
    }

    QMargins centerWidgetMargins() const override
    {
        return {};
    }
};
}

using namespace mu::dock;

DockWindow::DockWindow(QQuickItem* parent)
    : QQuickItem(parent),
      m_mainWindow(new MainWindow(this)),
      m_toolBars(this),
      m_pages(this)
{
}

QString DockWindow::currentPageUri() const
{
    return m_currentPageUri;
}

QQmlListProperty<mu::dock::DockToolBar> DockWindow::toolBarsProperty()
{
    return m_toolBars.property();
}

QQmlListProperty<mu::dock::DockPage> DockWindow::pagesProperty()
{
    return m_pages.property();
}

void DockWindow::loadPage(const QString& uri)
{
    TRACEFUNC;

    if (m_currentPageUri == uri) {
        return;
    }

    DockPage* newPage = pageByUri(uri);
    IF_ASSERT_FAILED(newPage) {
        return;
    }

    DockPage* currentPage = pageByUri(m_currentPageUri);
    if (currentPage) {
        currentPage->close();
    }

    loadPageContent(newPage);

    m_currentPageUri = uri;
    emit currentPageUriChanged(uri);
}

void DockWindow::loadPageContent(DockPage* page)
{
    TRACEFUNC;

    addDock(page->centralDock(), KDDockWidgets::Location_OnRight);

    for (DockPanel* panel : page->panels()) {
        addDock(panel, KDDockWidgets::Location_OnLeft);
    }

    DockToolBar* prevToolBar = nullptr;
    for (DockToolBar* toolBar : page->toolBars()) {
        auto location = prevToolBar ? KDDockWidgets::Location_OnRight : KDDockWidgets::Location_OnTop;
        addDock(toolBar, location, prevToolBar);
        prevToolBar = toolBar;
    }

    DockStatusBar* prevStatusBar = nullptr;
    for (DockStatusBar* statusBar : page->statusBars()) {
        auto location = prevStatusBar ? KDDockWidgets::Location_OnRight : KDDockWidgets::Location_OnBottom;
        addDock(statusBar, location, prevStatusBar);
        prevStatusBar = statusBar;
    }

    QList<DockToolBar*> allToolBars = m_toolBars.list();
    allToolBars << page->mainToolBars();

    prevToolBar = nullptr;

    for (DockToolBar* toolBar : allToolBars) {
        auto location = prevToolBar ? KDDockWidgets::Location_OnRight : KDDockWidgets::Location_OnTop;
        addDock(toolBar, location, prevToolBar);
        prevToolBar = toolBar;
    }
}

void DockWindow::addDock(DockBase* dock, KDDockWidgets::Location location, DockBase* relativeTo)
{
    IF_ASSERT_FAILED(dock) {
        return;
    }

    KDDockWidgets::DockWidgetBase* relativeDock = relativeTo ? relativeTo->dockWidget() : nullptr;
    m_mainWindow->addDockWidget(dock->dockWidget(), location, relativeDock, dock->preferredSize());
    dock->init();
}

DockPage* DockWindow::pageByUri(const QString& uri) const
{
    for (DockPage* page : m_pages.list()) {
        if (page->uri() == uri) {
            return page;
        }
    }

    return nullptr;
}
