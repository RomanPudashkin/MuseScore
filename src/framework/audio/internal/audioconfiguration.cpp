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
#include "audioconfiguration.h"
#include "settings.h"
#include "stringutils.h"

#include "global/xmlreader.h"
#include "global/xmlwriter.h"

#include "log.h"

//TODO: remove with global clearing of Q_OS_*** defines
#include <QtGlobal>

using namespace mu;
using namespace mu::framework;
using namespace mu::audio;
using namespace mu::audio::synth;

//TODO: add other setting: audio device etc
static const Settings::Key AUDIO_BUFFER_SIZE("audio", "driver_buffer");

static const Settings::Key USER_SOUNDFONTS_PATH("midi", "application/paths/mySoundfonts");

//! FIXME Temporary for tests
static const std::string DEFAULT_FLUID_SOUNDFONT = "MuseScore_General.sf3";     // "GeneralUser GS v1.471.sf2"; // "MuseScore_General.sf3";
static const std::string DEFAULT_ZERBERUS_SOUNDFONT = "FM-Piano1-20190916.sfz"; // "";

static const Settings::Key CURRENT_AUDIO_SYSTEM_KEY("audio", "io/currentAudioSystem");

static const Settings::Key ALSA_AUDIO_DEVICE_KEY("audio", "io/alsa/device");
static const Settings::Key ALSA_AUDIO_FRAGMENTS_KEY("audio", "io/alsa/fragments");
static const Settings::Key ALSA_AUDIO_PERIOD_SIZE_KEY("audio", "io/alsa/periodSize");
static const Settings::Key ALSA_AUDIO_SAMPLE_RATE_KEY("audio", "io/alsa/sampleRate");

static const Settings::Key JACK_AUDIO_SERVER_REMEMBER_LASTE_CONNECTIONS_KEY("audio", "io/jack/rememberLastConnections");
static const Settings::Key JACK_AUDIO_SERVER_TIMEBASE_MASTER_KEY("audio", "io/jack/timebaseMaster");
static const Settings::Key JACK_AUDIO_SERVER_USE_JACK_AUDIO_KEY("audio", "io/jack/useJackAudio");
static const Settings::Key JACK_AUDIO_SERVER_USE_JACK_MIDI_KEY("audio", "io/jack/useJackMIDI");
static const Settings::Key JACK_AUDIO_SERVER_USE_JACK_TRANSPORT_KEY("audio", "io/jack/useJackTransport");

static const Settings::Key PORT_AUDIO_DEVICE_INDEX_KEY("audio", "io/portAudio/device");
static const Settings::Key PORT_AUDIO_INPUT_DEVICE_NAME_KEY("audio", "io/portMidi/inputDevice");
static const Settings::Key PORT_AUDIO_OUTPUT_DEVICE_NAME_KEY("audio", "io/portMidi/outputDevice");
static const Settings::Key PORT_AUDIO_OUTPUT_LATENCY_KEY("audio", "io/portMidi/io/portMidi/outputLatencyMilliseconds");

void AudioConfiguration::init()
{
    settings()->setDefaultValue(CURRENT_AUDIO_SYSTEM_KEY, Val(static_cast<int>(defaultAudioSystem())));
    settings()->setDefaultValue(AUDIO_BUFFER_SIZE, Val(defaultAudioBufferSize()));

    settings()->setDefaultValue(ALSA_AUDIO_DEVICE_KEY, Val("default"));
    settings()->setDefaultValue(ALSA_AUDIO_FRAGMENTS_KEY, Val(3));
    settings()->setDefaultValue(ALSA_AUDIO_PERIOD_SIZE_KEY, Val(1024));
    settings()->setDefaultValue(ALSA_AUDIO_SAMPLE_RATE_KEY, Val(48000));

    settings()->setDefaultValue(JACK_AUDIO_SERVER_REMEMBER_LASTE_CONNECTIONS_KEY, Val(true));

    settings()->setDefaultValue(PORT_AUDIO_OUTPUT_LATENCY_KEY, Val(0));
}

AudioSystemType AudioConfiguration::defaultAudioSystem() const
{
    for (AudioSystemType type : allAudioSystemTypes()) {
        if (isAudioSystemAvailable(type)) {
            return type;
        }
    }

    return AudioSystemType::PortAudio;
}

int AudioConfiguration::defaultAudioBufferSize() const
{
#ifdef Q_OS_WASM
    return 8192;
#else
    return 1024;
#endif
}

bool AudioConfiguration::isAudioSystemAvailable(AudioSystemType type) const
{
    return true; // temporary

    switch (type) {
    case AudioSystemType::AlsaAudio:
#ifndef USE_ALSA
        return true;
#else
        return false;
#endif
    case AudioSystemType::PortAudio:
#ifdef USE_PORTAUDIO
        return true;
#else
        return false;
#endif
    case AudioSystemType::PulseAudio:
#ifdef USE_PULSEAUDIO
        return true;
#else
        return false;
#endif
    case AudioSystemType::JackAudioServer:
#ifdef USE_JACK
        return true;
#else
        return false;
#endif
    }

    return false;
}

AudioSystemType AudioConfiguration::currentAudioSystem() const
{
    return static_cast<AudioSystemType>(settings()->value(CURRENT_AUDIO_SYSTEM_KEY).toInt());
}

void AudioConfiguration::setCurrentAudioSystem(AudioSystemType type)
{
    settings()->setValue(CURRENT_AUDIO_SYSTEM_KEY, Val(static_cast<int>(type)));
}

AlsaAudioConfiguration AudioConfiguration::alsaAudioConfiguration() const
{
    AlsaAudioConfiguration configuration;
    configuration.deviceName = settings()->value(ALSA_AUDIO_DEVICE_KEY).toString();
    configuration.fragmentCount = settings()->value(ALSA_AUDIO_FRAGMENTS_KEY).toInt();
    configuration.periodSize = settings()->value(ALSA_AUDIO_PERIOD_SIZE_KEY).toInt();
    configuration.sampleRateHz = settings()->value(ALSA_AUDIO_SAMPLE_RATE_KEY).toInt();

    return configuration;
}

void AudioConfiguration::setAlsaAudioConfiguration(const AlsaAudioConfiguration& configuration)
{
    settings()->setValue(ALSA_AUDIO_DEVICE_KEY, Val(configuration.deviceName));
    settings()->setValue(ALSA_AUDIO_FRAGMENTS_KEY, Val(configuration.fragmentCount));
    settings()->setValue(ALSA_AUDIO_PERIOD_SIZE_KEY, Val(configuration.periodSize));
    settings()->setValue(ALSA_AUDIO_SAMPLE_RATE_KEY, Val(configuration.sampleRateHz));
}

std::vector<int> AudioConfiguration::availableAlsaSampleRates() const
{
    return {
        192000,
        96000,
        88200,
        48000,
        44100,
        32000,
        22050
    };
}

std::vector<int> AudioConfiguration::availableAlsaPeriodSizes() const
{
    return {
        4096,
        2048,
        1024,
        512,
        256,
        128,
        64
    };
}

JackAudioServerConfiguration AudioConfiguration::jackAudioServerConfiguration() const
{
    JackAudioServerConfiguration configuration;
    configuration.useJackAudio = settings()->value(JACK_AUDIO_SERVER_USE_JACK_AUDIO_KEY).toBool();
    configuration.useJackMidi = settings()->value(JACK_AUDIO_SERVER_USE_JACK_MIDI_KEY).toBool();
    configuration.useJackTransport = settings()->value(JACK_AUDIO_SERVER_USE_JACK_TRANSPORT_KEY).toBool();
    configuration.useTimebaseMaster = settings()->value(JACK_AUDIO_SERVER_TIMEBASE_MASTER_KEY).toBool();
    configuration.rememberLastConnections = settings()->value(JACK_AUDIO_SERVER_REMEMBER_LASTE_CONNECTIONS_KEY).toBool();

    return configuration;
}

void AudioConfiguration::setJackAudioServerConfiguration(const JackAudioServerConfiguration& configuration)
{
    settings()->setValue(JACK_AUDIO_SERVER_USE_JACK_AUDIO_KEY, Val(configuration.useJackAudio));
    settings()->setValue(JACK_AUDIO_SERVER_USE_JACK_MIDI_KEY, Val(configuration.useJackMidi));
    settings()->setValue(JACK_AUDIO_SERVER_USE_JACK_TRANSPORT_KEY, Val(configuration.useJackTransport));
    settings()->setValue(JACK_AUDIO_SERVER_TIMEBASE_MASTER_KEY, Val(configuration.useTimebaseMaster));
    settings()->setValue(JACK_AUDIO_SERVER_REMEMBER_LASTE_CONNECTIONS_KEY, Val(configuration.rememberLastConnections));
}

PortAudioConfiguration AudioConfiguration::portAudioConfiguration() const
{
    PortAudioConfiguration configuration;
    configuration.deviceIndex = settings()->value(PORT_AUDIO_DEVICE_INDEX_KEY).toInt();
    configuration.midiInputDeviceName = settings()->value(PORT_AUDIO_INPUT_DEVICE_NAME_KEY).toString();
    configuration.midiOutputDeviceName = settings()->value(PORT_AUDIO_OUTPUT_DEVICE_NAME_KEY).toString();
    configuration.midiOutputLatencyMilliseconds = settings()->value(PORT_AUDIO_OUTPUT_LATENCY_KEY).toInt();

    return configuration;
}

void AudioConfiguration::setPortAudioConfiguration(const PortAudioConfiguration& configuration)
{
    settings()->setValue(PORT_AUDIO_DEVICE_INDEX_KEY, Val(configuration.deviceIndex));
    settings()->setValue(PORT_AUDIO_INPUT_DEVICE_NAME_KEY, Val(configuration.midiInputDeviceName));
    settings()->setValue(PORT_AUDIO_OUTPUT_DEVICE_NAME_KEY, Val(configuration.midiOutputDeviceName));
    settings()->setValue(PORT_AUDIO_OUTPUT_LATENCY_KEY, Val(configuration.midiOutputLatencyMilliseconds));
}

unsigned int AudioConfiguration::driverBufferSize() const
{
    return settings()->value(AUDIO_BUFFER_SIZE).toInt();
}

std::vector<io::path> AudioConfiguration::soundFontPaths() const
{
    std::string pathsStr = settings()->value(USER_SOUNDFONTS_PATH).toString();
    std::vector<io::path> paths = io::path::pathsFromString(pathsStr, ";");
    paths.push_back(globalConfiguration()->sharePath());

    //! TODO Implement me
    // append extensions directory
    //QStringList extensionsDir = Ms::Extension::getDirectoriesByType(Ms::Extension::soundfontsDir);

    return paths;
}

const SynthesizerState& AudioConfiguration::defaultSynthesizerState() const
{
    static SynthesizerState state;
    if (state.isNull()) {
        SynthesizerState::Group gf;
        gf.name = "Fluid";
        gf.vals.push_back(SynthesizerState::Val(SynthesizerState::ValID::SoundFontID, DEFAULT_FLUID_SOUNDFONT));
        state.groups.insert({ gf.name, std::move(gf) });

        SynthesizerState::Group gz;
        gz.name = "Zerberus";
        gz.vals.push_back(SynthesizerState::Val(SynthesizerState::ValID::SoundFontID, DEFAULT_ZERBERUS_SOUNDFONT));
        state.groups.insert({ gz.name, std::move(gz) });
    }

    return state;
}

const SynthesizerState& AudioConfiguration::synthesizerState() const
{
    if (!m_state.isNull()) {
        return m_state;
    }

    bool ok = readState(stateFilePath(), m_state);
    if (!ok) {
        LOGW() << "failed read synthesizer state, file: " << stateFilePath();
        m_state = defaultSynthesizerState();
    }

    return m_state;
}

Ret AudioConfiguration::saveSynthesizerState(const SynthesizerState& state)
{
    std::list<std::string> changedGroups;
    for (auto it = m_state.groups.cbegin(); it != m_state.groups.cend(); ++it) {
        auto nit = state.groups.find(it->first);
        if (nit == state.groups.cend()) {
            continue;
        }

        if (it->second != nit->second) {
            changedGroups.push_back(it->first);
        }
    }

    Ret ret = writeState(stateFilePath(), state);
    if (!ret) {
        LOGE() << "failed write synthesizer state, file: " << stateFilePath();
        return ret;
    }

    m_state = state;
    m_synthesizerStateChanged.notify();
    for (const std::string& gname : changedGroups) {
        m_synthesizerStateGroupChanged[gname].notify();
    }

    return make_ret(Ret::Code::Ok);
}

async::Notification AudioConfiguration::synthesizerStateChanged() const
{
    return m_synthesizerStateChanged;
}

async::Notification AudioConfiguration::synthesizerStateGroupChanged(const std::string& gname) const
{
    return m_synthesizerStateGroupChanged[gname];
}

io::path AudioConfiguration::stateFilePath() const
{
    return globalConfiguration()->dataPath() + "/synthesizer.xml";
}

bool AudioConfiguration::readState(const io::path& path, SynthesizerState& state) const
{
    XmlReader xml(path);

    while (xml.canRead() && xml.success()) {
        XmlReader::TokenType token = xml.readNext();
        if (token == XmlReader::StartDocument) {
            continue;
        }

        if (token == XmlReader::StartElement) {
            if (xml.tagName() == "Synthesizer") {
                continue;
            }

            while (xml.tokenType() != XmlReader::EndElement) {
                SynthesizerState::Group group;
                group.name = xml.tagName();

                xml.readNext();

                while (xml.tokenType() != XmlReader::EndElement) {
                    if (xml.tokenType() == XmlReader::StartElement) {
                        if (xml.tagName() == "val") {
                            SynthesizerState::ValID id = static_cast<SynthesizerState::ValID>(xml.intAttribute("id"));
                            group.vals.push_back(SynthesizerState::Val(id, xml.readString()));
                        } else {
                            xml.skipCurrentElement();
                        }
                    }
                    xml.readNext();
                }

                state.groups.insert({ group.name, std::move(group) });
            }
        }
    }

    if (!xml.success()) {
        LOGE() << "failed parse xml, error: " << xml.error() << ", path: " << path;
    }

    return xml.success();
}

bool AudioConfiguration::writeState(const io::path& path, const SynthesizerState& state)
{
    XmlWriter xml(path);
    xml.writeStartDocument();

    xml.writeStartElement("Synthesizer");

    for (auto it = state.groups.cbegin(); it != state.groups.cend(); ++it) {
        const SynthesizerState::Group& group = it->second;

        if (group.name.empty()) {
            continue;
        }

        xml.writeStartElement(group.name);
        for (const SynthesizerState::Val& value : group.vals) {
            xml.writeStartElement("val");
            xml.writeAttribute("id", std::to_string(static_cast<int>(value.id)));
            xml.writeCharacters(value.val);
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }

    xml.writeEndElement();
    xml.writeEndDocument();

    if (!xml.success()) {
        LOGE() << "failed write xml";
    }

    return xml.success();
}
