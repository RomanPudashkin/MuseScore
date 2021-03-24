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
static const QString SAMPLE_RATE_INDEX("sampleRateIndex");
static const QString FRAGMENTS_NUMBER_KEY("fragmentsNumber");
static const QString PERIOD_SIZE_INDEX_KEY("periodSizeIndex");
static const QString AVAILABLE_SAMPLE_RATE_LIST_KEY("availableSampleRateList");
static const QString AVAILABLE_PERIOD_SIZE_LIST_KEY("availablePeriodSizeList");

// PortAudio configuration keys:
static const QString API_INDEX_KEY("apiIndex");
static const QString DEVICE_INDEX_KEY("deviceIndex");
static const QString MIDI_INPUT_INDEX_KEY("midiInputIndex");
static const QString MIDI_OUTPUT_INDEX_KEY("midiOutputIndex");
static const QString MIDI_OUTPUT_LATENCY_KEY("midiOutputLatencyMilliseconds");
static const QString AVAILABLE_API_LIST_KEY("availableApiList");
static const QString AVAILABLE_DEVICE_LIST_KEY("availableDeviceList");
static const QString AVAILABLE_MIDI_INPUT_LIST_KEY("availableMidiInputList");
static const QString AVAILABLE_MIDI_OUTPUT_LIST_KEY("availableMidiOutputList");

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
    NOT_IMPLEMENTED;

    QVariantMap configuration;
    configuration[DEVICE_NAME_KEY] = QString();
    configuration[SAMPLE_RATE_INDEX] = 0;
    configuration[FRAGMENTS_NUMBER_KEY] = 0;
    configuration[PERIOD_SIZE_INDEX_KEY] = 0;
    configuration[AVAILABLE_SAMPLE_RATE_LIST_KEY] = QStringList();
    configuration[AVAILABLE_PERIOD_SIZE_LIST_KEY] = QStringList();

    return configuration;
}

bool IOPreferencesModel::isPortAudioUsed() const
{
    return isAudioSystemUsed(AudioSystemType::PortAudio);
}

QVariant IOPreferencesModel::portAudioConfiguration() const
{
    NOT_IMPLEMENTED;

    QVariantMap configuration;

    configuration[API_INDEX_KEY] = 0;
    configuration[DEVICE_INDEX_KEY] = 0;
    configuration[MIDI_INPUT_INDEX_KEY] = 0;
    configuration[MIDI_OUTPUT_INDEX_KEY] = 0;
    configuration[MIDI_OUTPUT_LATENCY_KEY] = 0;
    configuration[AVAILABLE_API_LIST_KEY] = QStringList();
    configuration[AVAILABLE_DEVICE_LIST_KEY] = QStringList();
    configuration[AVAILABLE_MIDI_INPUT_LIST_KEY] = QStringList();
    configuration[AVAILABLE_MIDI_OUTPUT_LIST_KEY] = QStringList();

    return configuration;
}

bool IOPreferencesModel::isJaskAudioServerUsed() const
{
    return isAudioSystemUsed(AudioSystemType::JackAudioServer);
}

QVariant IOPreferencesModel::jackAudioServerConfiguration() const
{
    NOT_IMPLEMENTED;

    QVariantMap configuration;

    configuration[USE_JACK_AUDIO_KEY] = false;
    configuration[USE_JACK_MIDI_KEY] = false;
    configuration[USE_JACK_TRANSPORT_KEY] = false;
    configuration[USE_TIMEBASE_MASTER_KEY] = false;
    configuration[REMEMBER_LAST_CONNECTIONS_KEY] = false;

    return configuration;
}

bool IOPreferencesModel::isPulseAudioAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::PulseAudio);
}

bool IOPreferencesModel::isAlsaAudioAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::AlsaAudio);
}

bool IOPreferencesModel::isPortAudioAvailable() const
{
    return isAudioSystemAvailable(AudioSystemType::PortAudio);
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
        { "PortAudio", AudioSystemType::PortAudio },
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
    NOT_IMPLEMENTED;

    if (configuration == alsaAudioConfiguration() || configuration.isNull()) {
        return;
    }

    emit alsaAudioConfigurationChanged(configuration);
}

void IOPreferencesModel::setPortAudioConfiguration(const QVariant& configuration)
{
    NOT_IMPLEMENTED;

    if (configuration == portAudioConfiguration() || configuration.isNull()) {
        return;
    }

    emit portAudioConfigurationChanged(configuration);
}

void IOPreferencesModel::setJackAudioServerConfiguration(const QVariant& configuration)
{
    NOT_IMPLEMENTED;

    if (configuration == jackAudioServerConfiguration() || configuration.isNull()) {
        return;
    }

    emit jackAudioServerConfigurationChanged(configuration);
}
