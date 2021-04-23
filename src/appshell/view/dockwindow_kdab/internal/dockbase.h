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
#ifndef MU_DOCK_DOCKBASE_H
#define MU_DOCK_DOCKBASE_H

#include "../docktypes.h"

#include "thirdparty/KDDockWidgets/src/private/quick/DockWidgetInstantiator_p.h"

namespace mu::dock {
class DockBase : public KDDockWidgets::DockWidgetInstantiator
{
    Q_OBJECT

    Q_PROPERTY(int minimumWidth READ minimumWidth WRITE setMinimumWidth NOTIFY minimumSizeChanged)
    Q_PROPERTY(int minimumHeight READ minimumHeight WRITE setMinimumHeight NOTIFY minimumSizeChanged)
    Q_PROPERTY(int maximumWidth READ maximumWidth WRITE setMaximumWidth NOTIFY maximumSizeChanged)
    Q_PROPERTY(int maximumHeight READ maximumHeight WRITE setMaximumHeight NOTIFY maximumSizeChanged)

    Q_PROPERTY(Qt::DockWidgetAreas allowedAreas READ allowedAreas WRITE setAllowedAreas NOTIFY allowedAreasChanged)

public:
    explicit DockBase(QQuickItem* parent = nullptr);

    int minimumWidth() const;
    int minimumHeight() const;
    int maximumWidth() const;
    int maximumHeight() const;

    QSize preferredSize() const;

    Qt::DockWidgetAreas allowedAreas() const;

    virtual void init();

public slots:
    virtual void setMinimumWidth(int width);
    virtual void setMinimumHeight(int height);
    virtual void setMaximumWidth(int width);
    virtual void setMaximumHeight(int height);

    void setAllowedAreas(Qt::DockWidgetAreas areas);

signals:
    void minimumSizeChanged();
    void maximumSizeChanged();
    void allowedAreasChanged();

    void closed();

protected:
    virtual DockType type() const = 0;

    void componentComplete() override;

private slots:
    void resize();

private:
    void applySizeConstraints();

    int m_minimumWidth = 0;
    int m_minimumHeight = 0;
    int m_maximumWidth = 0;
    int m_maximumHeight = 0;

    Qt::DockWidgetAreas m_allowedAreas = Qt::NoDockWidgetArea;
};
}

#endif // MU_DOCK_DOCKBASE_H
