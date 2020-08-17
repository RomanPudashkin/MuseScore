import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.UserScores 1.0

Column {
    id: root

    property string title: ""
    property string subtitle: ""
    property string composer: ""
    property string lyricist: ""
    property string copiright: ""

    spacing: 20

    Row {
        anchors.left: parent.left
        anchors.right: parent.right

        property real childWidth: (width / 2) - 15
        height: 60

        spacing: 20

        Column {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.childWidth
            spacing: 10

            StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right

                font.bold: true
                horizontalAlignment: Text.AlignLeft
                text: qsTrc("instruments", "Title")
            }

            TextInputField {
                id: title
                hint: qsTrc("instruments", "Optional")

                onCurrentTextEdited: {
                    root.title = newTextValue
                }
            }
        }

        Column {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.childWidth
            spacing: 10

            StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right

                font.bold: true
                horizontalAlignment: Text.AlignLeft
                text: qsTrc("instruments", "Composer")
            }

            TextInputField {
                id: composer
                hint: qsTrc("instruments", "Optional")

                onCurrentTextEdited: {
                    root.composer = newTextValue
                }
            }
        }
    }

    Row {
        anchors.left: parent.left
        anchors.right: parent.right

        property real childWidth: (width / 3) - 15
        height: 60

        spacing: 20

        Column {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.childWidth

            spacing: 10

            StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right

                font.bold: true
                horizontalAlignment: Text.AlignLeft
                text: qsTrc("instruments", "Subtitle")
            }

            TextInputField {
                id: subtitle
                hint: qsTrc("instruments", "Optional")

                onCurrentTextEdited: {
                    root.subtitle = newTextValue
                }
            }
        }

        Column {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.childWidth

            spacing: 10

            StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right

                font.bold: true
                horizontalAlignment: Text.AlignLeft
                text: qsTrc("instruments", "Lyricist")
            }

            TextInputField {
                id: lyricist
                hint: qsTrc("instruments", "Optional")

                onCurrentTextEdited: {
                    root.lyricist = newTextValue
                }
            }
        }

        Column {
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.childWidth

            spacing: 10

            StyledTextLabel {
                anchors.left: parent.left
                anchors.right: parent.right

                font.bold: true
                horizontalAlignment: Text.AlignLeft
                text: qsTrc("instruments", "Copiright")
            }

            TextInputField {
                id: copiright
                hint: qsTrc("instruments", "Optional")

                onCurrentTextEdited: {
                    root.copiright = newTextValue
                }
            }
        }
    }
}
