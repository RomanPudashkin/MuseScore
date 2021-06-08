/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef MU_DOCK_DOCKTOOLBAR_H
#define MU_DOCK_DOCKTOOLBAR_H

#include "internal/dockbase.h"

namespace mu::dock {
class DockToolBar : public DockBase
{
    Q_OBJECT

    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    Q_PROPERTY(
        int horizontalFloatingLength READ horizontalFloatingLength WRITE setHorizontalFloatingLength NOTIFY horizontalFloatingLengthChanged)
    Q_PROPERTY(int verticalFloatingLength READ verticalFloatingLength WRITE setVerticalFloatingLength NOTIFY verticalFloatingLengthChanged)

public:
    explicit DockToolBar(QQuickItem* parent = nullptr);

    Qt::Orientation orientation() const;

    Q_INVOKABLE void setDraggableMouseArea(QQuickItem* mouseArea);

    int horizontalFloatingLength() const;
    int verticalFloatingLength() const;

public slots:
    void setOrientation(Qt::Orientation orientation);
    void setHorizontalFloatingLength(int length);
    void setVerticalFloatingLength(int length);

    void setMinimumWidth(int width) override;
    void setMinimumHeight(int height) override;
    void setMaximumWidth(int width) override;
    void setMaximumHeight(int height) override;

signals:
    void orientationChanged(Qt::Orientation orientation);
    void horizontalFloatingLengthChanged(int length);
    void verticalFloatingLengthChanged(int length);

protected:
    void componentComplete() override;

    DockType type() const override;

    static const int MIN_SIDE_SIZE;
    static const int MAX_SIDE_SIZE;

private slots:
    void applyFloatingSizeConstraints();

private:
    bool isOrientationChangingAllowed() const;

    class DraggableArea;
    DraggableArea* m_draggableArea = nullptr;

    Qt::Orientation m_orientation = Qt::Horizontal;
    int m_horizontalFloatingLength = 0;
    int m_verticalFloatingLength = 0;
};
}

#endif // MU_DOCK_DOCKTOOLBAR_H
