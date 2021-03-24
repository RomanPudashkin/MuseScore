import QtQuick 2.15
import QtQuick.Layouts 1.12

import MuseScore.UiComponents 1.0
import MuseScore.Preferences 1.0

Column {
    id: root

    property alias useAlsaAudio: useAlsaAudioRadioButton.checked
    property var configuration

    property int firstColumnWidth: 0

    signal useAlsaAudioRequsted()
    signal configurationChangeRequested(var newConfiguration)

    width: parent.width
    spacing: 22

    RoundedRadioButton {
        id: useAlsaAudioRadioButton

        width: parent.width

        text: qsTrc("appshell", "ALSA Audio")
        font: ui.theme.bodyBoldFont

        onClicked: {
            root.useAlsaAudioRequsted()
        }
    }

    Column {
        width: parent.width
        spacing: 4

        enabled: root.useAlsaAudio

        Row {
            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Device:")
            }

            TextInputField {
                id: deviceNameField

                width: 110

                currentText: root.configuration.deviceName

                onCurrentTextEdited: {
                    root.configuration.deviceName = newTextValue
                    root.configurationChangeRequested(root.configuration)
                }
            }
        }

        Row {
            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "SampleRate:")
            }

            Row {
                spacing: 4

                StyledComboBox {
                    width: deviceNameField.width

                    currentIndex: indexOfValue(root.configuration.sampleRateHz)
                    model: root.configuration.availableSampleRateList

                    onAccepted: {
                        root.configuration.sampleRateHz = currentValue
                        root.configurationChangeRequested(root.configuration)
                    }
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTrc("appshell", "Hz")
                }
            }
        }

        Row {
            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Fragments:")
            }

            IncrementalPropertyControl {
                id: fragmentsNumberField

                width: 80

                currentValue: root.configuration.fragmentsNumber
                decimals: 0
                step: 1

                onValueEdited: {
                    root.configuration.fragmentsNumber = newValue
                    root.configurationChangeRequested(root.configuration)
                }
            }
        }

        Row {
            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Period size:")
            }

            StyledComboBox {
                width: fragmentsNumberField.width

                currentIndex: indexOfValue(root.configuration.periodSize)
                model: root.configuration.availablePeriodSizeList

                onAccepted: {
                    root.configuration.periodSize = currentValue
                    root.configurationChangeRequested(root.configuration)
                }
            }
        }
    }
}
