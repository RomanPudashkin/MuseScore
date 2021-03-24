import QtQuick 2.15
import QtQuick.Layouts 1.12

import MuseScore.UiComponents 1.0

ColumnLayout {
    id: root

    property alias usePortAudio: usePortAudioRadioButton.checked
    property var configuration

    property int firstColumnWidth: 0

    signal usePortAudioRequsted()
    signal configurationChangeRequested(var newConfiguration)

    width: parent.width
    spacing: 0

    RoundedRadioButton {
        id: usePortAudioRadioButton

        Layout.fillWidth: true

        text: qsTrc("appshell", "PortAudio")
        font: ui.theme.bodyBoldFont

        onClicked: {
            root.usePortAudioRequsted()
        }
    }

    ListView {
        Layout.topMargin: 18

        Layout.fillWidth: true
        Layout.preferredHeight: contentHeight

        spacing: 4
        interactive: false
        enabled: root.usePortAudio

        model: [
            {
                text: qsTrc("appshell", "API"),
                value: root.configuration.apiName,
                name: "apiName",
                allValues: root.configuration.availableApiList
            },
            {
                text: qsTrc("appshell", "Device"),
                value: root.configuration.deviceName,
                name: "deviceName",
                allValues: root.configuration.availableDeviceList
            },
            {
                text: qsTrc("appshell", "MIDI input"),
                value: root.configuration.midiInput,
                name: "midiInput",
                allValues: root.configuration.availableMidiInputList
            },
            {
                text: qsTrc("appshell", "MIDI output"),
                value: root.configuration.midiOutput,
                name: "midiOutput",
                allValues: root.configuration.availableMidiOutputList
            }
        ]

        delegate: Row {
            width: ListView.view.width

            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: modelData.text + ":"
            }

            StyledComboBox {
                width: 210

                currentIndex: indexOfValue(modelData.value)
                model: modelData.allValues

                onAccepted: {
                    root.configuration[modelData.name] = currentValue
                    root.configurationChangeRequested(root.configuration)
                }
            }
        }
    }

    Row {
        Layout.topMargin: 4

        spacing: 0
        enabled: root.usePortAudio

        StyledTextLabel {
            anchors.verticalCenter: parent.verticalCenter

            width: root.firstColumnWidth
            horizontalAlignment: Qt.AlignLeft

            text: qsTrc("appshell", "MIDI output latency:")
        }

        IncrementalPropertyControl {
            width: 75

            measureUnitsSymbol: qsTrc("appshell", "ms")
            step: 1

            currentValue: root.configuration.midiOutputLatencyMilliseconds

            onValueEdited: {
                root.configuration.midiOutputLatencyMilliseconds = newValue
                root.configurationChangeRequested(root.configuration)
            }
        }
    }
}
