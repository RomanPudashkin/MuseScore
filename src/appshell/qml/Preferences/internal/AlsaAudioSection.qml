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

    QtObject {
        id: privateProperties

        readonly property int fieldWidth: 110
    }

    RoundedRadioButton {
        id: useAlsaAudioRadioButton

        implicitWidth: parent.width

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
            width: parent.width
            height: childrenRect.height

            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Device:")
            }

            TextInputField {
                width: privateProperties.fieldWidth

                currentText: root.configuration.deviceName
            }
        }

        Row {
            width: parent.width
            height: childrenRect.height

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
                    width: privateProperties.fieldWidth

                    currentIndex: root.configuration.sampleRateIndex
                }

                StyledTextLabel {
                    anchors.verticalCenter: parent.verticalCenter

                    text: qsTrc("appshell", "Hz")
                }
            }
        }

        Row {
            width: parent.width
            height: childrenRect.height

            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Fragments:")
            }

            IncrementalPropertyControl {
                width: 75

                currentValue: root.configuration.fragmentsNumber
            }
        }

        Row {
            width: parent.width
            height: childrenRect.height

            spacing: 0

            StyledTextLabel {
                anchors.verticalCenter: parent.verticalCenter

                width: root.firstColumnWidth
                horizontalAlignment: Qt.AlignLeft

                text: qsTrc("appshell", "Period size:")
            }

            StyledComboBox {
                width: privateProperties.fieldWidth

                currentIndex: root.configuration.periodSizeIndex
            }
        }
    }
}
