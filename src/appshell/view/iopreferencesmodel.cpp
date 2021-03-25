//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2021 MuseScore BVBA and others
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

#include "iopreferencesmodel.h"

#include "log.h"

using namespace mu::appshell;
using namespace mu::audio;

// ALSA Audio configuration keys:
static const QString DEVICE_NAME_KEY("deviceName");
static const QString SAMPLE_RATE_KEY("sampleRateHz");
static const QString FRAGMENTS_NUMBER_KEY("fragmentsNumber");
static const QString PERIOD_SIZE_KEY("periodSize");
static const QString AVAILABLE_SAMPLE_RATE_LIST_KEY("availableSampleRateList");
static const QString AVAILABLE_PERIOD_SIZE_LIST_KEY("availablePeriodSizeList");

// JACK Audio Server configuration keys:
static const QString USE_JACK_AUDIO_KEY("useJackAudio");
static const QString USE_JACK_TRANSPORT_KEY("useJackTransport");
static const QString USE_JACK_MIDI_KEY("useJackMidi");
static const QString USE_TIMEBASE_MASTER_KEY("useTimeBaseMaster");
static const QString REMEMBER_LAST_CONNECTIONS_KEY("rememberLastConnections");

IOPreferencesModel::IOPreferencesModel(QObject* parent)
    : QObject(parent)
{
}

bool IOPreferencesModel::isPulseAudioUsed() const
{
    return isAudioSystemUsed(AudioSystemType::PulseAudio);
}

bool IOPreferencesModel::isAlsaAudioUsed() const
{
    return isAudioSystemUsed(AudioSystemType::AlsaAudio);
}

QVariant IOPreferencesModel::alsaAudioConfiguration() const
{
    AlsaAudioConfiguration configuration = audioConfiguration()->alsaAudioConfiguration();

    QStringList sampleRateList;
    for (int sampleRate : audioConfiguration()->availableAlsaSampleRates()) {
        sampleRateList << QString::number(sampleRate);
    }

    QStringList periodSizeList;
    for (int periodSize : audioConfiguration()->availableAlsaPeriodSizes()) {
        periodSizeList << QString::number(periodSize);
    }

    QVariantMap obj;
    obj[DEVICE_NAME_KEY] = QString::fromStdString(configuration.deviceName);
    obj[FRAGMENTS_NUMBER_KEY] = configuration.fragmentCount;
    obj[PERIOD_SIZE_KEY] = configuration.periodSize;
    obj[SAMPLE_RATE_KEY] = configuration.sampleRateHz;
    obj[AVAILABLE_SAMPLE_RATE_LIST_KEY] = sampleRateList;
    obj[AVAILABLE_PERIOD_SIZE_LIST_KEY] = periodSizeList;

    return obj;
}

bool IOPreferencesModel::isJaskAudioServerUsed() const
{
    return isAudioSystemUsed(AudioSystemType::JackAudioServer);
}

QVariant IOPreferencesModel::jackAudioServerConfiguration() const
{
    JackAudioServerConfiguration configuration = audioConfiguration()->jackAudioServerConfiguration();

    QVariantMap obj;
    obj[USE_JACK_AUDIO_KEY] = configuration.useJackAudio;
    obj[USE_JACK_MIDI_KEY] = configuration.useJackMidi;
    obj[USE_JACK_TRANSPORT_KEY] = configuration.useJackTransport;
    obj[USE_TIMEBASE_MASTER_KEY] = configuration.useTimebaseMaster;
    obj[REMEMBER_LAST_CONNECTIONS_KEY] = configuration.rememberLastConnections;

    return obj;
}

bool IOPreferencesModel::isPulseAudioAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::PulseAudio);
}

bool IOPreferencesModel::isAlsaAudioAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::AlsaAudio);
}

bool IOPreferencesModel::isJackAudioServerAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::JackAudioServer);
}

bool IOPreferencesModel::isAudioSystemAvailable(AudioSystemType type) const
{
    return audioConfiguration()->isAudioSystemAvailable(type);
}

bool IOPreferencesModel::isAudioSystemUsed(audio::AudioSystemType type) const
{
    return audioConfiguration()->currentAudioSystem() == type;
}

void IOPreferencesModel::setUsedAudioSystem(const QString& audioSystemName)
{
    static const QMap<QString, AudioSystemType> types {
        { "AlsaAudio", AudioSystemType::AlsaAudio },
        { "PulseAudio", AudioSystemType::PulseAudio },
        { "JackAudioServer", AudioSystemType::JackAudioServer }
    };

    if (types.contains(audioSystemName)) {
        audioConfiguration()->setCurrentAudioSystem(types[audioSystemName]);
        emit usedAudioSystemChanged();
    }
}

void IOPreferencesModel::restartAudioAndMidiDevices()
{
    NOT_IMPLEMENTED;
}

void IOPreferencesModel::setAlsaAudioConfiguration(const QVariant& configuration)
{
    if (configuration == alsaAudioConfiguration() || configuration.isNull()) {
        return;
    }

    QVariantMap map = configuration.toMap();

    AlsaAudioConfiguration result;
    result.deviceName = map[DEVICE_NAME_KEY].toString().toStdString();
    result.fragmentCount = map[FRAGMENTS_NUMBER_KEY].toInt();
    result.periodSize = map[PERIOD_SIZE_KEY].toInt();
    result.sampleRateHz = map[SAMPLE_RATE_KEY].toInt();

    audioConfiguration()->setAlsaAudioConfiguration(result);
    emit alsaAudioConfigurationChanged(configuration);
}

void IOPreferencesModel::setJackAudioServerConfiguration(const QVariant& configuration)
{
    if (configuration == jackAudioServerConfiguration() || configuration.isNull()) {
        return;
    }

    QVariantMap map = configuration.toMap();

    JackAudioServerConfiguration result;
    result.useJackAudio = map[USE_JACK_AUDIO_KEY].toBool();
    result.useJackMidi = map[USE_JACK_MIDI_KEY].toBool();
    result.useJackTransport = map[USE_JACK_TRANSPORT_KEY].toBool();
    result.useTimebaseMaster = map[USE_TIMEBASE_MASTER_KEY].toBool();
    result.rememberLastConnections = map[REMEMBER_LAST_CONNECTIONS_KEY].toBool();

    audioConfiguration()->setJackAudioServerConfiguration(result);
    emit jackAudioServerConfigurationChanged(configuration);
}
