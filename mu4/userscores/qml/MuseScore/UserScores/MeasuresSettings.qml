import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.UserScores 1.0

FlatButton {
    id: root

    height: 96

    property var model: null

    property var arrowX
    property var popupPositionX
    property var popupPositionY: height
    property alias oppened: popup.visible

    StyledTextLabel {
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter

        font.pixelSize: 16
        text: model.measureCount + qsTrc("userscores", " measures,\n Pickup: ") +
              model.pickupTimeSignature.numerator + "/" + model.pickupTimeSignature.denominator
    }

    onClicked: {
        if (!popup.isOpened) {
            popup.open()
        } else {
            popup.close()
        }
    }

    StyledPopup {
        id: popup

        implicitHeight: 310
        implicitWidth: 320

        arrowX: root.arrowX
        x: popupPositionX
        y: popupPositionY

        Column {
            anchors.fill: parent
            anchors.margins: 10

            spacing: 30

            Column {
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 14

                CheckBox {
                    id: withPickupMeasure

                    anchors.left: parent.left
                    anchors.right: parent.right

                    checked: root.model.withPickupMeasure

                    text: qsTrc("userscores", "Show pickup measure")

                    onClicked: {
                        root.model.withPickupMeasure = !checked
                    }
                }

                Row {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    spacing: 14

                    IncrementalPropertyControl {
                        id: control

                        implicitWidth: 80

                        enabled: withPickupMeasure.checked

                        iconMode: iconModeEnum.hidden
                        currentValue: root.model.pickupTimeSignature.numerator
                        step: 1

                        maxValue: root.model.timeSignatureNumeratorRange().max // TODO
                        minValue: root.model.timeSignatureNumeratorRange().min // TODO
                        validator: IntInputValidator {
                            top: control.maxValue
                            bottom: control.minValue
                        }

                        onValueEdited: {
                            root.model.setPickupTimeSignatureNumerator(newValue)
                        }
                    }

                    StyledTextLabel {
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 15
                        text: "/"
                    }

                    StyledComboBox {
                        width: 60

                        enabled: withPickupMeasure.checked

                        textRoleName: "text"
                        valueRoleName: "value"

                        value: root.model.pickupTimeSignature.denominator

                        model: {
                            var resultList = []

                            var denominators = root.model.timeSignatureDenominators() // TODO

                            for (var i = 0; i < denominators.length; ++i) {
                                resultList.push({"text" : denominators[i], "value" : denominators[i]})
                            }

                            return resultList
                        }

                        onValueChanged: {
                            root.model.setPickupTimeSignatureDenominator(value)
                        }
                    }
                }

            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right

                height: 2

                color: ui.theme.buttonColor
            }


            Column {
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 14

                StyledTextLabel {
                    horizontalAlignment: Text.AlignLeft
                    text: qsTrc("userscores", "Initial number of measures")
                }

                IncrementalPropertyControl {
                    id: measuresCountControl

                    implicitWidth: 80

                    enabled: withPickupMeasure.checked

                    iconMode: iconModeEnum.hidden
                    currentValue: root.model.measureCount
                    step: 1

                    maxValue: root.model.measureCountRange().max
                    minValue: root.model.measureCountRange().min
                    validator: IntInputValidator {
                        top: measuresCountControl.maxValue
                        bottom: measuresCountControl.minValue
                    }

                    onValueEdited: {
                        root.model.measureCount = newValue
                    }
                }

                StyledTextLabel {
                    anchors.left: parent.left
                    anchors.right: parent.right

                    horizontalAlignment: Text.AlignLeft
                    text: qsTrc("userscores", "Hint: You can also add & delete measures after you have created your score")
                    wrapMode: Text.WordWrap
                    maximumLineCount: 2
                }
            }
        }
    }
}
