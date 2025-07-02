/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "chordnamerecognizer.h"

#include "dom/score.h"
#include "dom/chord.h"
#include "dom/note.h"

using namespace mu::engraving;

using PitchClassMap = std::map<int, const Note*>;

static PitchClassMap notesToPitchClasses(const Chord* chord)
{
    PitchClassMap result;
    for (const Note* note : chord->notes()) {
        result[note->pitch() % 12] = note;
    }

    return result;
}

static int calculateKeys(const PitchClassMap& pitchClasses, int root)
{
    std::set<int> interval;
    for (const auto& pair : pitchClasses) {
        interval.insert((pair.first - root + 12) % 12);
    }

    HChord hchord;
    for (int pc : interval) {
        hchord += pc;
    }

    return hchord.getKeys();
}

static String noteName(const Note* note)
{
    return tpc2name(note->tpc(), NoteSpellingType::STANDARD, NoteCaseType::UPPER);
}

ChordNameRecognizer::ChordNameRecognizer(Score* score)
{
    TRACEFUNC;

    score->checkChordList();

    if (score->style().styleB(Sid::chordsXmlFile)) {
        initChordNameMap(score->chordList());
    } else {
        ChordList chordList;
        chordList.read(u"chords.xml");
        initChordNameMap(&chordList);
    }
}

void ChordNameRecognizer::initChordNameMap(const ChordList* chordList)
{
    m_chordNameMap.clear();

    for (const auto& pair : *chordList) {
        const ChordDescription& desc = pair.second;
        if (desc.names.empty()) {
            continue;
        }

        const int keys = desc.chord.getKeys();
        const String& name = desc.names.front();

        auto it = m_chordNameMap.find(keys);
        if (it != m_chordNameMap.end()) {
            if (name.size() < it->second.size()) { // prefer shorter name
                it->second = name;
            }
        } else {
            m_chordNameMap[keys] = name;
        }
    }
}

muse::String ChordNameRecognizer::recognizeName(const Chord* chord) const
{
    if (chord->notes().size() < 3) {
        return muse::String();
    }

    const PitchClassMap pitchClasses = notesToPitchClasses(chord);
    const Note* bassNote = chord->downNote();
    const int bassNotePitchClass = bassNote->pitch() % 12;

    String bestMatch;

    for (const auto& pair : pitchClasses) {
        const int root = pair.first;
        const int keys = calculateKeys(pitchClasses, root);

        auto it = m_chordNameMap.find(keys);
        if (it == m_chordNameMap.end()) {
            continue;
        }

        String name = noteName(pair.second) + it->second;
        if (root != bassNotePitchClass) {
            name += u"/" + noteName(bassNote);
        }

        if (bestMatch.empty() || name.size() < bestMatch.size()) {
            bestMatch = name;
        }
    }

    return bestMatch;
}
