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
#include "additionalinfomodel.h"

#include "translation.h"
#include "ui/view/iconcodes.h"

using namespace mu::userscores;
using namespace mu::framework;
using namespace mu::domain::notation;

AdditionalInfoModel::AdditionalInfoModel(QObject* parent)
    : QObject(parent)
{
}

void AdditionalInfoModel::init()
{
    setKeySignature(KeySignature(qtrc("userscore", "None"), IconCode::Code::KEY_SIGNATURE_NONE, Key::C, KeyMode::MAJOR).toMap()); // TODO

    setTimeFraction(true);
    setTimeSignature(TimeSignature(4, 4).toMap());

    setWithTempo(false);
    setTempo(120);

    setWithPickupMeasure(false);
    setMeasureCount(32);
    setPickupTimeSignature(TimeSignature(4, 4).toMap());
}

QVariantList AdditionalInfoModel::keySignatureMajorList()
{
    QVariantList majorList;
    majorList << KeySignature(qtrc("userscore", "C major"), IconCode::Code::KEY_SIGNATURE_NONE, Key::C, KeyMode::MAJOR).toMap()// TODO
              << KeySignature(qtrc("userscore", "F major"), IconCode::Code::KEY_SIGNATURE_1_FLAT, Key::F, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Bb major"), IconCode::Code::KEY_SIGNATURE_2_FLAT, Key::B_B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Eb major"), IconCode::Code::KEY_SIGNATURE_3_FLAT, Key::E_B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Ab major"), IconCode::Code::KEY_SIGNATURE_4_FLAT, Key::A_B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Db major"), IconCode::Code::KEY_SIGNATURE_5_FLAT, Key::D_B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Gb major"), IconCode::Code::KEY_SIGNATURE_6_FLAT, Key::G_B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "Cb major"), IconCode::Code::KEY_SIGNATURE_7_FLAT, Key::C_B, KeyMode::MAJOR).toMap()

              << KeySignature(qtrc("userscore", "None"), IconCode::Code::KEY_SIGNATURE_NONE, Key::C, KeyMode::MAJOR).toMap()// TODO
              << KeySignature(qtrc("userscore", "G major"), IconCode::Code::KEY_SIGNATURE_1_SHARP, Key::G, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "D major"), IconCode::Code::KEY_SIGNATURE_2_SHARPS, Key::D, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "A major"), IconCode::Code::KEY_SIGNATURE_3_SHARPS, Key::A, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "E major"), IconCode::Code::KEY_SIGNATURE_4_SHARPS, Key::E, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "B major"), IconCode::Code::KEY_SIGNATURE_5_SHARPS, Key::B, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "F# major"), IconCode::Code::KEY_SIGNATURE_6_SHARPS, Key::F_S, KeyMode::MAJOR).toMap()
              << KeySignature(qtrc("userscore", "C# major"), IconCode::Code::KEY_SIGNATURE_7_SHARPS, Key::C_S, KeyMode::MAJOR).toMap();

    return majorList;
}

QVariantList AdditionalInfoModel::keySignatureMinorList()
{
    QVariantList minorList;
    minorList << KeySignature(qtrc("userscore", "A minor"), IconCode::Code::KEY_SIGNATURE_NONE, Key::C, KeyMode::MINOR).toMap() // TODO none
              << KeySignature(qtrc("userscore", "D minor"), IconCode::Code::KEY_SIGNATURE_1_FLAT, Key::F, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "G minor"), IconCode::Code::KEY_SIGNATURE_2_FLAT, Key::B_B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "C minor"), IconCode::Code::KEY_SIGNATURE_3_FLAT, Key::E_B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "F minor"), IconCode::Code::KEY_SIGNATURE_4_FLAT, Key::A_B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "Bb minor"), IconCode::Code::KEY_SIGNATURE_5_FLAT, Key::D_B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "Eb minor"), IconCode::Code::KEY_SIGNATURE_6_FLAT, Key::G_B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "Ab minor"), IconCode::Code::KEY_SIGNATURE_7_FLAT, Key::C_B, KeyMode::MINOR).toMap()

              << KeySignature(qtrc("userscore", "None"), IconCode::Code::KEY_SIGNATURE_NONE, Key::C, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "E minor"), IconCode::Code::KEY_SIGNATURE_1_SHARP, Key::G, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "B minor"), IconCode::Code::KEY_SIGNATURE_2_SHARPS, Key::D, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "F# minor"), IconCode::Code::KEY_SIGNATURE_3_SHARPS, Key::A, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "C# minor"), IconCode::Code::KEY_SIGNATURE_4_SHARPS, Key::E, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "G# minor"), IconCode::Code::KEY_SIGNATURE_5_SHARPS, Key::B, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "D# minor"), IconCode::Code::KEY_SIGNATURE_6_SHARPS, Key::F_S, KeyMode::MINOR).toMap()
              << KeySignature(qtrc("userscore", "A# minor"), IconCode::Code::KEY_SIGNATURE_7_SHARPS, Key::C_S, KeyMode::MINOR).toMap();

    return minorList;
}

QVariantMap AdditionalInfoModel::keySignature() const
{
    return m_keySignature.toMap();
}

QVariantMap AdditionalInfoModel::timeSignature() const
{
    return m_timeSignature.toMap();
}

int AdditionalInfoModel::timeSignatureType() const
{
    if (timeFraction()) {
        return static_cast<int>(TimeSigType::NORMAL);
    }
    if (timeCommon()) {
        return static_cast<int>(TimeSigType::FOUR_FOUR);
    }
    return static_cast<int>(TimeSigType::ALLA_BREVE);
}

void AdditionalInfoModel::setTimeSignatureNumerator(int numerator)
{
    m_timeSignature.numerator = numerator;
    setTimeSignature(m_timeSignature.toMap());
}

void AdditionalInfoModel::setTimeSignatureDenominator(int denominator)
{
    m_timeSignature.denominator = denominator;
    setTimeSignature(m_timeSignature.toMap());
}

bool AdditionalInfoModel::timeCommon() const
{
    return m_timeCommon;
}

bool AdditionalInfoModel::timeCut() const
{
    return m_timeCut;
}

int AdditionalInfoModel::tempo() const
{
    return m_tempo;
}

bool AdditionalInfoModel::withTempo() const
{
    return m_withTempo;
}

QVariantMap AdditionalInfoModel::tempoRange()
{
    return { {"min", 20}, {"max", 400} };
}

QVariantList AdditionalInfoModel::tempoMarks()
{
    QVariantList marks;
    // TODO: codes and values nedded actualize
    marks << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 120}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 150}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 180}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 210}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 240}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 270}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 300}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 330}}
          << QVariantMap{ {"icon", static_cast<int>(IconCode::Code::NOTE_HEAD_QUARTER)}, {"value", 400}};

    return marks;
}

QVariantMap AdditionalInfoModel::pickupTimeSignature() const
{
    return m_pickupTimeSignature.toMap();
}

void AdditionalInfoModel::setPickupTimeSignatureNumerator(int numerator)
{
    m_pickupTimeSignature.numerator = numerator;
    setPickupTimeSignature(m_pickupTimeSignature.toMap());
}

void AdditionalInfoModel::setPickupTimeSignatureDenominator(int denominator)
{
    m_pickupTimeSignature.denominator = denominator;
    setPickupTimeSignature(m_pickupTimeSignature.toMap());
}

bool AdditionalInfoModel::withPickupMeasure() const
{
    return m_withPickupMeasure;
}

int AdditionalInfoModel::measureCount() const
{
    return m_measureCount;
}

QVariantMap AdditionalInfoModel::measureCountRange()
{
    return { {"min", 1}, {"max", 9999} };
}

bool AdditionalInfoModel::timeFraction() const
{
    return m_timeFraction;
}

QVariantMap AdditionalInfoModel::timeSignatureNumeratorRange()
{
    return { {"min", 1}, {"max", 63} };
}

QStringList AdditionalInfoModel::timeSignatureDenominators()
{
    return {"1", "2", "4", "8", "16", "32", "64"};
}

void AdditionalInfoModel::setKeySignature(QVariantMap keySignature)
{
    m_keySignature = KeySignature(keySignature);
    emit keySignatureChanged(keySignature);
}

void AdditionalInfoModel::setTimeSignature(QVariantMap timeSignature)
{
    m_timeSignature = TimeSignature(timeSignature);
    emit timeSignatureChanged(timeSignature);
}

void AdditionalInfoModel::setTimeFraction(bool timeFraction)
{
    if (m_timeFraction == timeFraction) {
        return;
    }

    m_timeFraction = timeFraction;
    emit timeFractionChanged(m_timeFraction);
    emit timeSignatureTypeChanged();
}

void AdditionalInfoModel::setTimeCommon(bool timeCommon)
{
    if (m_timeCommon == timeCommon) {
        return;
    }

    m_timeCommon = timeCommon;
    emit timeCommonChanged(m_timeCommon);
    emit timeSignatureTypeChanged();

    if (timeCommon) {
        setTimeSignature(TimeSignature(4, 4).toMap());
    }
}

void AdditionalInfoModel::setTimeCut(bool timeCut)
{
    if (m_timeCut == timeCut) {
        return;
    }

    m_timeCut = timeCut;
    emit timeCutChanged(m_timeCut);
    emit timeSignatureTypeChanged();

    if (timeCut) {
        setTimeSignature(TimeSignature(2, 2).toMap());
    }
}

void AdditionalInfoModel::setTempo(int tempo)
{
    if (m_tempo == tempo) {
        return;
    }

    m_tempo = tempo;
    emit tempoChanged(m_tempo);
}

void AdditionalInfoModel::setWithTempo(bool withTempo)
{
    if (m_withTempo == withTempo) {
        return;
    }

    m_withTempo = withTempo;
    emit withTempoChanged(m_withTempo);
}

void AdditionalInfoModel::setPickupTimeSignature(QVariantMap pickupTimeSignature)
{
    m_pickupTimeSignature = TimeSignature(pickupTimeSignature);
    emit pickupTimeSignatureChanged(pickupTimeSignature);
}

void AdditionalInfoModel::setWithPickupMeasure(bool withPickupMeasure)
{
    if (m_withPickupMeasure == withPickupMeasure) {
        return;
    }

    m_withPickupMeasure = withPickupMeasure;
    emit withPickupMeasureChanged(m_withPickupMeasure);
}

void AdditionalInfoModel::setMeasureCount(int measureCount)
{
    if (m_measureCount == measureCount) {
        return;
    }

    m_measureCount = measureCount;
    emit measureCountChanged(m_measureCount);
}
