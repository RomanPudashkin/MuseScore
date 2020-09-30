import QtQuick 2.9

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

StyledPopup {
    id: root

    property string elementName: ""
    property bool drawStaff: false
    property real offsetByX: 0
    property real offsetByY: 0
    property real scaleFactor: 0

    height: contentColumn.implicitHeight + topPadding + bottomPadding
    width: parent.width

    Column {
        id: contentColumn

        anchors.fill: parent
        anchors.margins: 12

        spacing: 12

        StyledTextLabel {
            text: qsTrc("palette", "Name")
            font.bold: true
        }

        TextInputField {
            currentText: root.elementName

            onCurrentTextEdited: {
                root.elementName = newTextValue
            }
        }

        SeparatorLine {}

        StyledTextLabel {
            text: qsTrc("palette", "Content offset")
            font.bold: true
        }

        Grid {
            width: parent.width

            columns: 2
            spacing: 16

            Repeater {
                id: repeater

                model: [
                    { title: qsTrc("palette", "X"), value: root.offsetByX, incrementStep: 1, measureUnit: qsTrc("palette", "sp") },
                    { title: qsTrc("palette", "Y"), value: root.offsetByY, incrementStep: 1, measureUnit: qsTrc("palette", "sp") },
                    { title: qsTrc("palette", "Content scale"), value: root.scaleFactor, incrementStep: 0.1 }
                ]

                function setValue(index, value) {
                    if (index === 0) {
                        root.offsetByX = value
                    } else if (index === 1) {
                        root.offsetByY = value
                    } else if (index === 2) {
                        root.scaleFactor = value
                    }
                }

                Column {
                    width: parent.width / 2 - 8

                    spacing: 8

                    StyledTextLabel {
                        id: titleLabel

                        text: modelData["title"]
                    }

                    IncrementalPropertyControl {
                        id: valueField

                        currentValue: modelData["value"]
                        measureUnitsSymbol: Boolean(modelData["measureUnit"]) ? modelData["measureUnit"] : ""
                        step: modelData["incrementStep"]

                        onValueEdited: {
                            repeater.setValue(model.index, newValue)
                        }
                    }
                }
            }
        }

        CheckBox {
            text: qsTrc("palette", "Draw stave")

            checked: root.drawStaff

            onClicked: {
                root.drawStaff = !checked
            }
        }

        Item { height: 1; width: parent.width }

        Row {
            width: parent.width
            height: childrenRect.height + 20

            spacing: 4

            FlatButton {
                text: qsTrc("palette", "Cancel")

                width: parent.width / 2

                onClicked: {
                    root.close()
                }
            }

            FlatButton {
                text: qsTrc("palette", "Ok")

                width: parent.width / 2

                onClicked: {
                    root.apply()
                    root.close()
                }
            }
        }
    }
}
