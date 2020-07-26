import QtQuick 2.7
import QtQuick.Layouts 1.15

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Item {
    id: root

    property var extension: undefined

    height: contentColumn.height

    function setData(data) {
        extension = data
    }

    Column {
        id: contentColumn

        anchors.left: parent ? parent.left : undefined
        anchors.right: parent ? parent.right : undefined

        spacing: 42

        Column {
            width: 585

            spacing: 8

            StyledTextLabel {
                font.pixelSize: 22
                font.bold: true
                text: Boolean(extension) ? extension.name : ""
            }

            Row {
                anchors.left: parent.left
                anchors.right: parent.right

                spacing: 4

                StyledTextLabel {
                    font.pixelSize: 14
                    text: qsTrc("extensions", "Author:")
                }
                StyledTextLabel {
                    font.pixelSize: 14
                    color: ui.theme.accentColor
                    text: qsTrc("extensions", "MuseScore") // TODO: get from model
                }

                Rectangle {
                    width: 2
                    height: parent.height - 4
                    anchors.verticalCenter: parent.verticalCenter
                    color: ui.theme.fontColor
                }

                StyledTextLabel {
                    font.pixelSize: 14
                    text: qsTrc("extensions", "Maintained by:")
                }
                StyledTextLabel {
                    font.pixelSize: 14
                    color: ui.theme.accentColor
                    text: qsTrc("extensions", "MuseScore") // TODO: get from model
                }
            }
        }

        StyledTextLabel {
            width: 585
            height: 88

            font.pixelSize: 12
            opacity: 0.75
            wrapMode: Text.WordWrap
            verticalAlignment: Text.AlignTop
            horizontalAlignment: Text.AlignLeft

            text: Boolean(extension) ? extension.description : ""
        }

        RowLayout {
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 19

            FlatButton {
                Layout.alignment: Qt.AlignLeft
                text: qsTrc("extensions", "View full description")
            }

            Row {
                Layout.alignment: Qt.AlignRight

                spacing: 19

                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Update available")
                }
                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.Installed ||
                                                   extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Restart extension")
                }
                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.NoInstalled) : false

                    text: qsTrc("extensions", "Install")
                }
                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.Installed ||
                                                   extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Uninstall")
                }
            }
        }
    }
}
