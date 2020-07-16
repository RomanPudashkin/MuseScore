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

#ifndef MU_SCORES_SCORESTYPES_H
#define MU_SCORES_SCORESTYPES_H

namespace Ms {
enum ScoreState {
    STATE_INIT                                = 0,
    STATE_DISABLED                            = 1 << 0,
    STATE_NORMAL                              = 1 << 1,
    STATE_NOTE_ENTRY_STAFF_DRUM               = 1 << 2,
    STATE_NOTE_ENTRY_STAFF_TAB                = 1 << 3,
    STATE_NOTE_ENTRY_METHOD_STEPTIME          = 1 << 4,
    STATE_NOTE_ENTRY_METHOD_REPITCH           = 1 << 5,
    STATE_NOTE_ENTRY_METHOD_RHYTHM            = 1 << 6,
    STATE_NOTE_ENTRY_METHOD_REALTIME_AUTO     = 1 << 7,
    STATE_NOTE_ENTRY_METHOD_REALTIME_MANUAL   = 1 << 8,
    STATE_EDIT                                = 1 << 9,
    STATE_TEXT_EDIT                           = 1 << 10,
    STATE_LYRICS_EDIT                         = 1 << 11,
    STATE_HARMONY_FIGBASS_EDIT                = 1 << 12,
    STATE_PLAY                                = 1 << 13,
    STATE_FOTO                                = 1 << 14,
    STATE_LOCK                                = 1 << 15,
    STATE_NOTE_ENTRY_METHOD_TIMEWISE          = 1 << 16,

    STATE_NEVER                               = 1 << 31,

    STATE_NOTE_ENTRY_STAFF_PITCHED = STATE_NOTE_ENTRY_METHOD_STEPTIME
                                     | STATE_NOTE_ENTRY_METHOD_REPITCH
                                     | STATE_NOTE_ENTRY_METHOD_RHYTHM
                                     | STATE_NOTE_ENTRY_METHOD_REALTIME_AUTO
                                     | STATE_NOTE_ENTRY_METHOD_REALTIME_MANUAL
                                     | STATE_NOTE_ENTRY_METHOD_TIMEWISE,
    STATE_NOTE_ENTRY        = STATE_NOTE_ENTRY_STAFF_PITCHED | STATE_NOTE_ENTRY_STAFF_DRUM | STATE_NOTE_ENTRY_STAFF_TAB,
    STATE_ALLTEXTUAL_EDIT   = STATE_TEXT_EDIT | STATE_LYRICS_EDIT | STATE_HARMONY_FIGBASS_EDIT,
    STATE_ALL               = -1
};
}

#endif // MU_SCORES_SCORESTYPES_H
