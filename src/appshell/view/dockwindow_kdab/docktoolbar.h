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
#ifndef MU_DOCK_DOCKTOOLBAR_H
#define MU_DOCK_DOCKTOOLBAR_H

#include "internal/dockbase.h"

namespace mu::dock {
class DockToolBar : public DockBase
{
    Q_OBJECT

    Q_PROPERTY(bool movable READ movable WRITE setMovable NOTIFY movableChanged)
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    Q_PROPERTY(QStringList allowedPagesUriList READ allowedPagesUriList WRITE setAllowedPagesUriList NOTIFY allowedPagesUriListChanged)

public:
    explicit DockToolBar(QQuickItem* parent = nullptr);

    bool movable() const;
    Qt::Orientation orientation() const;

    QStringList allowedPagesUriList() const;

public slots:
    void setMinimumWidth(int width) override;
    void setMinimumHeight(int height) override;
    void setMaximumWidth(int width) override;
    void setMaximumHeight(int height) override;

    void setMovable(bool movable);
    void setOrientation(Qt::Orientation orientation);

    void updateOrientation();

    void setAllowedPagesUriList(QStringList allowedPagesUriList);

signals:
    void movableChanged(bool movable);
    void orientationChanged(Qt::Orientation orientation);

    void allowedPagesUriListChanged(QStringList allowedPagesUriList);

private:
    void componentComplete() override;

    DockType type() const override;
    DockType dockWidgetType(const KDDockWidgets::DockWidgetBase* widget) const;
    QObject* dockWidgetProperties(const KDDockWidgets::DockWidgetBase* widget) const;

    bool m_movable = true;
    Qt::Orientation m_orientation = Qt::Horizontal;
    QStringList m_allowedPagesUriList;
};
}

#endif // MU_DOCK_DOCKTOOLBAR_H
