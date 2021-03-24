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

        width: parent.width

        text: qsTrc("appshell", "JACK Audio Server")
        font: ui.theme.bodyBoldFont

        onClicked: {
            root.useJackAudioServerRequested()
        }
    }

    GridLayout {
        enabled: root.useJackAudioServer

        columns: 3
        rowSpacing: 22
        columnSpacing: 100

        Repeater {
            model: [
                {
                    text: qsTrc("appshell", "Use JACK audio"),
                    value: root.configuration.useJackAudio,
                    name: "useJackAudio"
                },
                {
                    text: qsTrc("appshell", "Use JACK MIDI"),
                    value: root.configuration.useJackMidi,
                    name: "useJackMidi"
                },
                {
                    text: qsTrc("appshell", "Remember last connection(s)"),
                    value: root.configuration.rememberLastConnections,
                    name: "rememberLastConnections"
                },
                {
                    text: qsTrc("appshell", "Use JACK transport"),
                    value: root.configuration.useJackTransport,
                    name: "useJackTransport"
                },
                {
                    text: qsTrc("appshell", "Timebase master"),
                    value: root.configuration.useTimeBaseMaster,
                    name: "useTimeBaseMaster"
                }
            ]

            CheckBox {
                implicitHeight: 20
                implicitWidth: 110

                text: modelData.text
                checked: modelData.value

                onClicked: {
                    root.configuration[modelData.name] = !checked
                    root.configurationChangeRequsted(root.configuration)
                }
            }
        }
    }
}
