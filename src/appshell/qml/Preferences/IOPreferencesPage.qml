import QtQuick 2.15

import MuseScore.UiComponents 1.0
import MuseScore.Preferences 1.0

import "internal"

PreferencesPage {
    id: root

    contentHeight: content.height

    IOPreferencesModel {
        id: ioModel
    }

    Column {
        id: content

        width: parent.width
        spacing: 24

        property int firstColumnWidth: 220

        RoundedRadioButton {
            id: pulseAudioSection

            width: parent.width

            visible: ioModel.isPulseAudioAvailable()
            checked: ioModel.isPulseAudioUsed

            text: qsTrc("appshell", "PulseAudio")
            font: ui.theme.bodyBoldFont

            onClicked: {
                ioModel.setUsedAudioSystem("PulseAudio")
            }
        }

        SeparatorLine { visible: pulseAudioSection.visible }

        AlsaAudioSection {
            id: alsaAudioSection

            visible: ioModel.isAlsaAudioAvailable()
            useAlsaAudio: ioModel.isAlsaAudioUsed
            configuration: ioModel.alsaAudioConfiguration

            firstColumnWidth: content.firstColumnWidth

            onUseAlsaAudioRequsted: {
                ioModel.setUsedAudioSystem("AlsaAudio")
            }

            onConfigurationChangeRequested: {
                ioModel.alsaAudioConfiguration = newConfiguration
            }
        }

        SeparatorLine { visible: alsaAudioSection.visible }

        JackAudioServerSection {
            id: jackAudioServerSection

            visible: ioModel.isJackAudioServerAvailable()
            useJackAudioServer: ioModel.isJaskAudioServerUsed
            configuration: ioModel.jackAudioServerConfiguration

            onUseJackAudioServerRequested: {
                ioModel.setUsedAudioSystem("JackAudioServer")
            }

            onConfigurationChangeRequsted: {
                ioModel.jackAudioServerConfiguration = newConfiguration
            }
        }

        SeparatorLine { visible: jackAudioServerSection.visible }

        AudioEngineSection {
            onRestartAudioAndMidiDevicesRequested: {
                ioModel.restartAudioAndMidiDevices()
            }
        }
    }
}
