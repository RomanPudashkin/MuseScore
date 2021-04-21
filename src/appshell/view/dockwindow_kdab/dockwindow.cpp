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

#include "thirdparty/KDDockWidgets/src/private/DockRegistry_p.h"

using namespace mu::dock;

DockWindow::DockWindow(QQuickItem* parent)
    : KDDockWidgets::MainWindowInstantiator(),
      m_toolBars(this),
      m_pages(this)
{
    setParentItem(parent);
    setUniqueName("mainWindow");
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

    DockPage* page = pageByUri(uri);
    IF_ASSERT_FAILED(page) {
        return;
    }

    KDDockWidgets::DockRegistry::self()->clear();

    page->init(this);

    DockToolBar* prevToolBar = nullptr;
    for (DockToolBar* toolBar : m_toolBars.list()) {
        toolBar->setParent(this);

        if (!prevToolBar) {
            addDockWidget(toolBar->dockWidget(), KDDockWidgets::Location_OnTop, nullptr, toolBar->preferredSize());
        } else {
            addDockWidget(toolBar->dockWidget(), KDDockWidgets::Location_OnRight, prevToolBar->dockWidget(), toolBar->preferredSize());
        }

        toolBar->init();
        prevToolBar = toolBar;
    }

    m_currentPageUri = uri;
    emit currentPageUriChanged(uri);
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
