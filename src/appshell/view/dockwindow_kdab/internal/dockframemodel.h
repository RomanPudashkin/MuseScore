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
#ifndef MU_DOCK_DOCKFRAMEMODEL_H
#define MU_DOCK_DOCKFRAMEMODEL_H

#include <QObject>
#include <QQuickItem>

namespace mu::dock {
class DockFrameModel : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem* frame READ frame WRITE setFrame NOTIFY frameChanged)
    Q_PROPERTY(bool titleBarVisible READ titleBarVisible NOTIFY titleBarVisibleChanged)

public:
    explicit DockFrameModel(QObject* parent = nullptr);

    QQuickItem* frame() const;
    bool titleBarVisible() const;

public slots:
    void setFrame(QQuickItem* item);

signals:
    void frameChanged(QQuickItem* frame);
    void titleBarVisibleChanged(bool visible);

private:
    void listenChangesInFrame();
    void setTitleBarVisible(bool visible);

    QQuickItem* m_frame = nullptr;
    bool m_titleBarVisible = false;
};
}

#endif // MU_DOCK_DOCKFRAMEMODEL_H
