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
};
}

using namespace mu::dock;

bool tabbingAllowed(const QVector<KDDockWidgets::DockWidgetBase*>& source,
                    const QVector<KDDockWidgets::DockWidgetBase*>& target)
{
    UNUSED(source);

    for (const KDDockWidgets::DockWidgetBase* dock: target) {
        if (dock->uniqueName().contains("central")) {
            return false;
        }
    }

    return true;
}

void DockSetup::setup(QQmlEngine* engine)
{
    qRegisterMetaType<DropIndicators*>();

    KDDockWidgets::Config::self().setFrameworkWidgetFactory(new DockWidgetFactory());
    KDDockWidgets::Config::self().setQmlEngine(engine);

    auto flags = KDDockWidgets::Config::self().flags()
            | KDDockWidgets::Config::Flag_HideTitleBarWhenTabsVisible;

    KDDockWidgets::Config::self().setFlags(flags);
    KDDockWidgets::Config::self().setTabbingAllowedFunc(tabbingAllowed);
}
