import QtQuick 2.15
import QtQuick.Layouts 1.12

import MuseScore.UiComponents 1.0
import MuseScore.Preferences 1.0

Column {
    id: root

    property alias useJackAudioServer: useJackAudioServerRadioButton.checked
    property var configuration

    signal useJackAudioServerRequested()
    signal configurationChangeRequsted(var newConfiguration)

    width: parent.width
    spacing: 22

    RoundedRadioButton {
        id: useJackAudioServerRadioButton

        implicitWidth: parent.width

        StyledTextLabel {
            text: qsTrc("appshell", "JACK Audio Server")
            font: ui.theme.bodyBoldFont

            horizontalAlignment: Qt.AlignLeft
        }

        onClicked: {
            root.useJackAudioServerRequested()
        }
    }

    GridLayout {
        columns: 3

        rowSpacing: 22
        columnSpacing: 100

        Repeater {
            anchors.fill: parent

            model: [
                { text: qsTrc("appshell", "Use JACK audio"), value: root.configuration.useJackAudio },
                { text: qsTrc("appshell", "Use JACK MIDI"), value: root.configuration.useJackMidi },
                { text: qsTrc("appshell", "Remember last connection(s)"), value: root.configuration.rememberLastConnections },
                { text: qsTrc("appshell", "Use JACK transport"), value: root.configuration.useJackTransport },
                { text: qsTrc("appshell", "Timebase master"), value: root.configuration.useTimeBaseMaster }
            ]

            CheckBox {
                implicitHeight: 20
                implicitWidth: 110

                text: modelData.text
                checked: modelData.value

                onClicked: {
                    var newConfiguration = root.configuration
                    var newValue = !checked

                    if (model.index === 0) {
                        newConfiguration.useJackAudio = newValue
                    } else if (model.index === 1) {
                        newConfiguration.useJackMidi = newValue
                    } else if (model.index === 2) {
                        newConfiguration.rememberLastConnections = newValue
                    } else if (model.index === 3) {
                        newConfiguration.useJackTransport = newValue
                    } else if (model.index === 4) {
                        newConfiguration.useTimeBaseMaster = newValue
                    }

                    root.configurationChangeRequsted(newConfiguration)
                }
            }
        }
    }
}
