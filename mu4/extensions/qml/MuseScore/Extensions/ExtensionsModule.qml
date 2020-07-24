import QtQuick 2.7

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Item {
    id: root

    property string search: ""
    property string selectedExtensionViewType: "undefined" // "installed" "notinstalled"
    property int selectedExtensionIndex: -1

    Component.onCompleted: {
        extensiomListModel.load()
    }

    ExtensionListModel {
        id: extensiomListModel
    }

    FlatButton {
        anchors.right: parent.right

        text: qsTrc("extensions", "Update")

        onClicked: {
            extensiomListModel.updateList()
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: flickable.top

        height: 8
        z: 1

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: ui.theme.backgroundColor
            }
            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }
    }

    Flickable {
        id: flickable

        anchors.fill: parent
        anchors.topMargin: 5
        anchors.leftMargin: 133
        anchors.rightMargin: 133

        clip: true

        contentWidth: width
        contentHeight: extensionsColumn.height
        interactive: height < contentHeight

        Column {
            id: extensionsColumn

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 20

            Rectangle {
                height: installedLabel.height + installedView.height + 6
                width: parent.width

                color: ui.theme.backgroundColor

                visible: installedView.count > 0

                StyledTextLabel {
                    id: installedLabel
                    height: 18
                    font.bold: true
                    text: qsTrc("extensions", "Installed")
                }

                ExtensionsListView {
                    id: installedView

                    anchors.top: installedLabel.bottom
                    anchors.topMargin: 12

                    anchors.left: parent.left
                    anchors.leftMargin: -24
                    anchors.right: parent.right
                    anchors.rightMargin: -24

                    model: extensiomListModel

                    selectedIndex: {
                        return selectedExtensionViewType === "installed" ? selectedExtensionIndex : -1
                    }

                    search: root.search
                    searchRoles: ["name"]

                    filterRoles: ["status"]
                    filtersValues: [ExtensionStatus.Installed]

                    onClicked: {
                        selectedExtensionViewType = "installed"
                        selectedExtensionIndex = index
                    }
                }
            }

            Rectangle {
                height: notInstalledLabel.height + notInstalledView.height + 6
                width: parent.width

                color: ui.theme.backgroundColor

                visible: notInstalledView.count > 0

                StyledTextLabel {
                    id: notInstalledLabel
                    height: 18
                    font.bold: true
                    text: qsTrc("extensions", "Not Installed")
                }

                ExtensionsListView {
                    id: notInstalledView

                    anchors.top: notInstalledLabel.bottom
                    anchors.topMargin: 12

                    anchors.left: parent.left
                    anchors.leftMargin: -24
                    anchors.right: parent.right
                    anchors.rightMargin: -24

                    model: extensiomListModel

                    selectedIndex: {
                        return selectedExtensionViewType === "notinstalled" ? selectedExtensionIndex : -1
                    }

                    search: root.search
                    searchRoles: ["name"]

                    filterRoles: ["status"]
                    filtersValues: [ExtensionStatus.NoInstalled]

                    onClicked: {
                        selectedExtensionViewType = "notinstalled"
                        selectedExtensionIndex = index
                    }
                }
            }
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: flickable.bottom

        height: 8
        z:1

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "transparent"
            }
            GradientStop {
                position: 1.0
                color: ui.theme.backgroundColor
            }
        }
    }
}
