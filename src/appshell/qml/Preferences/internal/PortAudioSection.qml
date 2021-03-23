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

        implicitWidth: parent.width

        StyledTextLabel {
            text: qsTrc("appshell", "PortAudio")
            font: ui.theme.bodyBoldFont

            horizontalAlignment: Qt.AlignLeft
        }

        onClicked: {
            root.usePortAudioRequsted()
        }
    }

    ListView {
        Layout.topMargin: 18

        width: parent.width
        height: contentHeight

        spacing: 4
        interactive: false

        model: [
            { text: qsTrc("appshell", "API"), currentIndex: root.configuration.apiIndex, possibleValues: root.configuration.availableApiList },
            { text: qsTrc("appshell", "Device"), currentIndex: root.configuration.deviceIndex, possibleValues: root.configuration.availableDeviceList },
            { text: qsTrc("appshell", "MIDI input"), currentIndex: root.configuration.midiInputIndex, possibleValues: root.configuration.availableMidiInputList },
            { text: qsTrc("appshell", "MIDI output"), currentIndex: root.configuration.midiOutputIndex, possibleValues: root.configuration.availableMidiOutputList }
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

                currentIndex: modelData.currentIndex
                model: modelData.possibleValues

                onValueChanged: {
                    var newConfiguration = root.configuration

                    if (model.index === 0) {
                        newConfiguration.apiIndex = currentIndex
                    } else if (model.index === 1) {
                        newConfiguration.deviceIndex = currentIndex
                    } else if (model.index === 2) {
                        newConfiguration.midiInputIndex = currentIndex
                    } else if (model.index === 3) {
                        newConfiguration.midiOutputIndex = currentIndex
                    }

                    root.configurationChangeRequested(newConfiguration)
                }
            }
        }
    }

    Row {
        Layout.topMargin: 4

        spacing: 0

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
                var newConfiguration = root.configuration
                newConfiguration.midiOutputLatencyMilliseconds = newValue
                root.configurationChangeRequested(newConfiguration)
            }
        }
    }
}
