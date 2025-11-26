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

#include "scoreelementsreader.h"

#include "global/serialization/json.h"
#include "global/types/retval.h"

#include "engraving/dom/arpeggio.h"
#include "engraving/dom/barline.h"
#include "engraving/dom/masterscore.h"
#include "engraving/dom/part.h"
#include "engraving/dom/pedal.h"
#include "engraving/dom/playtechannotation.h"
#include "engraving/dom/factory.h"
#include "engraving/dom/instrtemplate.h"
#include "engraving/dom/timesig.h"
#include "engraving/dom/tempotext.h"
#include "engraving/dom/keysig.h"
#include "engraving/dom/rest.h"
#include "engraving/dom/note.h"
#include "engraving/dom/lyrics.h"
#include "engraving/dom/hairpin.h"
#include "engraving/dom/harmony.h"
#include "engraving/dom/stafftext.h"

#include "engraving/types/typesconv.h"

#include "backendtypes.h"

#include "log.h"

using namespace mu::converter;
using namespace mu::engraving;

struct ElementsContext {
    std::vector<Note*> tiedToNextNotes;
};

static Fraction locToTick(const Score* score, const ElementInfo::Location& loc)
{
    return Fraction::fromTicks(score->sigmap()->bar2tick(loc.measureIdx, loc.beat));
}

static track_idx_t locToTrack(const ElementInfo::Location& loc)
{
    return staff2track(loc.staffIdx, loc.voiceIdx);
}

static ElementInfo::Location parseLocation(const muse::JsonObject& obj)
{
    ElementInfo::Location loc;

    if (obj.contains("measureIdx")) {
        loc.measureIdx = (size_t)obj["measureIdx"].toInt();
    }

    if (obj.contains("beat")) {
        loc.beat = obj["beat"].toDouble();
    }

    if (obj.contains("staffIdx")) {
        loc.staffIdx = (staff_idx_t)obj["staffIdx"].toInt();
    }

    if (obj.contains("voiceIdx")) {
        loc.voiceIdx = (voice_idx_t)obj["voiceIdx"].toInt();
    }

    return loc;
}

static ElementInfo::NoteList parseNotes(const muse::JsonArray& notesArray)
{
    ElementInfo::NoteList notes;
    notes.reserve(notesArray.size());

    for (size_t i = 0; i < notesArray.size(); ++i) {
        const muse::JsonObject noteObj = notesArray.at(i).toObject();

        ElementInfo::Note note;
        note.name = noteObj["name"].toString();

        for (const std::string& key : noteObj.keys()) {
            if (key != "name") {
                note.data[String::fromStdString(key)] = muse::Val(noteObj[key].toStdString());
            }
        }

        notes.emplace_back(std::move(note));
    }

    return notes;
}

static ElementInfo parseElementInfo(const muse::JsonObject& obj)
{
    ElementInfo info;
    info.type = TConv::fromXml(obj["type"].toStdString().c_str(), ElementType::INVALID);
    info.name = obj["name"].toString();

    if (obj.contains("duration")) {
        const muse::JsonObject durObj = obj["duration"].toObject();
        info.duration.name = durObj["name"].toString();
        info.duration.dots = durObj["dots"].toInt();
    }

    if (obj.contains("notes")) {
        info.notes = parseNotes(obj["notes"].toArray());
    }

    if (obj.contains("start") && obj.contains("end")) {
        info.start = parseLocation(obj["start"].toObject());
        info.end = parseLocation(obj["end"].toObject());
    } else {
        info.start = parseLocation(obj);
        info.end = info.start;
    }

    static const std::unordered_set<std::string> KEYS_TO_IGNORE {
        "type",
        "name",
        "duration",
        "notes",
        "start",
        "end",
    };

    for (const std::string& key : obj.keys()) {
        if (!muse::contains(KEYS_TO_IGNORE, key)) {
            info.data[String::fromStdString(key)] = muse::Val(obj[key].toStdString());
        }
    }

    return info;
}

static Fraction parseTimeSig(const String& str)
{
    bool ok = true;
    const size_t spaceIdx = str.indexOf(' ');
    const Fraction timeSig = Fraction::fromString(str.left(spaceIdx), &ok);

    if (ok) {
        return timeSig;
    }

    return TimeSigMap::DEFAULT_TIME_SIGNATURE;
}

static TDuration parseDuration(const ElementInfo::Duration& dur)
{
    const DurationType type = TConv::durationTypeByUserName(dur.name);
    return DurationTypeWithDots(type, (int)dur.dots);
}

static Segment* getOrAddSegment(SegmentType type, MasterScore* score, const ElementInfo::Location& loc)
{
    const Fraction tick = locToTick(score, loc);
    Measure* measure = score->tick2measure(tick);
    return measure ? measure->getSegment(type, tick) : nullptr;
}

static Segment* getOrAddChordRestSegment(MasterScore* score, const ElementInfo::Location& loc)
{
    return getOrAddSegment(SegmentType::ChordRest, score, loc);
}

static void addNotes(const ElementInfo::NoteList& notes, Chord* chord, ElementsContext& ctx)
{
    static const muse::String TIED_KEY(u"tied");

    for (const ElementInfo::Note& noteInfo : notes) {
        String noteName = noteInfo.name;
        noteName.replace(u"♭", u"b").replace(u"♯", u"#");

        size_t octaveIdx = 0;
        NoteCaseType caseType = NoteCaseType::AUTO;
        const int tpc = convertNote(noteName, NoteSpellingType::STANDARD, caseType, octaveIdx);

        bool ok = true;
        const String octaveStr = noteName.mid(octaveIdx);
        const int octave = octaveStr.toInt(&ok);
        IF_ASSERT_FAILED(ok) {
            continue;
        }

        const int pitch = (octave + 1) * 12 + tpc2pitch(tpc);

        Note* note = Factory::createNote(chord);
        note->setTpc(tpc);
        note->setPitch(pitch);
        note->setTrack(chord->track());
        chord->add(note);

        if (muse::contains(noteInfo.data, TIED_KEY)) {
            ctx.tiedToNextNotes.push_back(note);
        }
    }
}

static Chord* addChord(const ElementInfo& info, MasterScore* score, ElementsContext& ctx)
{
    const TDuration dur = parseDuration(info.duration);
    IF_ASSERT_FAILED(dur.isValid()) {
        return nullptr;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return nullptr;
    }

    Chord* chord = Factory::createChord(seg);
    chord->setDurationType(dur);
    chord->setTrack(locToTrack(info.start));

    if (dur.type() == DurationType::V_MEASURE) {
        chord->setTicks(seg->measure()->ticks());
    } else {
        chord->setTicks(dur.fraction());
    }

    seg->add(chord);

    if (!info.notes.empty()) {
        addNotes(info.notes, chord, ctx);
    } else {
        ElementInfo::Note note;
        note.name = info.name;
        note.data = info.data;
        addNotes({ note }, chord, ctx);
    }

    return chord;
}

static void addRest(const ElementInfo& info, MasterScore* score)
{
    const TDuration dur = parseDuration(info.duration);
    IF_ASSERT_FAILED(dur.isValid()) {
        return;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    Rest* rest = Factory::createRest(seg, dur);
    rest->setTrack(locToTrack(info.start));

    if (dur.type() == DurationType::V_MEASURE) {
        rest->setTicks(seg->measure()->ticks());
    } else {
        rest->setTicks(dur.fraction());
    }

    seg->add(rest);
}

static void addArpeggio(const ElementInfo& info, MasterScore* score, ElementsContext& ctx)
{
    Chord* chord = addChord(info, score, ctx);
    IF_ASSERT_FAILED(chord) {
        return;
    }

    Arpeggio* arpeggio = Factory::createArpeggio(chord);
    arpeggio->setArpeggioType(TConv::arpeggioTypeByUserName(info.name));
    arpeggio->setTrack(locToTrack(info.start));
    chord->add(arpeggio);
}

static void addLyrics(const ElementInfo& info, MasterScore* score)
{
    const String text = String::fromStdString(muse::value(info.data, u"text").toString());
    IF_ASSERT_FAILED(!text.empty()) {
        return;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    const track_idx_t trackIdx = locToTrack(info.start);
    EngravingItem* item = seg->element(trackIdx);
    IF_ASSERT_FAILED(item && item->isChordRest()) {
        return;
    }

    ChordRest* cr = toChordRest(item);
    Lyrics* lyrics = Factory::createLyrics(cr);
    lyrics->setPlainText(text);
    lyrics->setTrack(trackIdx);
    cr->add(lyrics);
}

static void addTempo(const ElementInfo& info, MasterScore* score)
{
    const auto bpmIt = info.data.find(u"bpm");
    IF_ASSERT_FAILED(bpmIt != info.data.end()) {
        return;
    }

    const TDuration dur = parseDuration(info.duration);
    IF_ASSERT_FAILED(dur.isValid()) {
        return;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    String tempo = TempoText::duration2tempoTextString(dur);
    tempo = tempo.append(u" = %1").arg(bpmIt->second.toDouble());

    TempoText* item = Factory::createTempoText(seg);
    item->setTempo(bpmIt->second.toDouble());
    item->setXmlText(tempo);
    item->setTrack(locToTrack(info.start));
    seg->add(item);
}

static void addSpanner(Spanner* spanner, const ElementInfo& info, MasterScore* score)
{
    spanner->setTrack(locToTrack(info.start));
    spanner->setTrack2(locToTrack(info.end));
    spanner->setTick(locToTick(score, info.start));
    spanner->setTick2(locToTick(score, info.end));
    score->addSpanner(spanner);
}

static void addHairpin(const ElementInfo& info, MasterScore* score)
{
    const HairpinType type = TConv::hairpinTypeByUserName(info.name);
    IF_ASSERT_FAILED(type != HairpinType::INVALID) {
        return;
    }

    Hairpin* hairpin = Factory::createHairpin(score->dummy()->segment());
    hairpin->setHairpinType(type);
    addSpanner(hairpin, info, score);
}

static void addPedal(const ElementInfo& info, MasterScore* score)
{
    Pedal* pedal = Factory::createPedal(score->dummy());
    addSpanner(pedal, info, score);
}

static void addDynamic(const ElementInfo& info, MasterScore* score)
{
    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    Dynamic* dynamic = Factory::createDynamic(seg);
    dynamic->setDynamicType(info.name);
    dynamic->setTrack(locToTrack(info.start));
    seg->add(dynamic);
}

static void addChordSymbol(const ElementInfo& info, MasterScore* score)
{
    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    Harmony* chordSymbol = Factory::createHarmony(seg);
    chordSymbol->setHarmony(info.name);
    chordSymbol->setTrack(locToTrack(info.start));
    seg->add(chordSymbol);
}

static void addPlayTechAnnotation(const ElementInfo& info, MasterScore* score)
{
    const PlayingTechniqueType type = TConv::playingTechniqueTypeByUserName(info.name);
    IF_ASSERT_FAILED(type != PlayingTechniqueType::Undefined) {
        return;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    PlayTechAnnotation* technique = Factory::createPlayTechAnnotation(seg, type, TextStyleType::STAFF);
    technique->setXmlText(info.name);
    technique->setTrack(locToTrack(info.start));
    seg->add(technique);
}

static void addStaffText(const ElementInfo& info, MasterScore* score)
{
    const String text = String::fromStdString(muse::value(info.data, u"text").toString());
    IF_ASSERT_FAILED(!text.empty()) {
        return;
    }

    Segment* seg = getOrAddChordRestSegment(score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    StaffText* staffText = Factory::createStaffText(seg);
    staffText->setXmlText(text);
    seg->add(staffText);
}

static void addKeySig(const ElementInfo& info, MasterScore* score)
{
    const Key key = TConv::keyByUserName(info.name);
    IF_ASSERT_FAILED(key != Key::INVALID) {
        return;
    }

    Segment* seg = getOrAddSegment(SegmentType::KeySig, score, info.start);
    IF_ASSERT_FAILED(seg) {
        return;
    }

    KeySig* ks = Factory::createKeySig(seg);
    ks->setKey(key);
    ks->setTrack(locToTrack(info.start));
    seg->add(ks);
}

static void addBarLine(const ElementInfo& info, MasterScore* score)
{
    const BarLineType type = BarLine::barlineTypeByUserName(info.name);
    if (type == BarLineType::NORMAL
        || type == BarLineType::SINGLE
        || type == BarLineType::END
        || type == BarLineType::FINAL) {
        return;
    }

    Measure* measure = score->crMeasure(info.start.measureIdx);
    IF_ASSERT_FAILED(measure) {
        return;
    }

    Segment* seg = nullptr;
    if (type == BarLineType::START_REPEAT) {
        seg = measure->getSegment(SegmentType::BeginBarLine, measure->tick());
        measure->setRepeatStart(true);
    } else {
        seg = measure->getSegment(SegmentType::EndBarLine, measure->endTick());

        if (type == BarLineType::END_REPEAT) {
            measure->setRepeatEnd(true);
        }
    }

    IF_ASSERT_FAILED(seg) {
        return;
    }

    BarLine* barline = Factory::createBarLine(seg);
    barline->setBarLineType(type);
    barline->setTrack(locToTrack(info.start));
    seg->add(barline);
}

static void addElement(const ElementInfo& info, MasterScore* score, ElementsContext& ctx)
{
    TRACEFUNC;

    switch (info.type) {
    case ElementType::NOTE:
    case ElementType::CHORD:
        addChord(info, score, ctx);
        break;
    case ElementType::ARPEGGIO:
        addArpeggio(info, score, ctx);
        break;
    case ElementType::REST:
        addRest(info, score);
        break;
    case ElementType::LYRICS:
        addLyrics(info, score);
        break;
    case ElementType::HAIRPIN:
        addHairpin(info, score);
        break;
    case ElementType::PEDAL:
        addPedal(info, score);
        break;
    case ElementType::TEMPO_TEXT:
        addTempo(info, score);
        break;
    case ElementType::DYNAMIC:
        addDynamic(info, score);
        break;
    case ElementType::HARMONY:
        addChordSymbol(info, score);
        break;
    case ElementType::PLAYTECH_ANNOTATION:
        addPlayTechAnnotation(info, score);
        break;
    case ElementType::STAFF_TEXT:
        addStaffText(info, score);
        break;
    case ElementType::KEYSIG:
        addKeySig(info, score);
        break;
    case ElementType::BAR_LINE:
        addBarLine(info, score);
        break;
    default:
        LOGW() << "Not supported: " << info.name;
        break;
    }
}

static Note* findEndTiedNote(const Note* note)
{
    const Segment* noteSeg = note->chord()->segment();

    for (const Segment* seg = noteSeg->next1enabled(); seg; seg = seg->next1enabled()) {
        if (!seg->isChordRestType()) {
            continue;
        }

        EngravingItem* el = seg->element(note->track());
        if (el || el->isChord()) {
            return toChord(el)->findNote(note->pitch());
        }
    }

    return nullptr;
}

static void connectTiedNotes(const std::vector<Note*>& tiedToNextNotes, MasterScore* score)
{
    for (Note* startNote : tiedToNextNotes) {
        Note* endNote = findEndTiedNote(startNote);
        IF_ASSERT_FAILED(endNote) {
            continue;
        }

        Tie* tie = Factory::createTie(score->dummy());
        tie->setStartNote(startNote);
        tie->setEndNote(endNote);
        tie->setTick(startNote->tick());
        tie->setTick2(endNote->tick());
        tie->setTrack(startNote->track());
        startNote->setTieFor(tie);
        endNote->setTieBack(tie);
        score->undoAddElement(tie);
    }
}

static void addElements(const ElementMap& elements, MasterScore* score)
{
    TRACEFUNC;

    ElementsContext ctx;

    for (const auto& pair : elements) {
        for (const ElementInfo& info : pair.second) {
            addElement(info, score, ctx);
        }
    }

    connectTiedNotes(ctx.tiedToNextNotes, score);
}

static void addTimesig(const Fraction& timeSig, MasterScore* score, Measure* measure, const Fraction& pos)
{
    TRACEFUNC;

    for (size_t staffIdx = 0; staffIdx < score->nstaves(); ++staffIdx) {
        const Staff* staff = score->staff(staffIdx);
        const StaffType* staffType = staff->staffType();
        if (staffType->genTimesig()) {
            Segment* seg = measure->getSegment(SegmentType::TimeSig, pos);
            TimeSig* ts = Factory::createTimeSig(seg);
            ts->setTrack(staffIdx * VOICES);
            ts->setSig(timeSig);
            seg->add(ts);
        }
    }
}

static void addMeasures(size_t measures, MasterScore* score)
{
    TRACEFUNC;

    mu::engraving::Fraction prevTimeSig;

    for (size_t i = 0; i < measures; ++i) {
        const engraving::Fraction tick = Fraction::fromTicks(score->sigmap()->bar2tick(i, 0.f));
        const engraving::Fraction timeSig = score->sigmap()->timesig(tick.ticks()).timesig();

        Measure* measure = Factory::createMeasure(score->dummy()->system());
        measure->setTick(tick);
        measure->setTimesig(timeSig);
        measure->setTicks(timeSig);

        if (!timeSig.identical(prevTimeSig)) {
            addTimesig(timeSig, score, measure, tick);
        }

        score->measures()->append(measure);
        prevTimeSig = timeSig;
    }
}

static void fillMeasureGaps(MasterScore* score)
{
    TRACEFUNC;

    for (Measure* m = score->firstMeasure(); m; m = m->nextMeasure()) {
        for (staff_idx_t staffIdx = 0; staffIdx < score->nstaves(); ++staffIdx) {
            m->checkMeasure(staffIdx);
        }
    }
}

muse::Ret ScoreElementsReader::read(MasterScore* score, const muse::io::path_t& path, const Options&)
{
    TRACEFUNC;

    const muse::RetVal<ScoreInfo> info = readScoreInfo(path);
    if (!info.ret) {
        return info.ret;
    }

    *(score->sigmap()) = info.val.timeSigMap;

    addParts(info.val, score);
    addMeasures(info.val.measures, score);
    addElements(info.val.chordRestElements, score);
    addElements(info.val.otherElements, score);
    fillMeasureGaps(score);

    score->setUpTempoMap();

    return muse::make_ok();
}

muse::RetVal<ScoreElementsReader::ScoreInfo> ScoreElementsReader::readScoreInfo(const muse::io::path_t& path)
{
    TRACEFUNC;

    muse::RetVal<muse::ByteArray> json = fileSystem()->readFile(path);
    if (!json.ret) {
        return json.ret;
    }

    std::string err;
    muse::JsonDocument doc = muse::JsonDocument::fromJson(json.val, &err);
    if (!err.empty()) {
        return muse::make_ret(muse::Ret::Code::UnknownError, err);
    }

    if (!doc.isArray()) {
        return muse::make_ret(muse::Ret::Code::UnknownError, std::string("Expected a JSON array at the root"));
    }

    muse::RetVal<ScoreInfo> result;
    result.ret = muse::make_ok();

    const muse::JsonArray rootArray = doc.rootArray();
    size_t maxMeasureIdx = 0;
    TimeSigMap& timeSigMap = result.val.timeSigMap;

    const ElementTypeSet CHORD_REST_TYPES {
        ElementType::NOTE,
        ElementType::CHORD,
        ElementType::REST,
        ElementType::ARPEGGIO, // creates a chord
    };

    for (size_t i = 0; i < rootArray.size(); ++i) {
        const muse::JsonValue& instrumentVal = rootArray.at(i);
        const muse::JsonObject instrumentObj = instrumentVal.toObject();
        const muse::JsonArray elementsArray = instrumentObj["elements"].toArray();

        ElementInfoList chordRestElements;
        ElementInfoList otherElements;
        size_t maxStaffIdx = 0;

        for (size_t j = 0; j < elementsArray.size(); ++j) {
            const muse::JsonValue& elementVal = elementsArray.at(j);
            const muse::JsonObject elementObj = elementVal.toObject();

            ElementInfo info = parseElementInfo(elementObj);
            if (info.type == ElementType::INVALID) {
                continue;
            }

            maxMeasureIdx = std::max(maxMeasureIdx, info.end.measureIdx);
            maxStaffIdx = std::max(maxStaffIdx, info.end.staffIdx);

            if (info.type == ElementType::TIMESIG) {
                const engraving::Fraction timeSig = parseTimeSig(info.name);

                if (timeSigMap.empty()) {
                    timeSigMap.add(0, timeSig);
                } else {
                    const int tick = timeSigMap.bar2tick((int)info.start.measureIdx, info.start.beat);
                    timeSigMap.add(tick, timeSig);
                }

                continue;
            } else if (muse::contains(CHORD_REST_TYPES, info.type)) {
                chordRestElements.emplace_back(std::move(info));
            } else {
                otherElements.emplace_back(std::move(info));
            }
        }

        if (chordRestElements.empty() && otherElements.empty()) {
            continue;
        }

        const InstrumentTrackId trackId {
            ID(instrumentObj["partId"].toInt()),
            instrumentObj["instrumentId"].toString(),
        };

        result.val.chordRestElements[trackId] = std::move(chordRestElements);
        result.val.otherElements[trackId] = std::move(otherElements);
        result.val.partStaves[trackId.partId] = maxStaffIdx + 1;
    }

    if (!result.val.chordRestElements.empty()) {
        result.val.measures = maxMeasureIdx + 1;
    }

    return result;
}

void ScoreElementsReader::addParts(const ScoreInfo& info, MasterScore* score)
{
    TRACEFUNC;

    std::map<ID, Part*> parts;

    // Step 1: Create parts & instruments
    for (const auto& pair : info.chordRestElements) {
        const InstrumentTrackId& trackId = pair.first;
        const InstrumentTemplate* templ = searchTemplateIndexForId(trackId.instrumentId).instrTemplate;

        Part* part = muse::value(parts, trackId.partId, nullptr);
        if (!part) {
            part = new Part(score);
            part->setId(trackId.partId);
            part->setInstrument(Instrument::fromTemplate(templ));
            parts[part->id()] = part;
        } else {
            NOT_IMPLEMENTED;
        }
    }

    // Step 2: Create staves & append parts
    for (const auto& pair : parts) {
        const size_t staves = muse::value(info.partStaves, pair.first, 0);
        for (size_t i = 0; i < staves; ++i) {
            Staff* staff = Factory::createStaff(pair.second);
            score->appendStaff(staff);
        }

        score->appendPart(pair.second);
    }
}
