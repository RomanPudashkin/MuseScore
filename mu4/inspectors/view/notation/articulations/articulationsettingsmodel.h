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

#ifndef MU_INSPECTORS_ARTICULATIONSETTINGSMODEL_H
#define MU_INSPECTORS_ARTICULATIONSETTINGSMODEL_H

#include "view/abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace mu {
namespace inspectors {
class ArticulationSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT

    INJECT(inspectors, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(PropertyItem* direction READ direction CONSTANT)
    Q_PROPERTY(PropertyItem* placement READ placement CONSTANT)

public:
    explicit ArticulationSettingsModel(QObject* parent, IElementRepositoryService* repository);

    Q_INVOKABLE void openChannelAndMidiProperties();

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    PropertyItem* direction() const;
    PropertyItem* placement() const;

private:
    PropertyItem* m_direction = nullptr;
    PropertyItem* m_placement = nullptr;
};
}
}

#endif // MU_INSPECTORS_ARTICULATIONSETTINGSMODEL_H
