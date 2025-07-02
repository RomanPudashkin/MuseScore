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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "scoreelementsscanner.h"

#include "engraving/dom/score.h"
#include "engraving/dom/part.h"
#include "engraving/dom/note.h"
#include "engraving/dom/harmony.h"
#include "engraving/utils/chordnamerecognizer.h"

using namespace mu::converter;

struct ScannerData {
    // in
    ScoreElementScanner::Options options;
    std::shared_ptr<mu::engraving::ChordNameRecognizer> chordNameRecognizer;

    // out
    ScoreElementScanner::InstrumentElementMap elements;
    std::set<const mu::engraving::Chord*> chords;
};

static bool itemAccepted(const mu::engraving::EngravingItem* item, const mu::engraving::ElementTypeSet& acceptedTypes)
{
    // Ignore temporary / invalid elements and elements that cannot be interacted with
    if (!item || !item->part() || item->generated() || !item->selectable() || !item->isInteractionAvailable()) {
        return false;
    }

    if (acceptedTypes.empty()) {
        return true;
    }

    mu::engraving::ElementType type = item->type();

    if (item->isNote()) {
        const mu::engraving::Chord* chord = toNote(item)->chord();
        if (chord->notes().size() > 1) {
            type = chord->type();
        }
    } else if (item->isSpannerSegment()) {
        type = mu::engraving::toSpannerSegment(item)->spanner()->type();
    }

    return muse::contains(acceptedTypes, type);
}

static muse::String chordToVoicing(const mu::engraving::Chord* chord)
{
    muse::StringList notes;

    for (const mu::engraving::Note* note : chord->notes()) {
        notes.push_back(note->tpcUserName());
    }

    return notes.join(u" ");
}

static void addElementInfoIfNeed(void* data, mu::engraving::EngravingItem* item)
{
    ScannerData* scannerData = static_cast<ScannerData*>(data);
    if (!itemAccepted(item, scannerData->options.acceptedTypes)) {
        return;
    }

    mu::engraving::ElementType type = item->type();
    ScoreElementScanner::ElementInfo info;

    if (item->isNote()) {
        const mu::engraving::Chord* chord = mu::engraving::toNote(item)->chord();
        if (chord->notes().size() > 1) {
            if (muse::contains(scannerData->chords, chord)) {
                return;
            }

            type = chord->type();
            scannerData->chords.insert(chord);
        }

        info.name = chordToVoicing(chord);
    } else if (item->isSpannerSegment()) {
        const mu::engraving::Spanner* spanner = mu::engraving::toSpannerSegment(item)->spanner();
        type = spanner->type();
        info.name = spanner->translatedSubtypeUserName();
    } else if (item->isHarmony()) {
        info.name = mu::engraving::toHarmony(item)->harmonyName();
    } else {
        info.name = item->translatedSubtypeUserName();
    }

    if (info.name.empty()) {
        return;
    }

    const mu::engraving::Part* part = item->part();
    const mu::engraving::InstrumentTrackId trackId {
        part->id(),
        part->instrumentId(item->tick())
    };

    std::vector<ScoreElementScanner::ElementInfo>& elements = scannerData->elements[trackId][type];
    if (scannerData->options.avoidDuplicates) {
        bool contains = std::find_if(elements.begin(), elements.end(), [&info](const ScoreElementScanner::ElementInfo& e) {
            return e.name == info.name;
        }) != elements.end();

        if (contains) {
            return;
        }
    }

    const mu::engraving::EngravingItem::BarBeat barbeat = item->barbeat();
    info.staffIdx = item->staffIdx();
    info.voiceIdx = item->voice();
    info.measureIdx = barbeat.bar - 1;
    info.beat = barbeat.beat - 1.;
    elements.push_back(info);
}

ScoreElementScanner::InstrumentElementMap ScoreElementScanner::scanElements(mu::engraving::Score* score, const Options& options)
{
    ScannerData data;
    data.options = options;
    data.chordNameRecognizer = std::make_shared<mu::engraving::ChordNameRecognizer>(score);

    score->scanElements(&data, addElementInfoIfNeed, false);

    return data.elements;
}
