import QtQuick 2.7
import QtQuick.Controls 2.7
import QtQuick.Layouts 1.12

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Rectangle {
    id: root

    anchors.fill: parent

    color: ui.theme.backgroundColor

    StyledTextLabel {
        id: addonsLabel

        anchors.top: parent.top
        anchors.topMargin: 66
        anchors.left: parent.left
        anchors.leftMargin: 133

        font.pixelSize: 32
        font.bold: true

        text: qsTrc("appshell", "Add-ons")
    }

    TextInputField {
        id: search

        anchors.right: parent.horizontalCenter
        anchors.rightMargin: 2
        anchors.verticalCenter: addonsLabel.verticalCenter

        width: 184
        height: 30
    }

    TabBar {
        id: bar

        anchors.top: addonsLabel.bottom
        anchors.topMargin: 54
        anchors.horizontalCenter: parent.horizontalCenter

        StyledTabButton {
            text: qsTrc("appshell", "Plugins")
            sideMargin: 22
            isCurrent: bar.currentIndex === 0
        }
        StyledTabButton {
            text: qsTrc("appshell", "Extensions")
            sideMargin: 22
            isCurrent: bar.currentIndex === 1
        }
        StyledTabButton {
            text: qsTrc("appshell", "Languages")
            sideMargin: 22
            isCurrent: bar.currentIndex === 2
        }
    }

    StackLayout {
        anchors.top: bar.bottom
        anchors.topMargin: 24
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        currentIndex: bar.currentIndex

        Rectangle {
            id: pluginsComp
            color: ui.theme.backgroundColor
            StyledTextLabel {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Plugins Module"
            }
        }

        ExtensionsModule {
            id: extensionsComp

            Connections {
                target: search

                function onCurrentTextEdited(newTextValue) {
                    extensionsComp.search = newTextValue
                }
            }
        }

        Rectangle {
            id: languagesComp
            color: ui.theme.backgroundColor
            StyledTextLabel {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
                text: "Languages Module"
            }
        }
    }
}

