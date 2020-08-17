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

        font.family: "Bravura"
        font.pixelSize: 52
        text: model.timeSignature.numerator + "\n" +
              model.timeSignature.denominator
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

        implicitHeight: 240
        implicitWidth: 290

        arrowX: root.arrowX
        x: popupPositionX
        y: popupPositionY

        Column {
            anchors.fill: parent
            anchors.margins: 10

            spacing: 20

            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 30

                RadioButton {
                    id: timeFractionButton

                    checked: model.timeFraction
                    onCheckedChanged: {
                        model.timeFraction = checked

                        //TODO
                        if (checked) {
                            model.timeCut = false
                            model.timeCommon = false
                        }
                    }
                }

                Row {
                    Layout.fillWidth: true
                    spacing: 14

                    IncrementalPropertyControl {
                        id: control

                        implicitWidth: 80

                        enabled: timeFractionButton.checked

                        iconMode: iconModeEnum.hidden
                        currentValue: root.model.timeSignature.numerator
                        step: 1

                        maxValue: root.model.timeSignatureNumeratorRange().max
                        minValue: root.model.timeSignatureNumeratorRange().min
                        validator: IntInputValidator {
                            top: control.maxValue
                            bottom: control.minValue
                        }

                        onValueEdited: {
                            root.model.setTimeSignatureNumerator(newValue)
                        }
                    }

                    StyledTextLabel {
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 15
                        text: "/"
                    }

                    StyledComboBox {
                        width: 60

                        enabled: timeFractionButton.checked

                        textRoleName: "text"
                        valueRoleName: "value"

                        value: root.model.timeSignature.denominator

                        model: {
                            var resultList = []

                            var denominators = root.model.timeSignatureDenominators()

                            for (var i = 0; i < denominators.length; ++i) {
                                resultList.push({"text" : denominators[i], "value" : denominators[i]})
                            }

                            return resultList
                        }

                        onValueChanged: {
                            root.model.setTimeSignatureDenominator(value)
                        }
                    }
                }
            }

            RowLayout {
                spacing: 30

                RadioButton {
                    id: timeCommonButton

                    checked: model.timeCommon
                    onCheckedChanged: {
                        model.timeCommon = checked

                        //TODO
                        if (checked) {
                            model.timeCut = false
                            model.timeFraction = false
                        }
                    }
                }

                FontIcon {
                    width: 20
                    iconCode: IconCode.AMBITUS
                }
            }

            RowLayout {
                spacing: 30

                RadioButton {
                    id: timeCutButton
                    checked: model.timeCut
                    onCheckedChanged: {
                        model.timeCut = checked

                        // TODO
                        if (checked) {
                            model.timeCommon = false
                            model.timeFraction = false
                        }
                    }
                }
                FontIcon {
                    width: 20
                    iconCode: IconCode.AMBITUS
                }
            }
        }
    }
}
