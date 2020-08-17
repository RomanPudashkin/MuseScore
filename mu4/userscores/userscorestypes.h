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

#ifndef MU_USERSCORES_TYPES_H
#define MU_USERSCORES_TYPES_H

#include <QList>

#include "domain/notation/notationtypes.h"

#include "libmscore/key.h"
#include "libmscore/timesig.h"

namespace mu {
namespace userscores {
using Key = Ms::Key;
using KeyMode = Ms::KeyMode;
using TimeSigType = Ms::TimeSigType;

struct Template : public domain::notation::Meta {
    QString categoryTitle;

    Template() = default;
    Template(const domain::notation::Meta& meta) : Meta(meta) {}

};

using Templates = QList<Template>;
}
}

#endif // MU_USERSCORES_TYPES_H
