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
#ifndef MU_DOCK_TOOLBARGRIPMODEL_H
#define MU_DOCK_TOOLBARGRIPMODEL_H

#include <QQuickView>

#include "thirdparty/KDDockWidgets/src/private/TitleBar_p.h"
#include "thirdparty/KDDockWidgets/src/private/Draggable_p.h"

namespace mu::dock
{
class ToolBarGripModel : public KDDockWidgets::QWidgetAdapter,
                         public KDDockWidgets::Draggable
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem* gripMouseArea READ gripMouseArea WRITE setGripMouseArea NOTIFY gripMouseAreaChanged)

public:
    explicit ToolBarGripModel(QQuickItem *parent = nullptr);

    std::unique_ptr<KDDockWidgets::WindowBeingDragged> makeWindow() override;
    KDDockWidgets::DockWidgetBase *singleDockWidget() const override;
    bool isMDI() const override;
    bool isWindow() const override;

    QPoint mapToItem(const QPoint &pos) const override;

    Q_INVOKABLE void setToolBarWidget(KDDockWidgets::DockWidgetBase* toolbar);

    QQuickItem* gripMouseArea() const;

public slots:
    void setGripMouseArea(QQuickItem* gripMouseArea);

signals:
    void gripMouseAreaChanged(QQuickItem* gripMouseArea);

private:
    KDDockWidgets::DockWidgetBase* m_toolbarWidget = nullptr;
    QQuickItem* m_gripMouseArea = nullptr;
};
}

#endif // MU_DOCK_TOOLBARGRIPMODEL_H
