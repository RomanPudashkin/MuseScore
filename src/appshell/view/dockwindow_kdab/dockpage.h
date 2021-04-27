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
#ifndef MU_DOCK_DOCKPAGE_H
#define MU_DOCK_DOCKPAGE_H

#include "framework/uicomponents/view/qmllistproperty.h"

#include <QQuickItem>

namespace mu::dock {
class DockWindow;
class DockToolBar;
class DockPanel;
class DockCentral;
class DockStatusBar;
class DockPage : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString uri READ uri WRITE setUri NOTIFY uriChanged)
    Q_PROPERTY(QQmlListProperty<mu::dock::DockToolBar> toolBars READ toolBarsProperty)
    Q_PROPERTY(QQmlListProperty<mu::dock::DockPanel> panels READ panelsProperty)
    Q_PROPERTY(mu::dock::DockCentral* centralDock READ centralDock WRITE setCentralDock NOTIFY centralDockChanged)
    Q_PROPERTY(QQmlListProperty<mu::dock::DockStatusBar> statusBars READ statusBarsProperty)

public:
    explicit DockPage(QQuickItem* parent = nullptr);

    QString uri() const;

    QQmlListProperty<mu::dock::DockToolBar> toolBarsProperty();
    QQmlListProperty<mu::dock::DockPanel> panelsProperty();
    DockCentral* centralDock() const;
    QQmlListProperty<mu::dock::DockStatusBar> statusBarsProperty();

    Q_INVOKABLE void init(DockWindow& window);

    void close();

public slots:
    void setUri(const QString& uri);
    void setCentralDock(DockCentral* central);

signals:
    void uniqueNameChanged(const QString& name);
    void uriChanged(const QString& uri);
    void centralDockChanged(DockCentral* central);

private:
    void componentComplete() override;

    QString m_uri;
    uicomponents::QmlListProperty<DockToolBar> m_toolBars;
    uicomponents::QmlListProperty<DockPanel> m_panels;
    DockCentral* m_central = nullptr;
    uicomponents::QmlListProperty<DockStatusBar> m_statusBars;
};
}

#endif // MU_DOCK_DOCKPAGE_H
