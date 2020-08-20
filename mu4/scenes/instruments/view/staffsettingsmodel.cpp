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

#include "staffsettingsmodel.h"

#include "domain/notation/notationtypes.h"

#include "log.h"

using namespace mu::scene::instruments;
using namespace mu::domain::notation;

StaffSettingsModel::StaffSettingsModel(QObject* parent)
    : QObject(parent)
{

}

void StaffSettingsModel::load()
{
    NOT_IMPLEMENTED;
}

QStringList StaffSettingsModel::allStaffTypes() const
{
    NOT_IMPLEMENTED;

    QStringList examples {
        "Custom Standard",
        "Custom Percussion",
        "Custom Tablature"
    };

    return examples;
}

QString StaffSettingsModel::staffType() const
{
    NOT_IMPLEMENTED;

    return allStaffTypes()[0];
}

void StaffSettingsModel::setStaffType(int index)
{
    Q_UNUSED(index);

    NOT_IMPLEMENTED;
}

QVariantList StaffSettingsModel::voices() const
{
    QVariantList result;

    for (int i = 0; i < VOICES_COUNT; ++i) {
        QVariantMap voice;

        voice["title"] = i + 1;
        voice["visible"] = true;

        result << voice;
    }

    return result;
}

void StaffSettingsModel::setVoiceVisible(int index, bool visible)
{
    Q_UNUSED(index)
    Q_UNUSED(visible)

    NOT_IMPLEMENTED;
}

bool StaffSettingsModel::smallStaff() const
{
    NOT_IMPLEMENTED;

    return false;
}

void StaffSettingsModel::setSmallStaff(bool small)
{
    Q_UNUSED(small)

    NOT_IMPLEMENTED;
}

bool StaffSettingsModel::measuresHidden() const
{
    NOT_IMPLEMENTED;

    return false;
}

void StaffSettingsModel::hideMeasures(bool hide)
{
    Q_UNUSED(hide)

    NOT_IMPLEMENTED;
}

void StaffSettingsModel::createLinkedStaff()
{
    NOT_IMPLEMENTED;
}
