//=============================================================================
//  MusE Score
//  Linux Music Score Editor
//
//  Copyright (C) 2002-2011 Werner Schweer and others
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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

namespace Ms {
extern bool enableExperimental;
extern bool noSeq;            ///< Don’t use sequencer; cmd line option.
extern bool noMidi;           ///< Don’t use midi; cmd line option.
extern bool midiInputTrace;   ///< debug option: dump midi input
extern bool midiOutputTrace;  ///< debug option: dump midi output
extern bool converterMode;
extern bool pluginMode;
extern double guiScaling;
extern int trimMargin;
extern bool noWebView;
extern bool ignoreWarnings;

enum TelemetryDataCollectionType : unsigned char {
    COLLECT_NO_DATA = 0,
    COLLECT_CRASH_FREE_DATA = 1,
    COLLECT_INSPECTOR_DATA = 1 << 1,
    COLLECT_SHORTCUT_AND_MENU_DATA = 1 << 2,
    COLLECT_ALL_DATA = COLLECT_CRASH_FREE_DATA & COLLECT_INSPECTOR_DATA & COLLECT_SHORTCUT_AND_MENU_DATA
};

constexpr TelemetryDataCollectionType enabledTelemetryDataTypes = TelemetryDataCollectionType::COLLECT_CRASH_FREE_DATA;

//---------------------------------------------------------
// MsWidget
// used to assign actions (shortcuts) to the appropriate
// widget (actions.cpp, shortcuts.cpp)
//---------------------------------------------------------

enum MsWidget {
    MAIN_WINDOW       = 0,
    SCORE_TAB         = 1,
    PIANO_ROLL_EDITOR = 2,
};

//---------------------------------------------------------
//   MidiRemoteType
//---------------------------------------------------------

enum MidiRemoteType {
    MIDI_REMOTE_TYPE_INACTIVE = -1,
    MIDI_REMOTE_TYPE_NOTEON = 0, MIDI_REMOTE_TYPE_CTRL
};

//---------------------------------------------------------
//   MidiRemote
//---------------------------------------------------------

struct MidiRemote {
    int channel;
    MidiRemoteType type;
    int data;           // pitch or controller number
};

static constexpr qreal DPI_DISPLAY     = 96.0;  // 96 DPI nominal resolution
static constexpr qreal DPMM_DISPLAY    = DPI_DISPLAY / 25.4;
static constexpr qreal PALETTE_SPATIUM = 1.764 * DPMM_DISPLAY;
} // namespace Ms
#endif
