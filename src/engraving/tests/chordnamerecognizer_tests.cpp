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

#include <gtest/gtest.h>

#include "utils/chordnamerecognizer.h"
#include "dom/chord.h"
#include "dom/masterscore.h"

#include "utils/scorerw.h"

using namespace mu;
using namespace mu::engraving;

static const String CHORDNAMEPARSER_DATA_DIR("chordnameparser_data/");

class Engraving_ChordNameRecognizerTests : public ::testing::Test
{
};

TEST_F(Engraving_ChordNameRecognizerTests, RecognizeNames)
{
    // [GIVEN] There's a whole chord in each measure
    MasterScore* score = ScoreRW::readScore(CHORDNAMEPARSER_DATA_DIR + u"chords.mscx");
    EXPECT_TRUE(score);

    // [WHEN] Init recognizer
    ChordNameRecognizer recognizer(score);

    // [WHEN] Recognize chord names
    StringList actualNames;

    for (const Segment* seg = score->firstSegment(SegmentType::ChordRest); seg; seg = seg->next1(SegmentType::ChordRest)) {
        const EngravingItem* item = seg->elementAt(0);
        if (!item->isChord()) {
            continue;
        }

        String name = recognizer.recognizeName(toChord(item));
        actualNames << name;
    }

    // [THEN] The list of names matches the expected
    const StringList expectedNames {
        // Less than 3 notes, skip
        u"",

        // Major
        u"C",
        u"G",
        u"F",
        u"D",
        u"A",
        u"E",
        u"Bb",

        // Minor
        u"Am",
        u"Em",
        u"Dm",
        u"Bm",
        u"C#m",
        u"Gm",

        // Major 7
        u"CMaj7",
        u"G7",
        u"A7",
        u"D7",
        u"E7",
        u"B7",

        // Minor 7
        u"Cm7",
        u"Gm7",
        u"Am7",
        u"Dm7",
        u"Em7",
        u"Bm7",
    };

    ASSERT_EQ(actualNames.size(), expectedNames.size());

    for (size_t i = 0; i < expectedNames.size(); ++i) {
        const String& actual = actualNames.at(i);
        const String& expected = expectedNames.at(i);
        EXPECT_EQ(actual, expected);
    }

    delete score;
}
