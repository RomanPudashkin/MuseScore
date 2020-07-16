//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
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

#ifndef MU_INSPECTORS_APPEARANCESETTINGSMODEL_H
#define MU_INSPECTORS_APPEARANCESETTINGSMODEL_H

#include "view/abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace mu {
namespace inspectors {
class AppearanceSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT

    INJECT(inspectors, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(PropertyItem* leadingSpace READ leadingSpace CONSTANT)
    Q_PROPERTY(PropertyItem* barWidth READ barWidth CONSTANT)
    Q_PROPERTY(PropertyItem* minimumDistance READ minimumDistance CONSTANT)
    Q_PROPERTY(PropertyItem* color READ color CONSTANT)
    Q_PROPERTY(PropertyItem* arrangeOrder READ arrangeOrder CONSTANT)
    Q_PROPERTY(PropertyItem* horizontalOffset READ horizontalOffset CONSTANT)
    Q_PROPERTY(PropertyItem* verticalOffset READ verticalOffset CONSTANT)
    Q_PROPERTY(bool isSnappedToGrid READ isSnappedToGrid WRITE setIsSnappedToGrid NOTIFY isSnappedToGridChanged)

public:
    explicit AppearanceSettingsModel(QObject* parent, IElementRepositoryService* repository);

    Q_INVOKABLE void pushBackInOrder();
    Q_INVOKABLE void pushFrontInOrder();

    Q_INVOKABLE void configureGrid();

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    PropertyItem* leadingSpace() const;
    PropertyItem* barWidth() const;
    PropertyItem* minimumDistance() const;
    PropertyItem* color() const;
    PropertyItem* arrangeOrder() const;
    PropertyItem* horizontalOffset() const;
    PropertyItem* verticalOffset() const;

    bool isSnappedToGrid() const;

public slots:
    void setIsSnappedToGrid(bool isSnapped);

signals:
    void isSnappedToGridChanged(bool isSnappedToGrid);

private:
    PropertyItem* m_leadingSpace = nullptr;
    PropertyItem* m_barWidth = nullptr;
    PropertyItem* m_minimumDistance = nullptr;
    PropertyItem* m_color = nullptr;
    PropertyItem* m_arrangeOrder = nullptr;
    PropertyItem* m_horizontalOffset = nullptr;
    PropertyItem* m_verticalOffset = nullptr;

    bool m_isSnappedToGrid = false;
};
}
}

#endif // MU_INSPECTORS_APPEARANCESETTINGSMODEL_H
