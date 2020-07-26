import QtQuick 2.7
import QtQuick.Layouts 1.15

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Item {
    id: root

    property var extension: undefined

    height: contentColumn.height

    QtObject {
        id: prop

        property string _currentOperation: "undefined" // "install" "update" "uninstall" "openFullDescription"

        function _currentOperationButton() {
            var currentButton
            if (prop._currentOperation === "install") {
                currentButton = installButton
            } else if (prop._currentOperation === "update") {
                currentButton = updateButton
            }

             return currentButton
        }
    }

    function setData(data) {
        extension = data
    }

    function setProgress(status, indeterminate, current, total) {
        var currentButton = prop._currentOperationButton()

        currentButton.progressTitle = status
        currentButton.indeterminate = indeterminate
        if (!indeterminate) {
            currentButton.value = current
            currentButton.to = total
        }
    }

    function resetProgress() {
        var currentButton = prop._currentOperationButton()
        currentButton.indeterminate = false
    }

    signal install(string code)
    signal update(string code)
    signal uninstall(string code)
    signal openFullDescription(string code)

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

                onClicked: {
                    prop._currentOperation = "openFullDescription"
                    root.openFullDescription(extension.code)
                }
            }

            Row {
                Layout.alignment: Qt.AlignRight

                spacing: 19

                ProgressButton {
                    id: updateButton

                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Update available")

                    onClicked: {
                        prop._currentOperation = "update"
                        root.update(extension.code)
                    }
                }
                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.Installed ||
                                                   extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Restart extension")

                    onClicked: {
                        prop._currentOperation = "update"
//                        root.restart(extension.code) // TODO: restart
                    }
                }
                ProgressButton {
                    id: installButton

                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.NoInstalled) : false

                    text: qsTrc("extensions", "Install")

                    onClicked: {
                        prop._currentOperation = "install"
                        root.install(extension.code)
                    }
                }
                FlatButton {
                    visible: Boolean(extension) ? (extension.status === ExtensionStatus.Installed ||
                                                   extension.status === ExtensionStatus.NeedUpdate) : false

                    text: qsTrc("extensions", "Uninstall")

                    onClicked: {
                        prop._currentOperation = "uninstall"
                        root.uninstall(extension.code)
                    }
                }
            }
        }
    }
}
