/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
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

#include "playbackcontext.h"

#include <cmath>

#include "dom/lyrics.h"
#include "dom/measure.h"
#include "dom/measurerepeat.h"
#include "dom/part.h"
#include "dom/playtechannotation.h"
#include "dom/repeatlist.h"
#include "dom/score.h"
#include "dom/segment.h"
#include "dom/soundflag.h"
#include "dom/staff.h"
#include "dom/stafftext.h"

#include "engraving/automation/iautomation.h"

#include "global/realfn.h"

#include "utils/arrangementutils.h"
#include "utils/expressionutils.h"
#include "types/constants.h"
#include "types/typesconv.h"

using namespace mu::engraving;
using namespace muse;
using namespace muse::mpe;

static bool soundFlagPlayable(const SoundFlag* flag)
{
    if (flag && flag->play()) {
        return !flag->soundPresets().empty() || !flag->playingTechnique().empty();
    }

    return false;
}

static dynamic_level_t toDynamicLevel(double normalizedValue)
{
    return static_cast<dynamic_level_t>(std::lround(normalizedValue * MAX_DYNAMIC_LEVEL));
}

dynamic_level_t PlaybackContext::appliableDynamicLevel(const track_idx_t trackIdx, const int nominalPositionTick) const
{
    const IAutomation* automation = m_score ? m_score->automation() : nullptr;
    if (!automation) {
        return NATURAL_DYNAMIC_LEVEL;
    }

    const Staff* staff = m_score->staff(track2staff(trackIdx));
    if (!staff) {
        return NATURAL_DYNAMIC_LEVEL;
    }

    AutomationCurveKey key;
    key.type = AutomationType::Dynamics;
    key.staffId = staff->id();
    if (muse::contains(m_voiceDynamicTracks, trackIdx)) {
        key.voiceIdx = track2voice(trackIdx);
    }

    const AutomationCurve& curve = automation->curve(key);
    const auto it = muse::findLessOrEqual(curve, nominalPositionTick);
    if (it == curve.cend()) {
        return NATURAL_DYNAMIC_LEVEL;
    }

    const auto nextIt = std::next(it);
    if (nextIt == curve.cend()) {
        return toDynamicLevel(it->second.outValue);
    }

    const ChangeMethod method = nextIt->second.interpolation == AutomationPoint::InterpolationType::Exponential
                                ? ChangeMethod::EXPONENTIAL
                                : ChangeMethod::NORMAL;
    const double factor = static_cast<double>(nominalPositionTick - it->first) / (nextIt->first - it->first);
    const double easedFactor = TConv::easingFactor(static_cast<float>(factor), method);
    const double value = it->second.outValue + easedFactor * (nextIt->second.inValue - it->second.outValue);
    return toDynamicLevel(value);
}

std::pair<mpe::timestamp_t, PlayingTechniqueType> PlaybackContext::playingTechnique(const int nominalPositionTick) const
{
    auto it = findLessOrEqual(m_playTechniquesMap, nominalPositionTick);
    if (it == m_playTechniquesMap.cend()) {
        return std::make_pair(0, PlayingTechniqueType::Natural);
    }

    return std::make_pair(timestampFromTicks(m_score, it->first), it->second);
}

muse::mpe::timestamp_t PlaybackContext::findPlayingTechniqueTimestamp(PlayingTechniqueType type,
                                                                      const int startFromTick) const
{
    auto it = m_playTechniquesMap.upper_bound(startFromTick);

    for (; it != m_playTechniquesMap.end(); ++it) {
        if (it->second == type) {
            return timestampFromTicks(m_score, it->first);
        }
    }

    return -1;
}

std::map<timestamp_t, SoundPresetChangeEventList> PlaybackContext::soundPresets() const
{
    std::map<timestamp_t, SoundPresetChangeEventList> result;

    for (const auto& trackPair : m_soundPresetsByTrack) {
        if (shouldSkipTrack(trackPair.first)) {
            continue;
        }

        for (const auto& pair : trackPair.second) {
            const timestamp_t timestamp = timestampFromTicks(m_score, pair.first);
            SoundPresetChangeEventList& list = result[timestamp];
            list.insert(list.end(), pair.second.begin(), pair.second.end());
        }
    }

    return result;
}

SoundPresetChangeEventList PlaybackContext::soundPresets(const track_idx_t trackIdx, const int nominalPositionTick) const
{
    auto presetsIt = m_soundPresetsByTrack.find(trackIdx);
    if (presetsIt == m_soundPresetsByTrack.end()) {
        return {};
    }

    const SoundPresetsMap& map = presetsIt->second;
    auto it = muse::findLessOrEqual(map, nominalPositionTick);
    if (it == map.end()) {
        return {};
    }

    return it->second;
}

std::map<timestamp_t, TextArticulationEventList> PlaybackContext::textArticulations() const
{
    std::map<timestamp_t, TextArticulationEventList> result;

    for (const auto& trackPair : m_textArticulationsByTrack) {
        if (shouldSkipTrack(trackPair.first)) {
            continue;
        }

        for (const auto& pair : trackPair.second) {
            const timestamp_t timestamp = timestampFromTicks(m_score, pair.first);
            result[timestamp].push_back(pair.second);
        }
    }

    return result;
}

TextArticulationEvent PlaybackContext::textArticulation(const track_idx_t trackIdx, const int nominalPositionTick) const
{
    auto articulationsIt = m_textArticulationsByTrack.find(trackIdx);
    if (articulationsIt == m_textArticulationsByTrack.end()) {
        return {};
    }

    const TextArticulationMap& map = articulationsIt->second;
    auto it = muse::findLessOrEqual(map, nominalPositionTick);
    if (it == map.end()) {
        return {};
    }

    TextArticulationEvent result = it->second;
    result.flags.setFlag(TextArticulationEvent::StartsAtPlaybackPosition, it->first == nominalPositionTick);

    return result;
}

std::map<timestamp_t, SyllableEventList> PlaybackContext::syllables() const
{
    std::map<timestamp_t, SyllableEventList> result;

    for (const auto& trackPair : m_syllablesByTrack) {
        if (shouldSkipTrack(trackPair.first)) {
            continue;
        }

        for (const auto& pair : trackPair.second) {
            const timestamp_t timestamp = timestampFromTicks(m_score, pair.first);
            result[timestamp].push_back(pair.second);
        }
    }

    return result;
}

SyllableEvent PlaybackContext::syllable(const track_idx_t trackIdx, const int nominalPositionTick) const
{
    auto syllablesIt = m_syllablesByTrack.find(trackIdx);
    if (syllablesIt == m_syllablesByTrack.end()) {
        return {};
    }

    const SyllableMap& map = syllablesIt->second;
    auto it = muse::findLessOrEqual(map, nominalPositionTick);
    if (it == map.end()) {
        return {};
    }

    SyllableEvent result = it->second;
    result.flags.setFlag(SyllableEvent::StartsAtPlaybackPosition, it->first == nominalPositionTick);

    return result;
}

DynamicLevelLayers PlaybackContext::dynamicLevelLayers() const
{
    TRACEFUNC;

    const IAutomation* automation = m_score ? m_score->automation() : nullptr;
    if (!automation) {
        return {};
    }

    DynamicLevelLayers result;
    AutomationCurveKey lastSharedKey;
    DynamicLevelMap lastSharedLevelMap;

    for (track_idx_t trackIdx = m_partStartTrack; trackIdx < m_partEndTrack; ++trackIdx) {
        const Staff* staff = m_score->staff(track2staff(trackIdx));
        if (!staff) {
            continue;
        }

        AutomationCurveKey key;
        key.type = AutomationType::Dynamics;
        key.staffId = staff->id();

        const bool isVoiceTrack = muse::contains(m_voiceDynamicTracks, trackIdx);
        if (isVoiceTrack) {
            key.voiceIdx = track2voice(trackIdx);
        } else if (key == lastSharedKey) {
            if (!lastSharedLevelMap.empty()) {
                result[static_cast<layer_idx_t>(trackIdx)] = lastSharedLevelMap;
            }
            continue;
        }

        const AutomationCurve& curve = automation->curve(key);
        if (curve.empty()) {
            continue;
        }

        DynamicLevelMap& levelMap = result[static_cast<layer_idx_t>(trackIdx)];

        if (curve.cbegin()->first > 0) {
            levelMap[timestampFromTicks(m_score, 0)] = NATURAL_DYNAMIC_LEVEL;
        }

        for (auto it = curve.cbegin(); it != curve.cend(); ++it) {
            const dynamic_level_t baseLvl = toDynamicLevel(it->second.outValue);
            levelMap[timestampFromTicks(m_score, it->first)] = baseLvl;

            const auto nextIt = std::next(it);
            if (nextIt == curve.cend()) {
                continue;
            }

            const dynamic_level_t nextLvl = toDynamicLevel(nextIt->second.inValue);
            const int range = nextLvl - baseLvl;
            if (range == 0) {
                continue;
            }

            const ChangeMethod method = nextIt->second.interpolation == AutomationPoint::InterpolationType::Exponential
                                        ? ChangeMethod::EXPONENTIAL : ChangeMethod::NORMAL;

            // If nextIt is itself a discontinuity (e.g. a hairpin arriving at a value right before a sudden
            // dynamic change), its own map entry below will use outValue, so the ramp's true target (inValue)
            // is instead recorded explicitly one tick early — interpolate the interior samples across that
            // shorter interval so they approach the tick where the target actually lands.
            const bool nextIsDiscontinuity = !muse::RealIsEqual(nextIt->second.inValue, nextIt->second.outValue);
            const utick_t targetTick = nextIsDiscontinuity ? nextIt->first - 1 : nextIt->first;
            const int intervalDuration = targetTick - it->first;
            const int steps = std::max(intervalDuration / (Constants::DIVISION / 4), 24);

            for (int j = 1; j < steps; ++j) {
                const utick_t tick = it->first + static_cast<utick_t>(static_cast<float>(j) * intervalDuration / steps);
                const double factor = TConv::easingFactor(static_cast<float>(j) / static_cast<float>(steps), method);
                levelMap[timestampFromTicks(m_score, tick)] = static_cast<dynamic_level_t>(std::lround(baseLvl + factor * range));
            }

            if (nextIsDiscontinuity) {
                levelMap[timestampFromTicks(m_score, targetTick)] = nextLvl;
            }
        }

        if (!isVoiceTrack) {
            lastSharedKey = key;
            lastSharedLevelMap = levelMap;
        }
    }

    return result;
}

void PlaybackContext::updateVoiceDynamicTracks()
{
    m_voiceDynamicTracks.clear();

    const IAutomation* automation = m_score->automation();
    if (!automation) {
        return;
    }

    for (const auto& [key, curve] : automation->curves()) {
        if (key.type != AutomationType::Dynamics || !key.voiceIdx.has_value() || curve.empty()) {
            continue;
        }
        const Staff* staff = m_score->staffById(key.staffId);
        if (staff) {
            m_voiceDynamicTracks.insert(staff2track(staff->idx(), *key.voiceIdx));
        }
    }
}

void PlaybackContext::update(const ID partId, const Score* score, bool expandRepeats)
{
    TRACEFUNC;

    const Part* part = score->partById(partId);
    IF_ASSERT_FAILED(part) {
        return;
    }

    m_score = score;
    updateVoiceDynamicTracks();

    m_partStartTrack = part->startTrack();
    m_partEndTrack = part->endTrack();

    IF_ASSERT_FAILED(m_partStartTrack <= m_partEndTrack) {
        return;
    }

    for (const RepeatSegment* repeatSegment : score->repeatList(expandRepeats)) {
        std::vector<const MeasureRepeat*> measureRepeats;
        int tickPositionOffset = repeatSegment->utick - repeatSegment->tick;

        for (const Measure* measure : repeatSegment->measureList()) {
            for (const Segment* segment = measure->first(); segment; segment = segment->next()) {
                int segmentStartTick = segment->tick().ticks() + tickPositionOffset;

                handleSegmentElements(repeatSegment, segment, segmentStartTick, measureRepeats);
                handleSegmentAnnotations(partId, segment, segmentStartTick);
            }
        }

        handleMeasureRepeats(measureRepeats, tickPositionOffset);
    }
}

void PlaybackContext::clear()
{
    m_score = nullptr;
    m_partStartTrack = 0;
    m_partEndTrack = 0;
    m_usedVoices.clear();
    m_voiceDynamicTracks.clear();
    m_playTechniquesMap.clear();
    m_soundPresetsByTrack.clear();
    m_textArticulationsByTrack.clear();
    m_syllablesByTrack.clear();
    m_currentVerseNumByChordRest.clear();
    m_multiVerseLyricsPositionMap.clear();
}

bool PlaybackContext::hasSoundFlags() const
{
    return !m_soundPresetsByTrack.empty() || !m_textArticulationsByTrack.empty();
}

void PlaybackContext::updatePlayTechMap(const PlayTechAnnotation* annotation, const int segmentPositionTick)
{
    if (!annotation->playPlayTechAnnotation()) {
        return;
    }
    const PlayingTechniqueType type = annotation->techniqueType();
    if (type == PlayingTechniqueType::Undefined) {
        return;
    }

    m_playTechniquesMap[segmentPositionTick] = type;

    bool cancelPlayTechniques = type == PlayingTechniqueType::Natural || type == PlayingTechniqueType::Open;

    if (cancelPlayTechniques && !m_textArticulationsByTrack.empty()) {
        TextArticulationEvent textArticulation;
        textArticulation.text = mpe::ORDINARY_PLAYING_TECHNIQUE_CODE;

        for (track_idx_t idx = m_partStartTrack; idx < m_partEndTrack; ++idx) {
            textArticulation.layerIdx = static_cast<layer_idx_t>(idx);
            m_textArticulationsByTrack[idx][segmentPositionTick] = textArticulation;
        }
    }
}

void PlaybackContext::updateSoundPresetAndTextArticulationMap(const SoundFlagMap& flagsOnSegment, const int segmentPositionTick)
{
    auto trackAccepted = [&flagsOnSegment](const SoundFlag* flag, track_idx_t trackIdx) {
        staff_idx_t staffIdx = track2staff(trackIdx);

        if (flag->staffIdx() == staffIdx) {
            return true;
        }

        if (flag->applyToAllStaves()) {
            return !muse::contains(flagsOnSegment, staffIdx);
        }

        return false;
    };

    for (const auto& pair : flagsOnSegment) {
        const SoundFlag* flag = pair.second;

        for (track_idx_t trackIdx = m_partStartTrack; trackIdx < m_partEndTrack; ++trackIdx) {
            if (!trackAccepted(flag, trackIdx)) {
                continue;
            }

            for (const String& soundPreset : flag->soundPresets()) {
                if (soundPreset.empty()) {
                    continue;
                }

                SoundPresetChangeEvent event;
                event.code = soundPreset;
                event.layerIdx = static_cast<layer_idx_t>(trackIdx);
                m_soundPresetsByTrack[trackIdx][segmentPositionTick].emplace_back(std::move(event));
            }

            if (!flag->playingTechnique().empty()) {
                TextArticulationEvent event;
                event.text = flag->playingTechnique();
                event.layerIdx = static_cast<layer_idx_t>(trackIdx);
                m_textArticulationsByTrack[trackIdx][segmentPositionTick] = std::move(event);
            }
        }

        if (flag->playingTechnique() == mpe::ORDINARY_PLAYING_TECHNIQUE_CODE) {
            m_playTechniquesMap[segmentPositionTick] = PlayingTechniqueType::Natural;
        }
    }
}

void PlaybackContext::updateSyllableMap(const TextBase* text, const int segmentPositionTick)
{
    IF_ASSERT_FAILED(text->isLyrics() || text->isSticking()) {
        return;
    }

    if (text->empty()) {
        return;
    }

    SyllableEvent syllable;
    syllable.text = text->plainText();

    if (text->isLyrics()) {
        const Lyrics* lyrics = toLyrics(text);

        switch (lyrics->syllabic()) {
        case LyricsSyllabic::BEGIN:
        case LyricsSyllabic::MIDDLE:
            syllable.flags.setFlag(SyllableEvent::HyphenedToNext);
            break;
        case LyricsSyllabic::SINGLE:
        case LyricsSyllabic::END:
            break;
        }
    }

    const staff_idx_t staffIdx = text->staffIdx();

    for (voice_idx_t voiceIdx = 0; voiceIdx < VOICES; ++voiceIdx) {
        track_idx_t trackIdx = staff2track(staffIdx, voiceIdx);
        syllable.layerIdx = static_cast<layer_idx_t>(trackIdx);
        m_syllablesByTrack[trackIdx][segmentPositionTick] = syllable;
    }
}

void PlaybackContext::handleSegmentAnnotations(const ID partId, const Segment* segment, const int segmentPositionTick)
{
    SoundFlagMap soundFlagsOnSegment;

    for (const EngravingItem* annotation : segment->annotations()) {
        if (!annotation || !annotation->part()) {
            continue;
        }

        if (annotation->part()->id() != partId.toUint64()) {
            continue;
        }

        if (annotation->isPlayTechAnnotation()) {
            updatePlayTechMap(toPlayTechAnnotation(annotation), segmentPositionTick);
            continue;
        }

        if (annotation->isSticking()) {
            updateSyllableMap(toTextBase(annotation), segmentPositionTick);
            continue;
        }

        if (annotation->isStaffText()) {
            if (const SoundFlag* flag = toStaffText(annotation)->soundFlag()) {
                if (soundFlagPlayable(flag)) {
                    soundFlagsOnSegment.emplace(flag->staffIdx(), flag);
                }
            }
        }
    }

    if (!soundFlagsOnSegment.empty()) {
        updateSoundPresetAndTextArticulationMap(soundFlagsOnSegment, segmentPositionTick);
    }
}

void PlaybackContext::handleSegmentElements(const RepeatSegment* repeat, const Segment* segment,
                                            const int segmentPositionTick,
                                            std::vector<const MeasureRepeat*>& foundMeasureRepeats)
{
    for (track_idx_t track = m_partStartTrack; track < m_partEndTrack; ++track) {
        const EngravingItem* item = segment->element(track);
        if (!item) {
            continue;
        }

        if (item->isMeasureRepeat()) {
            foundMeasureRepeats.push_back(toMeasureRepeat(item));
            continue;
        }

        if (item->isChordRest()) {
            m_usedVoices.insert(item->voice());

            const ChordRest* chordRest = toChordRest(item);
            if (chordRest->lyrics().empty()) {
                continue;
            }

            const Lyrics* lyrics = nullptr;

            auto verseNumIt = m_currentVerseNumByChordRest.find(chordRest);
            if (verseNumIt == m_currentVerseNumByChordRest.end()) {
                m_currentVerseNumByChordRest[chordRest] = 0;
                lyrics = chordRest->lyrics(0);
                if (chordRest->lyrics().size() > 1) {
                    m_multiVerseLyricsPositionMap[track].insert(chordRest->tick().ticks());
                }
            } else if (hasOnlyOneLyricsVerse(repeat, track)) {
                lyrics = chordRest->lyrics(0);
            } else {
                verseNumIt->second++;
                lyrics = chordRest->lyrics(verseNumIt->second);
            }

            if (lyrics) {
                updateSyllableMap(lyrics, segmentPositionTick);
            }
        }
    }
}

template<typename ItemsMap>
static void copyItemsInRange(ItemsMap& source, const int rangeStartTick, const int rangeEndTick, const int newItemsOffsetTick)
{
    auto startIt = source.lower_bound(rangeStartTick);
    if (startIt == source.end()) {
        return;
    }

    auto endIt = source.lower_bound(rangeEndTick);

    ItemsMap newItems;
    for (auto it = startIt; it != endIt; ++it) {
        int tick = it->first + newItemsOffsetTick;
        newItems.insert_or_assign(tick, it->second);
    }

    source.merge(std::move(newItems));
}

template<typename ItemsMap>
static void copyItemsInRange(std::map<track_idx_t, ItemsMap>& source, const int rangeStartTick, const int rangeEndTick,
                             const int newItemsOffsetTick)
{
    for (auto& pair : source) {
        copyItemsInRange(pair.second, rangeStartTick, rangeEndTick, newItemsOffsetTick);
    }
}

void PlaybackContext::handleMeasureRepeats(const std::vector<const MeasureRepeat*>& measureRepeats, const int tickPositionOffset)
{
    for (const MeasureRepeat* mr : measureRepeats) {
        const Measure* currMeasure = mr->firstMeasureOfGroup();
        if (!currMeasure) {
            continue;
        }

        const Measure* referringMeasure = mr->referringMeasure(currMeasure);
        if (!referringMeasure) {
            continue;
        }

        int currentMeasureTick = currMeasure->tick().ticks();
        int referringMeasureTick = referringMeasure->tick().ticks();
        int newItemsOffsetTick = currentMeasureTick - referringMeasureTick;

        for (int num = 0; num < mr->numMeasures(); ++num) {
            int startTick = referringMeasure->tick().ticks() + tickPositionOffset;
            int endTick = referringMeasure->endTick().ticks() + tickPositionOffset;

            copyItemsInRange(m_soundPresetsByTrack, startTick, endTick, newItemsOffsetTick);
            copyItemsInRange(m_textArticulationsByTrack, startTick, endTick, newItemsOffsetTick);
            copyItemsInRange(m_syllablesByTrack, startTick, endTick, newItemsOffsetTick);
            copyItemsInRange(m_playTechniquesMap, startTick, endTick, newItemsOffsetTick);

            currMeasure = currMeasure->nextMeasure();
            if (!currMeasure) {
                break;
            }

            referringMeasure = mr->referringMeasure(currMeasure);
            if (!referringMeasure) {
                break;
            }
        }
    }
}

bool PlaybackContext::shouldSkipTrack(const track_idx_t trackIdx) const
{
    return !muse::contains(m_usedVoices, track2voice(trackIdx));
}

bool PlaybackContext::hasOnlyOneLyricsVerse(const RepeatSegment* repeat, const track_idx_t track) const
{
    if (m_multiVerseLyricsPositionMap.empty()) {
        return true;
    }

    const auto trackIt = m_multiVerseLyricsPositionMap.find(track);
    if (trackIt == m_multiVerseLyricsPositionMap.cend()) {
        return true;
    }

    const int startTick = repeat->tick;
    const int endTick = repeat->endTick();
    const auto start = trackIt->second.lower_bound(startTick);
    const auto end = trackIt->second.lower_bound(endTick);

    return start == end;
}
