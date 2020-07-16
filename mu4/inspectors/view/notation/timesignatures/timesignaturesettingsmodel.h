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

#ifndef MU_INSPECTORS_TIMESIGNATURESETTINGSMODEL_H
#define MU_INSPECTORS_TIMESIGNATURESETTINGSMODEL_H

#include "view/abstractinspectormodel.h"

#include "modularity/ioc.h"
#include "actions/iactionsdispatcher.h"

namespace mu {
namespace inspectors {
class TimeSignatureSettingsModel : public AbstractInspectorModel
{
    Q_OBJECT

    INJECT(inspectors, actions::IActionsDispatcher, dispatcher)

    Q_PROPERTY(PropertyItem* horizontalScale READ horizontalScale CONSTANT)
    Q_PROPERTY(PropertyItem* verticalScale READ verticalScale CONSTANT)
    Q_PROPERTY(PropertyItem* shouldShowCourtesy READ shouldShowCourtesy CONSTANT)

public:
    explicit TimeSignatureSettingsModel(QObject* parent, IElementRepositoryService* repository);

    Q_INVOKABLE void showTimeSignatureProperties();

    void createProperties() override;
    void requestElements() override;
    void loadProperties() override;
    void resetProperties() override;

    PropertyItem* horizontalScale() const;
    PropertyItem* verticalScale() const;
    PropertyItem* shouldShowCourtesy() const;

private:
    PropertyItem* m_horizontalScale = nullptr;
    PropertyItem* m_verticalScale = nullptr;
    PropertyItem* m_shouldShowCourtesy = nullptr;
};
}
}

#endif // MU_INSPECTORS_TIMESIGNATURESETTINGSMODEL_H
