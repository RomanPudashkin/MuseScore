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
#ifndef MU_EXTENSIONS_EXTENSIONSERRORS_H
#define MU_EXTENSIONS_EXTENSIONSERRORS_H

#include "ret.h"
#include "translation.h"

namespace mu {
namespace extensions {
enum class Err {
    Undefined       = int(Ret::Code::Undefined),
    NoError         = int(Ret::Code::Ok),
    UnknownError    = int(Ret::Code::ExtensionsFirst),

    ErrorParseConfig,
    ErrorLoadingExtension,

    UnpackDestinationReadOnly,
    UnpackNoFreeSpace,
    UnpackInvalidStructure,
    UnpackInvalidOldExtension,
    UnpackPreviousVersionExists,
    UnpackErrorRemovePreviousVersion,
    UnpackNoActualVersion,
    UnpackError
};

inline Ret make_ret(Err e)
{
    switch (e) {
    case Err::Undefined: return Ret(static_cast<int>(Ret::Code::Undefined));
    case Err::NoError: return Ret(static_cast<int>(Ret::Code::Ok));
    case Err::UnknownError: return Ret(static_cast<int>(Ret::Code::UnknownError));
    case Err::ErrorParseConfig: return Ret(static_cast<int>(Err::ErrorParseConfig),
                                           trc("extensions", "Error parsing response from server"));
    case Err::ErrorLoadingExtension: return Ret(static_cast<int>(Err::ErrorLoadingExtension),
                                                trc("extensions", "Error loading extension"));
    case Err::UnpackDestinationReadOnly: return Ret(static_cast<int>(Err::UnpackDestinationReadOnly),
                                                    trc("extensions", "Cannot import extension on read-only storage"));
    case Err::UnpackNoFreeSpace: return Ret(static_cast<int>(Err::UnpackNoFreeSpace),
                                            trc("extensions", "Cannot import extension on full storage"));
    case Err::UnpackInvalidStructure: return Ret(static_cast<int>(Err::UnpackInvalidStructure),
                                                 trc("extensions", "Invalid archive structure"));
    case Err::UnpackInvalidOldExtension: return Ret(static_cast<int>(Err::UnpackInvalidOldExtension),
                                                    trc("extensions", "Invalid old extension"));
    case Err::UnpackPreviousVersionExists: return Ret(static_cast<int>(Err::UnpackPreviousVersionExists),
                                                      trc("extensions", "Previous version of extension exists"));
    case Err::UnpackErrorRemovePreviousVersion: return Ret(static_cast<int>(Err::UnpackErrorRemovePreviousVersion),
                                                           trc("extensions", "Error removing previous version"));
    case Err::UnpackNoActualVersion: return Ret(static_cast<int>(Err::UnpackNoActualVersion),
                                                trc("extensions", "A newer version is already installed"));
    case Err::UnpackError: return Ret(static_cast<int>(Err::UnpackError),
                                      trc("extensions", "Error unpacking extension"));
    }

    return Ret(static_cast<int>(e));
}
}
}

#endif // MU_EXTENSIONS_EXTENSIONSERRORS_H
