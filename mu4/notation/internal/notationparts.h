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
#ifndef MU_NOTATION_NOTATIONPARTS_H
#define MU_NOTATION_NOTATIONPARTS_H

#include "inotationparts.h"
#include "igetscore.h"
#include "async/asyncable.h"

namespace mu {
namespace notation {
class NotationParts : public INotationParts, public async::Asyncable
{
public:
    NotationParts(IGetScore* getScore, mu::async::Notification selectionChangedNotification);

    async::NotifyList<const Part*> partList() const override;
    async::NotifyList<instruments::Instrument> instrumentList(const ID& partId) const override;
    async::NotifyList<const Staff*> staffList(const ID& partId, const ID& instrumentId) const override;

    ValCh<bool> canChangeInstrumentVisibility(const ID& partId, const ID& instrumentId) const override;

    void setInstruments(const instruments::InstrumentList& instruments) override;
    void setPartVisible(const ID& partId, bool visible) override;
    void setInstrumentVisible(const ID& partId, const ID& instrumentId, bool visible) override;
    void setStaffVisible(const ID& staffId, bool visible) override;
    void setVoiceVisible(const ID& staffId, int voiceIndex, bool visible) override;
    void setPartName(const ID& partId, const QString& name) override;
    void setInstrumentName(const ID& partId, const ID& instrumentId, const QString& name) override;
    void setInstrumentAbbreviature(const ID& partId, const ID& instrumentId, const QString& abbreviature) override;
    void setStaffType(const ID& staffId, StaffType type) override;
    void setCutawayEnabled(const ID& staffId, bool value) override;
    void setSmallStaff(const ID& staffId, bool value) override;

    void removeParts(const IDList& partsIds) override;
    void removeInstruments(const ID& partId, const IDList& instrumentsIds) override;
    void removeStaves(const IDList& stavesIds) override;

    void moveParts(const IDList& partsIds, const ID& toPartId, InsertMode mode = Before) override;
    void moveInstruments(const IDList& instrumentIds, const ID& fromPartId, const ID& toPartId,
                         const ID& toInstrumentId, InsertMode mode = Before) override;
    void moveStaves(const IDList& stavesIds, const ID& toStaffId, InsertMode mode = Before) override;

    void appendDoublingInstrument(const ID& partId, const instruments::Instrument& instrument) override;
    void appendStaff(const ID& partId, const ID& instrumentId) override;
    void appendLinkedStaff(const ID& originStaffId) override;

    void replaceInstrument(const ID& partId, const ID& instrumentId, const instruments::Instrument& newInstrument) override;

    async::Notification partsChanged() const override;

private:
    ~NotationParts() override;

    struct InstrumentInfo
    {
        int tick = 0;
        Ms::Instrument* instrument = nullptr;

        InstrumentInfo() = default;

        InstrumentInfo(int tick, Ms::Instrument* instrument)
            : tick(tick), instrument(instrument) {}

        bool isValid() const { return instrument != nullptr; }
    };

    struct InstrumentKey
    {
        ID partId;
        ID instrumentId;

        bool operator==(const InstrumentKey& key) const
        {
            return partId == key.partId && instrumentId == key.instrumentId;
        }

        friend uint qHash(const InstrumentKey& key)
        {
            return qHash(QString(key.partId + key.instrumentId));
        }
    };

    Ms::Score* score() const;
    Ms::MasterScore* masterScore() const;

    void startEdit();
    void apply();

    Ms::ChordRest* selectedChord() const;
    void updateCanChangeInstrumentsVisibility();
    bool resolveCanChangeInstrumentVisibility(const ID& partId, const ID& instrumentId) const;
    bool needAssignInstrumentToChord(const ID& partId, const ID& instrumentId) const;
    void assignIstrumentToSelectedChord(Ms::Instrument* instrument);

    void doMovePart(const ID& partId, const ID& toPartId, InsertMode mode = Before);
    void doSetStaffVisible(Staff* staff, bool visible);
    void doRemoveParts(const IDList& partsIds);
    void doRemoveInstruments(Part* part, const IDList& instrumentsIds);
    void doRemoveStaves(const IDList& stavesIds);
    void doSetPartName(Part* part, const QString& name);

    Part* part(const ID& partId, const Ms::Score* score = nullptr) const;
    InstrumentInfo instrumentInfo(const Part* part, const ID& instrumentId) const;
    InstrumentInfo instrumentInfo(const Staff* staff) const;

    Staff* staff(const ID& staffId) const;
    std::vector<const Staff*> staves(const Part* part, const ID& instrumentId) const;
    std::vector<Staff*> staves(const IDList& stavesIds) const;

    std::vector<Part*> availableParts() const;
    std::vector<Part*> scoreParts(const Ms::Score* score) const;
    std::vector<Part*> excerptParts(const Ms::Score* score) const;

    void appendPart(Part* part);
    void addStaves(Part* part, const instruments::Instrument& instrument, int& globalStaffIndex);

    void insertInstrument(Part* part, Ms::Instrument* instrumentInfo, const std::vector<const Staff*>& staves, const ID& toInstrumentId,
                          InsertMode mode);

    void removeUnselectedInstruments(const IDList& selectedInstrumentIds);
    IDList missingInstrumentIds(const IDList& selectedInstrumentIds) const;

    void removeEmptyExcerpts();

    Ms::Instrument convertedInstrument(const instruments::Instrument& instrument) const;
    instruments::Instrument convertedInstrument(const Ms::Instrument* museScoreInstrument, const Part* part) const;

    bool isInstrumentVisible(const Part* part, const ID& instrumentId) const;

    void initStaff(Staff* staff, const instruments::Instrument& instrument, const Ms::StaffType* staffType, int cleffIndex);

    QList<Ms::NamedEventList> convertedMidiActions(const instruments::MidiActionList& midiActions) const;
    instruments::MidiActionList convertedMidiActions(const QList<Ms::NamedEventList>& midiActions) const;

    void sortParts(const IDList& instrumentIds);

    void notifyAboutStaffChanged(const ID& staffId) const;

    async::ChangedNotifier<instruments::Instrument>* partNotifier(const ID& partId) const;
    async::ChangedNotifier<const Staff*>* instrumentNotifier(const ID& instrumentId, const ID& partId) const;

    QString formatPartName(const Part* part) const;

    IGetScore* m_getScore = nullptr;
    async::Notification m_partsChanged;

    mutable async::ChangedNotifier<const Part*>* m_partsNotifier = nullptr;
    mutable std::map<ID, async::ChangedNotifier<instruments::Instrument>*> m_partsNotifiersMap;
    mutable QHash<InstrumentKey, async::ChangedNotifier<const Staff*>*> m_instrumentsNotifiersHash;
    mutable QHash<InstrumentKey, ValCh<bool>> m_canChangeInstrumentsVisibilityHash;
};
}
}

#endif // MU_NOTATION_NOTATIONPARTS_H
