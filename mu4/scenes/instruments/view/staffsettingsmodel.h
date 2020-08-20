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
#ifndef MU_INSTRUMENTS_STAFFSETTINGSMODEL_H
#define MU_INSTRUMENTS_STAFFSETTINGSMODEL_H

#include <QObject>
#include <QQmlEngine>

#include "modularity/ioc.h"
#include "context/iglobalcontext.h"

namespace mu {
namespace scene {
namespace instruments {
class StaffSettingsModel : public QObject
{
    Q_OBJECT

    INJECT(instruments, context::IGlobalContext, globalContext)

public:
    explicit StaffSettingsModel(QObject* parent = nullptr);

    Q_INVOKABLE void load();

    Q_INVOKABLE QStringList allStaffTypes() const;
    Q_INVOKABLE QString staffType() const;
    Q_INVOKABLE void setStaffType(int index);

    Q_INVOKABLE QVariantList voices() const;
    Q_INVOKABLE void setVoiceVisible(int index, bool visible);

    Q_INVOKABLE bool smallStaff() const;
    Q_INVOKABLE void setSmallStaff(bool small);

    Q_INVOKABLE bool measuresHidden() const;
    Q_INVOKABLE void hideMeasures(bool hide);

    Q_INVOKABLE void createLinkedStaff();
};
}
}
}

#endif // MU_INSTRUMENTS_STAFFSETTINGSMODEL_H
