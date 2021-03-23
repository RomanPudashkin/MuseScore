import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.UiComponents 1.0
import MuseScore.Preferences 1.0

import "internal"

Flickable {
    id: root

    contentWidth: width
    contentHeight: content.height

    clip: true
    boundsBehavior: Flickable.StopAtBounds
    interactive: height < contentHeight

    ScrollBar.vertical: StyledScrollBar {}

    IOPreferencesModel {
        id: ioModel
    }

    Column {
        id: content

        width: parent.width
        height: childrenRect.height

        spacing: 24

        property int firstColumnWidth: 220

        RoundedRadioButton {
            width: parent.width

            checked: ioModel.usePulseAudio
            visible: ioModel.isPulseAudioAvailable()

            StyledTextLabel {
                text: qsTrc("appshell", "PulseAudio")
                font: ui.theme.bodyBoldFont

                horizontalAlignment: Qt.AlignLeft
            }

            onClicked: {
                ioModel.usePulseAudio = !checked
            }
        }

        SeparatorLine { visible: ioModel.isPulseAudioAvailable() }

        PortAudioSection {
            usePortAudio: ioModel.usePortAudio
            configuration: ioModel.portAudioConfiguration
            firstColumnWidth: content.firstColumnWidth

            onConfigurationChangeRequested: {
                ioModel.portAudioConfiguration = newConfiguration
            }
        }

        SeparatorLine {}

        AlsaAudioSection {
            useAlsaAudio: ioModel.useAlsaAudio
            configuration: ioModel.alsaAudioConfiguration
            firstColumnWidth: content.firstColumnWidth

            visible: ioModel.isAlsaAudioAvailable()

            onConfigurationChangeRequested: {
                ioModel.alsaAudioConfiguration = newConfiguration
            }
        }

        SeparatorLine { visible: ioModel.isAlsaAudioAvailable() }

        JackAudioServerSection {
            useJackAudioServer: ioModel.useJaskAudioServer
            configuration: ioModel.jackAudioServerConfiguration

            onConfigurationChangeRequsted: {
                ioModel.jackAudioServerConfiguration = newConfiguration
            }
        }

        SeparatorLine {}

        AudioEngineSection {
            onRestartAudioAndMidiDevicesRequested: ioModel.restartAudioAndMidiDevices()
        }
    }
}
