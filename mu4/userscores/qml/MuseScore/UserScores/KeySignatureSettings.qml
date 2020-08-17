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

    Column {
        anchors.horizontalCenter: root.horizontalCenter
        anchors.verticalCenter: root.verticalCenter

        spacing: 10

        FontIcon {
            anchors.horizontalCenter: parent.horizontalCenter
            height: 50

            pixelSize: 65
            iconCode: model.keySignature.icon
        }

        StyledTextLabel {
            text: model.keySignature.title
        }
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
        implicitWidth: 724

        arrowX: root.arrowX
        x: popupPositionX
        y: popupPositionY

        TabBar {
            id: bar

            anchors.top: parent.top
            anchors.topMargin: 10
            anchors.horizontalCenter: parent.horizontalCenter

            contentHeight: 28
            spacing: 0

            StyledTabButton {
                text: qsTrc("userscores", "Major")
                sideMargin: 22
                isCurrent: bar.currentIndex === 0
            }
            StyledTabButton {
                text: qsTrc("appshell", "Minor")
                sideMargin: 22
                isCurrent: bar.currentIndex === 1
            }
        }

        StackLayout {
            id: pagesStack

            anchors.top: bar.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 20

            currentIndex: bar.currentIndex

            KeySignatureListView {
                model: root.model.keySignatureMajorList()
                currentSignature: root.model.keySignature

                onSignatureSelected: {
                    root.model.keySignature = signature
                    popup.close()
                }
            }

            KeySignatureListView {
                model: root.model.keySignatureMinorList()
                currentSignature: root.model.keySignature

                onSignatureSelected: {
                    root.model.keySignature = signature
                    popup.close()
                }
            }
        }
    }
}
