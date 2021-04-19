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

#include "docksetup.h"

#include "internal/dropindicators.h"
#include "internal/toolbargripmodel.h"

#include "dockpanel.h"
#include "dockstatusbar.h"
#include "docktoolbar.h"
#include "dockcentral.h"
#include "dockpage.h"

#include "thirdparty/KDDockWidgets/src/Config.h"
#include "thirdparty/KDDockWidgets/src/DockWidgetBase.h"
#include "thirdparty/KDDockWidgets/src/FrameworkWidgetFactory.h"

#include <QQmlEngine>

namespace mu::dock {
class DockWidgetFactory : public KDDockWidgets::DefaultWidgetFactory
{
public:
    KDDockWidgets::DropIndicatorOverlayInterface* createDropIndicatorOverlay(KDDockWidgets::DropArea* dropArea) const override
    {
        return new DropIndicators(dropArea);
    }

    QUrl titleBarFilename() const override
    {
        return QUrl("qrc:/qml/kdab/docksystem/DockTitleBar.qml");
    }
};
}

using namespace mu::dock;

void DockSetup::setup(QQmlEngine* engine)
{
    qmlRegisterType<DockPanel>("MuseScore.Dock", 1, 0, "DockPanel");
    qmlRegisterType<DockStatusBar>("MuseScore.Dock", 1, 0, "DockStatusBar");
    qmlRegisterType<DockToolBar>("MuseScore.Dock", 1, 0, "DockToolBar");
    qmlRegisterType<DockCentral>("MuseScore.Dock", 1, 0, "DockCentral");
    qmlRegisterType<DockPage>("MuseScore.Dock", 1, 0, "DockPage");

    qmlRegisterType<ToolBarGripModel>("MuseScore.Dock", 1, 0, "ToolBarGripModel");

    qmlRegisterUncreatableType<DockType>("MuseScore.Dock", 1, 0, "DockType", "Cannot create a DockType");

    qRegisterMetaType<DropIndicators*>();

    KDDockWidgets::Config::self().setFrameworkWidgetFactory(new DockWidgetFactory());
    KDDockWidgets::Config::self().setQmlEngine(engine);

    auto flags = KDDockWidgets::Config::self().flags()
            | KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;

    KDDockWidgets::Config::self().setFlags(flags);

    QSize minDockSize = KDDockWidgets::Config::self().absoluteWidgetMinSize();
    minDockSize.setHeight(30);
    KDDockWidgets::Config::self().setAbsoluteWidgetMinSize(minDockSize);
}
