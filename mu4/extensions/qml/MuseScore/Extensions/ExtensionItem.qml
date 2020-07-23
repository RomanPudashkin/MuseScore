import QtQuick 2.7

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Rectangle {
    id: root

    color: ui.theme.popupBackgroundColor

    property var code: undefined
    property alias name: nameLabel.text
    property var status: undefined

    signal install(string code)
    signal update(string code)
    signal uninstall(string code)

    Column {
        anchors.fill: parent
        spacing: 10

        StyledTextLabel {
            id: nameLabel
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Row {
            anchors.left: parent.left
            anchors.right: parent.right

            FlatButton {
                text: qsTrc("extensions", "Install")
                width: 60

                visible: root.status === ExtensionStatus.NoInstalled

                onClicked: {
                    root.install(root.code)
                }
            }
            FlatButton {
                text: qsTrc("extensions", "Update")
                width: 60

                visible: root.status === ExtensionStatus.NeedUpdate

                onClicked: {
                    root.update(root.code)
                }
            }
            FlatButton {
                text: qsTrc("extensions", "Uninstall")
                width: 60

                visible: root.status === ExtensionStatus.Installed || status === ExtensionStatus.NeedUpdate

                onClicked: {
                    root.uninstall(root.code)
                }
            }
        }
    }
}
