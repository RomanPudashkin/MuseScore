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
//  MERCHANTABILITY or FIT-0NESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================
#include "notationparts.h"

#include "libmscore/score.h"
#include "libmscore/undo.h"
#include "libmscore/excerpt.h"
#include "libmscore/drumset.h"
#include "libmscore/instrchange.h"

#include "log.h"

using namespace mu::async;
using namespace mu::notation;
using namespace mu::instruments;

NotationParts::NotationParts(IGetScore* getScore, Notification selectionChangedNotification)
    : m_getScore(getScore), m_partsNotifier(new ChangedNotifier<const Part*>())
{
    selectionChangedNotification.onNotify(this, [this]() {
        m_canChangeInstrumentsVisibilityChanged.notify();
    });
}

Ms::Score* NotationParts::score() const
{
    return m_getScore->score();
}

Ms::MasterScore* NotationParts::masterScore() const
{
    return score()->masterScore();
}

void NotationParts::startEdit()
{
    masterScore()->startCmd();
}

void NotationParts::apply()
{
    masterScore()->endCmd();
}

NotifyList<const Part*> NotationParts::partList() const
{
    NotifyList<const Part*> result;

    std::vector<Part*> parts = availableParts();

    QSet<QString> partIds;
    for (const Part* part: parts) {
        if (partIds.contains(part->id())) {
            continue;
        }

        result.push_back(part);
        partIds.insert(part->id());
    }

    result.setNotify(m_partsNotifier->notify());
    return result;
}

NotifyList<Instrument> NotationParts::instrumentList(const QString& partId) const
{
    Part* part = this->part(partId);
    if (!part) {
        return NotifyList<Instrument>();
    }

    NotifyList<Instrument> result;

    auto instrumentList = part->instruments();
    for (auto it = instrumentList->begin(); it != instrumentList->end(); it++) {
        result.push_back(convertedInstrument(it->second, part));
    }

    ChangedNotifier<Instrument>* notifier = partNotifier(partId);
    result.setNotify(notifier->notify());
    return result;
}

NotifyList<const Staff*> NotationParts::staffList(const QString& partId, const QString& instrumentId) const
{
    Part* part = this->part(partId);
    if (!part) {
        return NotifyList<const Staff*>();
    }

    NotifyList<const Staff*> result;
    std::vector<const Staff*> staves = this->staves(part, instrumentId);
    for (const Staff* staff: staves) {
        result.push_back(staff);
    }

    ChangedNotifier<const Staff*>* notifier = instrumentNotifier(instrumentId, partId);
    result.setNotify(notifier->notify());
    return result;
}

void NotationParts::setInstruments(const InstrumentList& instruments)
{
    std::vector<QString> instrumentIds;
    for (const Instrument& instrument: instruments) {
        instrumentIds.push_back(instrument.id);
    }

    startEdit();
    removeUnselectedInstruments(instrumentIds);

    std::vector<QString> missingInstrumentIds = this->missingInstrumentIds(instrumentIds);

    int lastGlobalStaffIndex = !score()->staves().empty() ? score()->staves().last()->idx() : 0;
    for (const Instrument& instrument: instruments) {
        bool instrumentIsExists = std::find(missingInstrumentIds.begin(), missingInstrumentIds.end(),
                                            instrument.id) == missingInstrumentIds.end();
        if (instrumentIsExists) {
            continue;
        }

        Part* part = new Part(score());

        part->setPartName(instrument.trackName);
        part->setInstrument(convertedInstrument(instrument));

        score()->undo(new Ms::InsertPart(part, lastGlobalStaffIndex));
        addStaves(part, instrument, lastGlobalStaffIndex);
    }

    if (score()->measures()->empty()) {
        score()->insertMeasure(ElementType::MEASURE, 0, false);
    }

    sortParts(instrumentIds);

    removeEmptyExcerpts();

    apply();

    m_partsNotifier->changed();
    m_partsChanged.notify();
}

void NotationParts::setPartVisible(const QString& partId, bool visible)
{
    Part* part = this->part(partId);
    if (!part) {
        part = this->part(partId, masterScore());
        if (!part) {
            return;
        }

        appendPart(part);
        m_partsChanged.notify();
        return;
    }

    startEdit();
    part->undoChangeProperty(Ms::Pid::VISIBLE, visible);
    apply();

    m_partsNotifier->itemChanged(part);
    m_partsChanged.notify();
}

void NotationParts::setPartName(const QString& partId, const QString& name)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    startEdit();
    doSetPartName(part, name);
    apply();

    m_partsNotifier->itemChanged(part);
    m_partsChanged.notify();
}

void NotationParts::setInstrumentVisible(const QString& partId, const QString& instrumentId, bool visible)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    InstrumentInfo instrumentInfo = this->instrumentInfo(part, instrumentId);
    if (!instrumentInfo.isValid()) {
        return;
    }

    if (isDoublingInstrument(instrumentInfo.tick) && !isInstrumentAssignedToChord(partId, instrumentId)) {
        assignIstrumentToSelectedChord(instrumentInfo.instrument);
        return;
    }

    startEdit();

    std::vector<const Staff*> instrumentStaves = staves(part, instrumentId);
    for (const Staff* staff: instrumentStaves) {
        Staff* _staff = score()->staff(staff->idx());
        doSetStaffVisible(_staff, visible);
    }

    apply();

    ChangedNotifier<Instrument>* notifier = partNotifier(part->id());
    notifier->itemChanged(convertedInstrument(instrumentInfo.instrument, part));
    m_partsChanged.notify();
}

Ms::ChordRest* NotationParts::selectedChord() const
{
    Ms::ChordRest* chord = score()->getSelectedChordRest();

    if (Ms::MScore::_error == Ms::MsError::NO_NOTE_REST_SELECTED) {
        Ms::MScore::_error = Ms::MsError::MS_NO_ERROR;
    }

    return chord;
}

bool NotationParts::isDoublingInstrument(int ticks) const
{
    return Ms::Fraction(-1, 1).ticks() != ticks;
}

bool NotationParts::isInstrumentAssignedToChord(const QString& partId, const QString& instrumentId) const
{
    Ms::SegmentType segmentType = Ms::SegmentType::ChordRest;
    for (const Ms::Segment* segment = score()->firstSegment(segmentType); segment; segment = segment->next1(segmentType)) {
        for (Ms::Element* element: segment->elist()) {
            auto instrumentChange = dynamic_cast<Ms::InstrumentChange*>(element);
            if (!instrumentChange) {
                continue;
            }

            if (instrumentChange->part()->id() == partId && instrumentChange->instrument()->instrumentId() == instrumentId) {
                return true;
            }
        }
    }

    return false;
}

void NotationParts::assignIstrumentToSelectedChord(Ms::Instrument* instrument)
{
    Ms::ChordRest* chord = selectedChord();
    if (!chord) {
        return;
    }

    startEdit();
    Part* part = chord->part();
    part->removeInstrument(instrument->instrumentId());
    part->setInstrument(instrument, chord->tick());

    auto instrumentChange = new Ms::InstrumentChange(*instrument, score());
    instrumentChange->setInit(true);
    instrumentChange->setParent(chord->segment());
    instrumentChange->setTrack((chord->track() / VOICES) * VOICES);
    instrumentChange->setupInstrument(instrument);

    score()->undoAddElement(instrumentChange);
    apply();

    ChangedNotifier<Instrument>* notifier = partNotifier(part->id());
    notifier->itemChanged(convertedInstrument(instrument, part));
    m_partsChanged.notify();
}

void NotationParts::doMovePart(const QString& partId, const QString& toPartId, INotationParts::InsertMode mode)
{
    Part* part = this->part(partId);
    Part* toPart = this->part(toPartId);
    if (!part || !toPart) {
        return;
    }

    bool partIsBefore = score()->staffIdx(part) < score()->staffIdx(toPart);
    QList<Staff*> staves = *part->staves();
    int newStaffIndex = partIsBefore ? staves.size() : 0;

    score()->undoRemovePart(part);

    int toPartIndex = score()->parts().indexOf(toPart);
    int newPartIndex = mode == Before ? toPartIndex : toPartIndex + 1;
    score()->parts().insert(newPartIndex, part);

    for (Staff* staff: staves) {
        Staff* movedStaff = staff->clone();
        score()->undoInsertStaff(movedStaff, newStaffIndex);
        Ms::Excerpt::cloneStaff(staff, movedStaff);
        newStaffIndex++;
    }

    for (Staff* staff: staves) {
        score()->undoRemoveStaff(staff);
    }
}

void NotationParts::setInstrumentName(const QString& partId, const QString& instrumentId, const QString& name)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    InstrumentInfo instrumentInfo = this->instrumentInfo(part, instrumentId);
    if (!instrumentInfo.isValid()) {
        return;
    }

    startEdit();
    score()->undo(new Ms::ChangeInstrumentLong(Ms::Fraction::fromTicks(instrumentInfo.tick), part, { StaffName(name, 0) }));
    apply();

    InstrumentInfo newInstrumentInfo = this->instrumentInfo(part, instrumentId);
    ChangedNotifier<Instrument>* notifier = partNotifier(part->id());
    notifier->itemChanged(convertedInstrument(newInstrumentInfo.instrument, part));
    m_partsChanged.notify();
}

void NotationParts::setInstrumentAbbreviature(const QString& partId, const QString& instrumentId, const QString& abbreviature)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    InstrumentInfo instrumentInfo = this->instrumentInfo(part, instrumentId);
    if (!instrumentInfo.isValid()) {
        return;
    }

    startEdit();
    score()->undo(new Ms::ChangeInstrumentShort(Ms::Fraction::fromTicks(instrumentInfo.tick), part, { StaffName(abbreviature, 0) }));
    apply();

    InstrumentInfo newInstrumentInfo = this->instrumentInfo(part, instrumentId);
    ChangedNotifier<Instrument>* notifier = partNotifier(part->id());
    notifier->itemChanged(convertedInstrument(newInstrumentInfo.instrument, part));
    m_partsChanged.notify();
}

void NotationParts::setStaffVisible(int staffIndex, bool visible)
{
    Staff* staff = this->staff(staffIndex);
    if (!staff) {
        return;
    }

    startEdit();
    doSetStaffVisible(staff, visible);
    apply();

    notifyAboutStaffChanged(staffIndex);
    m_partsChanged.notify();
}

void NotationParts::doSetStaffVisible(Staff* staff, bool visible)
{
    if (!staff) {
        return;
    }

    staff->setInvisible(!visible);
    score()->undo(new Ms::ChangeStaff(staff));
}

void NotationParts::setStaffType(int staffIndex, StaffType type)
{
    Staff* staff = this->staff(staffIndex);
    const Ms::StaffType* staffType = Ms::StaffType::preset(type);

    if (!staff || !staffType) {
        return;
    }

    startEdit();
    score()->undo(new Ms::ChangeStaffType(staff, *staffType));
    apply();

    notifyAboutStaffChanged(staffIndex);
    m_partsChanged.notify();
}

void NotationParts::setCutaway(int staffIndex, bool value)
{
    Staff* staff = this->staff(staffIndex);
    if (!staff) {
        return;
    }

    startEdit();
    staff->setCutaway(value);
    score()->undo(new Ms::ChangeStaff(staff));
    apply();

    notifyAboutStaffChanged(staffIndex);
    m_partsChanged.notify();
}

void NotationParts::setSmallStaff(int staffIndex, bool value)
{
    Staff* staff = this->staff(staffIndex);
    Ms::StaffType* staffType = staff->staffType(Ms::Fraction(0, 1));

    if (!staff || !staffType) {
        return;
    }

    startEdit();
    staffType->setSmall(value);
    score()->undo(new Ms::ChangeStaffType(staff, *staffType));
    apply();

    notifyAboutStaffChanged(staffIndex);
    m_partsChanged.notify();
}

void NotationParts::setVoiceVisible(int staffIndex, int voiceIndex, bool visible)
{
    Staff* staff = this->staff(staffIndex);
    if (!staff) {
        return;
    }

    startEdit();

    Ms::SegmentType segmentType = Ms::SegmentType::ChordRest;
    for (const Ms::Segment* segment = score()->firstSegment(segmentType); segment; segment = segment->next1(segmentType)) {
        for (Ms::Element* element: segment->elist()) {
            if (!element) {
                continue;
            }

            if (element->staffIdx() == staffIndex && element->voice() == voiceIndex) {
                element->undoChangeProperty(Ms::Pid::VISIBLE, visible);
            }
        }
    }

    staff->setVoiceVisible(voiceIndex, visible);

    apply();

    notifyAboutStaffChanged(staffIndex);
    m_partsChanged.notify();
}

void NotationParts::appendInstrument(const QString& partId, const Instrument& instrument)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    int lastTick = 1;
    for (auto it = part->instruments()->cbegin(); it != part->instruments()->cend(); ++it) {
        lastTick = std::max(it->first, lastTick);
    }

    startEdit();
    part->setInstrument(convertedInstrument(instrument), Ms::Fraction::fromTicks(lastTick + 1));
    doSetPartName(part, calculatedPartName(part));
    apply();

    ChangedNotifier<Instrument>* notifier = partNotifier(partId);
    notifier->itemAdded(instrument);
    m_partsNotifier->itemChanged(part);
    m_partsChanged.notify();
}

void NotationParts::appendStaff(const QString& partId, const QString& instrumentId)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    InstrumentInfo instrumentInfo = this->instrumentInfo(part, instrumentId);
    if (!instrumentInfo.isValid()) {
        return;
    }

    Ms::Instrument* instrument = instrumentInfo.instrument;
    std::vector<const Staff*> instrumentStaves = staves(part, instrumentId);

    startEdit();
    Staff* staff = instrumentStaves.front()->clone();
    staff->setId(Staff::makeId());
    int staffIndex = staff->part()->nstaves();
    score()->undoInsertStaff(staff, staffIndex);
    instrument->setClefType(staffIndex, staff->defaultClefType());
    apply();

    ChangedNotifier<const Staff*>* notifier = instrumentNotifier(instrument->instrumentId(), partId);
    notifier->itemAdded(staff);
    m_partsChanged.notify();
}

void NotationParts::appendLinkedStaff(int originStaffIndex)
{
    Staff* staff = this->staff(originStaffIndex);
    if (!staff || !staff->part()) {
        return;
    }

    Staff* linkedStaff = staff->clone();
    linkedStaff->setId(Staff::makeId());
    int linkedStaffIndex = staff->part()->nstaves();
    staff->linkTo(linkedStaff);

    startEdit();
    score()->undoInsertStaff(linkedStaff, linkedStaffIndex);
    apply();

    InstrumentInfo instrumentInfo = this->instrumentInfo(linkedStaff);
    ChangedNotifier<const Staff*>* notifier = instrumentNotifier(instrumentInfo.instrument->instrumentId(), linkedStaff->part()->id());
    notifier->itemAdded(linkedStaff);
    m_partsChanged.notify();
}

void NotationParts::replaceInstrument(const QString& partId, const QString& instrumentId, const Instrument& newInstrument)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    InstrumentInfo oldInstrumentInfo = this->instrumentInfo(part, instrumentId);
    if (!oldInstrumentInfo.isValid()) {
        return;
    }

    startEdit();
    part->setInstrument(convertedInstrument(newInstrument), Ms::Fraction::fromTicks(oldInstrumentInfo.tick));
    doSetPartName(part, calculatedPartName(part));
    apply();

    ChangedNotifier<Instrument>* notifier = partNotifier(part->id());
    notifier->itemReplaced(convertedInstrument(oldInstrumentInfo.instrument, part), newInstrument);

    m_partsNotifier->itemChanged(part);
    m_partsChanged.notify();
}

Notification NotationParts::partsChanged() const
{
    return m_partsChanged;
}

void NotationParts::removeParts(const std::vector<QString>& partsIds)
{
    if (partsIds.empty()) {
        return;
    }

    startEdit();
    doRemoveParts(partsIds);
    apply();

    m_partsChanged.notify();
}

void NotationParts::doRemoveParts(const std::vector<QString>& partsIds)
{
    for (const QString& partId: partsIds) {
        score()->cmdRemovePart(part(partId));
    }
}

void NotationParts::removeInstruments(const QString& partId, const std::vector<QString>& instrumentIds)
{
    Part* part = this->part(partId);
    if (!part) {
        return;
    }

    startEdit();
    doRemoveInstruments(part, instrumentIds);
    apply();

    m_partsNotifier->itemChanged(part);
    m_partsChanged.notify();
}

void NotationParts::doRemoveInstruments(Part* part, const std::vector<QString>& instrumentIds)
{
    for (const QString& instrumentId: instrumentIds) {
        InstrumentInfo instrumentInfo = this->instrumentInfo(part, instrumentId);
        if (!instrumentInfo.isValid()) {
            continue;
        }

        part->removeInstrument(instrumentId);
    }
}

void NotationParts::removeStaves(const std::vector<int>& stavesIndexes)
{
    if (stavesIndexes.empty()) {
        return;
    }

    startEdit();
    doRemoveStaves(stavesIndexes);
    apply();

    m_partsChanged.notify();
}

void NotationParts::doRemoveStaves(const std::vector<int>& stavesIndexes)
{
    for (Staff* staff: staves(stavesIndexes)) {
        score()->cmdRemoveStaff(staff->idx());
    }
}

void NotationParts::doSetPartName(Part* part, const QString& name)
{
    score()->undo(new Ms::ChangePart(part, new Ms::Instrument(*part->instrument()), name));
}

void NotationParts::moveParts(const std::vector<QString>& partIds, const QString& toPartId, InsertMode mode)
{
    startEdit();

    for (const QString& partId: partIds) {
        doMovePart(partId, toPartId, mode);
    }

    apply();

    m_partsChanged.notify();
}

void NotationParts::moveInstruments(const std::vector<QString>& instrumentIds, const QString& fromPartId, const QString& toPartId,
                                    const QString& toInstrumentId, InsertMode mode)
{
    Part* fromPart = part(fromPartId);
    Part* toPart = part(toPartId);

    if (!fromPart || !toPart) {
        return;
    }

    startEdit();
    for (const QString& instrumentId: instrumentIds) {
        Ms::Instrument* instrument = this->instrumentInfo(fromPart, instrumentId).instrument;
        Ms::Instrument* newInstrument = new Ms::Instrument(*instrument);
        std::vector<const Staff*> staves = staffList(fromPartId, instrument->instrumentId());

        doRemoveInstruments(fromPart, { instrument->instrumentId() });
        insertInstrument(toPart, newInstrument, staves, toInstrumentId, mode);
    }
    apply();

    m_partsNotifier->itemChanged(fromPart);
    m_partsNotifier->itemChanged(toPart);
    m_partsChanged.notify();
}

void NotationParts::moveStaves(const std::vector<int>& stavesIndexes, int toStaffIndex, InsertMode mode)
{
    if (stavesIndexes.empty()) {
        return;
    }

    std::vector<Staff*> staves = this->staves(stavesIndexes);
    Part* toPart = score()->staff(toStaffIndex)->part();
    int newStaffIndex = (mode == Before ? toStaffIndex : toStaffIndex + 1);
    newStaffIndex -= score()->staffIdx(toPart); // NOTE: convert to local part's staff index

    startEdit();
    for (Staff* staff: staves) {
        Staff* movedStaff = staff->clone();
        score()->undoInsertStaff(movedStaff, newStaffIndex);
        Ms::Excerpt::cloneStaff(staff, movedStaff);
        newStaffIndex++;
    }

    for (Staff* staff: staves) {
        score()->undoRemoveStaff(staff);
    }
    apply();

    for (const Staff* staff: *toPart->staves()) {
        notifyAboutStaffChanged(staff->idx());
    }

    m_partsChanged.notify();
}

Notification NotationParts::canChangeInstrumentsVisibilityChanged() const
{
    return m_canChangeInstrumentsVisibilityChanged;
}

NotationParts::~NotationParts()
{
    delete m_partsNotifier;
}

bool NotationParts::canChangeInstrumentVisibility(const QString& partId, const QString& instrumentId) const
{
    InstrumentInfo info = instrumentInfo(part(partId), instrumentId);

    if (!info.isValid()) {
        return false;
    }

    if (!isDoublingInstrument(info.tick)) {
        return true;
    }

    const Ms::ChordRest* chord = selectedChord();
    return chord && chord->part()->id() == partId;
}

std::vector<Part*> NotationParts::availableParts() const
{
    std::vector<Part*> parts;

    std::vector<Part*> scoreParts = this->scoreParts(score());
    parts.insert(parts.end(), scoreParts.begin(), scoreParts.end());

    std::vector<Part*> excerptParts = this->excerptParts(score());
    parts.insert(parts.end(), excerptParts.begin(), excerptParts.end());

    return parts;
}

std::vector<Part*> NotationParts::scoreParts(const Ms::Score* score) const
{
    std::vector<Part*> result;

    for (Part* part: score->parts()) {
        result.push_back(part);
    }

    return result;
}

std::vector<Part*> NotationParts::excerptParts(const Ms::Score* score) const
{
    if (!score->isMaster()) {
        return std::vector<Part*>();
    }

    std::vector<Part*> result;

    for (const Ms::Excerpt* excerpt: score->excerpts()) {
        for (Part* part: excerpt->parts()) {
            result.push_back(part);
        }
    }

    return result;
}

Part* NotationParts::part(const QString& partId, const Ms::Score* score) const
{
    if (!score) {
        score = this->score();
    }

    std::vector<Part*> parts = availableParts();

    for (Part* part: parts) {
        if (part->id() == partId) {
            return part;
        }
    }

    LOGW() << QString("Part %1 not found").arg(partId);
    return nullptr;
}

NotationParts::InstrumentInfo NotationParts::instrumentInfo(const Part* part, const QString& instrumentId) const
{
    if (!part) {
        return InstrumentInfo();
    }

    auto instrumentList = part->instruments();
    if (!instrumentList) {
        return InstrumentInfo();
    }

    for (auto it = instrumentList->begin(); it != instrumentList->end(); it++) {
        if (it->second->instrumentId() == instrumentId) {
            return InstrumentInfo(it->first, it->second);
        }
    }

    return InstrumentInfo();
}

NotationParts::InstrumentInfo NotationParts::instrumentInfo(const Staff* staff) const
{
    if (!staff || !staff->part()) {
        return InstrumentInfo();
    }

    return InstrumentInfo(Ms::Fraction(-1, 1).ticks(), staff->part()->instrument());
}

Staff* NotationParts::staff(int staffIndex) const
{
    Staff* staff = score()->staff(staffIndex);

    if (!staff) {
        LOGW() << "Could not find staff with index:" << staffIndex;
    }

    return staff;
}

std::vector<const Staff*> NotationParts::staves(const Part* part, const QString& instrumentId) const
{
    // TODO: configure staves by instrumentId
    Q_UNUSED(instrumentId)

    std::vector<const Staff*> result;

    for (const Staff* staff: *part->staves()) {
        result.push_back(staff);
    }

    return result;
}

std::vector<Staff*> NotationParts::staves(const std::vector<int>& stavesIndexes) const
{
    std::vector<Staff*> staves;

    for (int index: stavesIndexes) {
        Staff* staff = this->staff(index);

        if (staff) {
            staves.push_back(staff);
        }
    }

    return staves;
}

void NotationParts::appendPart(Part* part)
{
    for (Staff* partStaff: *part->staves()) {
        Staff* staff = new Staff(score());
        staff->setPart(part);
        staff->init(partStaff);
        if (partStaff->links() && !part->staves()->isEmpty()) {
            Staff* linkedStaff = part->staves()->back();
            staff->linkTo(linkedStaff);
        }
        part->insertStaff(staff, -1);
        score()->staves().append(staff);
    }

    score()->appendPart(part);
}

void NotationParts::addStaves(Part* part, const Instrument& instrument, int& globalStaffIndex)
{
    for (int i = 0; i < instrument.staves; i++) {
        Staff* staff = new Staff(score());
        staff->setPart(part);
        initStaff(staff, instrument, Ms::StaffType::preset(StaffType(0)), i);

        if (globalStaffIndex > 0) {
            staff->setBarLineSpan(score()->staff(globalStaffIndex - 1)->barLineSpan());
        }

        score()->undoInsertStaff(staff, i);
        globalStaffIndex++;
    }
}

void NotationParts::insertInstrument(Part* part, Ms::Instrument* instrument, const std::vector<const Staff*>& staves,
                                     const QString& toInstrumentId, InsertMode mode)
{
    InstrumentInfo toInstrumentInfo = instrumentInfo(part, toInstrumentId);

    if (mode == Before) {
        auto it = part->instruments()->lower_bound(toInstrumentInfo.tick);
        int lastStaffIndex = 0;
        if (it != part->instruments()->begin()) {
            lastStaffIndex = this->staves(part, it->second->instrumentId()).back()->idx();
        }

        part->removeInstrument(Ms::Fraction::fromTicks(toInstrumentInfo.tick));
        part->setInstrument(toInstrumentInfo.instrument, Ms::Fraction::fromTicks(toInstrumentInfo.tick + 1));

        part->setInstrument(instrument, Ms::Fraction::fromTicks(toInstrumentInfo.tick));
        for (size_t i = 0; i < staves.size(); i++) {
            Staff* staff = staves[i]->clone();
            staff->setPart(part);
            score()->undoInsertStaff(staff, lastStaffIndex + i);
        }
    } else {
        int lastStaffIndex = this->staves(part, toInstrumentId).back()->idx();

        part->setInstrument(instrument, Ms::Fraction::fromTicks(toInstrumentInfo.tick + 1));
        for (size_t i = 0; i < staves.size(); i++) {
            score()->undoInsertStaff(staves[i]->clone(), lastStaffIndex + i);
        }
    }
}

void NotationParts::removeUnselectedInstruments(const std::vector<QString>& selectedInstrumentIds)
{
    NotifyList<const Part*> parts = partList();
    if (parts.empty()) {
        return;
    }

    std::vector<QString> partsToRemove;
    for (const Part* part: parts) {
        std::vector<QString> instrumentsToRemove;
        auto instrumentList = part->instruments();
        for (auto it = instrumentList->begin(); it != instrumentList->end(); it++) {
            bool existsInSelectedInstruments = std::find(selectedInstrumentIds.begin(),
                                                         selectedInstrumentIds.end(),
                                                         it->second->instrumentId()) != selectedInstrumentIds.end();
            if (!existsInSelectedInstruments) {
                instrumentsToRemove.push_back(it->second->instrumentId());
            }
        }

        bool removeAllInstruments = instrumentsToRemove.size() == part->instruments()->size();
        if (removeAllInstruments) {
            partsToRemove.push_back(part->id());
        } else {
            doRemoveInstruments(this->part(part->id()), instrumentsToRemove);
        }
    }

    if (!partsToRemove.empty()) {
        doRemoveParts(partsToRemove);
    }
}

std::vector<QString> NotationParts::missingInstrumentIds(const std::vector<QString>& selectedInstrumentIds) const
{
    NotifyList<const Part*> parts = partList();
    if (parts.empty()) {
        return {};
    }

    std::vector<QString> missingInstrumentIds = selectedInstrumentIds;

    for (const Part* part: parts) {
        auto instrumentList = part->instruments();
        for (auto it = instrumentList->begin(); it != instrumentList->end(); it++) {
            missingInstrumentIds.erase(std::remove(missingInstrumentIds.begin(), missingInstrumentIds.end(),
                                                   it->second->instrumentId()), missingInstrumentIds.end());
        }
    }

    return missingInstrumentIds;
}

void NotationParts::removeEmptyExcerpts()
{
    const QList<Ms::Excerpt*> excerpts(masterScore()->excerpts());
    for (Ms::Excerpt* excerpt: excerpts) {
        QList<Staff*> staves = excerpt->partScore()->staves();

        if (staves.empty()) {
            masterScore()->undo(new Ms::RemoveExcerpt(excerpt));
        }
    }
}

Ms::Instrument NotationParts::convertedInstrument(const Instrument& instrument) const
{
    Ms::Instrument museScoreInstrument;
    museScoreInstrument.setAmateurPitchRange(instrument.amateurPitchRange.min, instrument.amateurPitchRange.max);
    museScoreInstrument.setProfessionalPitchRange(instrument.professionalPitchRange.min, instrument.professionalPitchRange.max);
    for (Ms::StaffName sn: instrument.longNames) {
        museScoreInstrument.addLongName(StaffName(sn.name(), sn.pos()));
    }
    for (Ms::StaffName sn: instrument.shortNames) {
        museScoreInstrument.addShortName(StaffName(sn.name(), sn.pos()));
    }
    museScoreInstrument.setTrackName(instrument.trackName);
    museScoreInstrument.setTranspose(instrument.transpose);
    museScoreInstrument.setInstrumentId(instrument.id);
    if (instrument.useDrumset) {
        museScoreInstrument.setDrumset(instrument.drumset ? instrument.drumset : Ms::smDrumset);
    }
    for (int i = 0; i < instrument.staves; ++i) {
        museScoreInstrument.setClefType(i, instrument.clefs[i]);
    }
    museScoreInstrument.setMidiActions(convertedMidiActions(instrument.midiActions));
    museScoreInstrument.setArticulation(instrument.midiArticulations);
    for (const instruments::Channel& c : instrument.channels) {
        museScoreInstrument.appendChannel(new instruments::Channel(c));
    }
    museScoreInstrument.setStringData(instrument.stringData);
    museScoreInstrument.setSingleNoteDynamics(instrument.singleNoteDynamics);
    return museScoreInstrument;
}

Instrument NotationParts::convertedInstrument(const Ms::Instrument* museScoreInstrument, const Part* part) const
{
    Instrument instrument;
    instrument.amateurPitchRange = PitchRange(museScoreInstrument->minPitchA(), museScoreInstrument->maxPitchA());
    instrument.professionalPitchRange = PitchRange(museScoreInstrument->minPitchP(), museScoreInstrument->maxPitchP());
    for (Ms::StaffName sn: museScoreInstrument->longNames()) {
        instrument.longNames << StaffName(sn.name(), sn.pos());
    }
    for (Ms::StaffName sn: museScoreInstrument->shortNames()) {
        instrument.shortNames << StaffName(sn.name(), sn.pos());
    }
    instrument.trackName = museScoreInstrument->trackName();
    instrument.transpose = museScoreInstrument->transpose();
    instrument.id = museScoreInstrument->instrumentId();
    instrument.useDrumset = museScoreInstrument->useDrumset();
    instrument.drumset = museScoreInstrument->drumset();
    for (int i = 0; i < museScoreInstrument->cleffTypeCount(); ++i) {
        instrument.clefs[i] = museScoreInstrument->clefType(i);
    }
    instrument.midiActions = convertedMidiActions(museScoreInstrument->midiActions());
    instrument.midiArticulations = museScoreInstrument->articulation();
    for (instruments::Channel* c : museScoreInstrument->channel()) {
        instrument.channels.append(*c);
    }
    instrument.stringData = *museScoreInstrument->stringData();
    instrument.singleNoteDynamics = museScoreInstrument->singleNoteDynamics();
    instrument.visible = isInstrumentVisible(part, instrument.id);
    instrument.isDoubling = part->instrument()->instrumentId() != instrument.id;
    return instrument;
}

bool NotationParts::isInstrumentVisible(const Part* part, const QString& instrumentId) const
{
    for (const Staff* staff: staves(part, instrumentId)) {
        if (!staff->invisible()) {
            return true;
        }
    }

    return false;
}

void NotationParts::initStaff(Staff* staff, const Instrument& instrument, const Ms::StaffType* staffType, int cidx)
{
    const Ms::StaffType* pst = staffType ? staffType : instrument.staffTypePreset;
    if (!pst) {
        pst = Ms::StaffType::getDefaultPreset(instrument.staffGroup);
    }

    Ms::StaffType* stt = staff->setStaffType(Ms::Fraction(0, 1), *pst);
    if (cidx >= MAX_STAVES) {
        stt->setSmall(false);
    } else {
        stt->setSmall(instrument.smallStaff[cidx]);
        staff->setBracketType(0, instrument.bracket[cidx]);
        staff->setBracketSpan(0, instrument.bracketSpan[cidx]);
        staff->setBarLineSpan(instrument.barlineSpan[cidx]);
    }
    staff->setDefaultClefType(instrument.clefs[cidx]);
}

QList<Ms::NamedEventList> NotationParts::convertedMidiActions(const MidiActionList& midiActions) const
{
    QList<Ms::NamedEventList> result;

    for (const MidiAction& action: midiActions) {
        Ms::NamedEventList event;
        event.name = action.name;
        event.descr = action.description;

        for (const midi::Event& midiEvent: action.events) {
            Ms::MidiCoreEvent midiCoreEvent;
            midiCoreEvent.setType(static_cast<uchar>(midiEvent.type()));
            midiCoreEvent.setChannel(midiCoreEvent.channel());
            //!FIXME
            //midiCoreEvent.setData(midiEvent.a, midiEvent.b);
            event.events.push_back(midiCoreEvent);
        }
    }

    return result;
}

MidiActionList NotationParts::convertedMidiActions(const QList<Ms::NamedEventList>& midiActions) const
{
    MidiActionList result;

    for (const Ms::NamedEventList& coreAction: midiActions) {
        MidiAction action;
        action.name = coreAction.name;
        action.description = coreAction.descr;

        for (const Ms::MidiCoreEvent& midiCoreEvent: coreAction.events) {
            midi::Event midiEvent;
            midiEvent.setChannel(midiCoreEvent.channel());
            midiEvent.setType(static_cast<midi::EventType>(midiCoreEvent.type()));

            //!FIXME
            //midiEvent.a = midiCoreEvent.dataA();
            //midiEvent.b = midiCoreEvent.dataB();

            action.events.push_back(midiEvent);
        }
    }

    return result;
}

void NotationParts::sortParts(const std::vector<QString>& instrumentIds)
{
    Q_ASSERT(score()->parts().size() == static_cast<int>(instrumentIds.size()));

    auto generalInstrumentId = [](Part* part) -> QString {
                                   return part->instrument()->instrumentId();
                               };

    for (size_t i = 0; i < instrumentIds.size(); i++) {
        Part* currentPart = score()->parts().at(i);
        if (generalInstrumentId(currentPart) == instrumentIds.at(i)) {
            continue;
        }

        for (int j = i; j < score()->parts().size(); j++) {
            Part* part = score()->parts().at(j);
            if (generalInstrumentId(part) == instrumentIds.at(i)) {
                doMovePart(part->id(), currentPart->id());
                break;
            }
        }
    }
}

void NotationParts::notifyAboutStaffChanged(int staffIndex) const
{
    Staff* staff = this->staff(staffIndex);
    if (!staff) {
        return;
    }

    InstrumentInfo instrumentInfo = this->instrumentInfo(staff);
    ChangedNotifier<const Staff*>* notifier = instrumentNotifier(instrumentInfo.instrument->instrumentId(), staff->part()->id());
    notifier->itemChanged(staff);
}

ChangedNotifier<Instrument>* NotationParts::partNotifier(const QString& partId) const
{
    if (m_partsNotifiersMap.find(partId) != m_partsNotifiersMap.end()) {
        return m_partsNotifiersMap[partId];
    }

    ChangedNotifier<Instrument>* notifier = new ChangedNotifier<Instrument>();
    auto value = std::pair<QString, ChangedNotifier<Instrument>*>(partId, notifier);
    m_partsNotifiersMap.insert(value);
    return notifier;
}

ChangedNotifier<const Staff*>* NotationParts::instrumentNotifier(const QString& instrumentId, const QString& partId) const
{
    std::pair<QString, QString> key(instrumentId, partId);
    if (m_instrumentsNotifiersMap.find(key) != m_instrumentsNotifiersMap.end()) {
        return m_instrumentsNotifiersMap[key];
    }

    ChangedNotifier<const Staff*>* notifier = new ChangedNotifier<const Staff*>();
    auto value = std::pair<std::pair<QString, QString>, ChangedNotifier<const Staff*>*>(key, notifier);
    m_instrumentsNotifiersMap.insert(value);
    return notifier;
}

QString NotationParts::calculatedPartName(const Part* part) const
{
    QStringList instrumentsNames;
    for (auto it = part->instruments()->cbegin(); it != part->instruments()->cend(); ++it) {
        instrumentsNames << it->second->trackName();
    }

    return instrumentsNames.join(" & ");
}
