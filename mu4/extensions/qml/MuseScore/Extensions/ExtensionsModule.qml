import QtQuick 2.7

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Item {
    id: root

    property string search: ""

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

    Flickable {

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

            spacing: 8
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            Rectangle {
                height: label.height + view.height + 6
                width: parent.width

                color: ui.theme.backgroundColor

                visible: view.count > 0

                StyledTextLabel {
                    id: label
                    height: 20
                    text: qsTrc("extensions", "Installed")
                }

                ExtensionsListView {
                    id: view
                    anchors.top: label.bottom
                    anchors.topMargin: 6
                    width: parent.width

                    model: extensiomListModel

                    search: root.search
                    searchRoles: ["name"]

                    filterRoles: ["status"]
                    filtersValues: [ExtensionStatus.Installed]

                    onInstall: {
                        extensiomListModel.install(code)
                    }

                    onUpdate: {
                        extensiomListModel.update(code)
                    }

                    onUninstall: {
                        extensiomListModel.uninstall(code)
                    }

                }
            }

            Rectangle {
                height: label2.height + view2.height + 6
                width: parent.width

                color: ui.theme.backgroundColor

                visible: view2.count > 0

                StyledTextLabel {
                    id: label2
                    height: 20
                    text: qsTrc("extensions", "Not Installed")
                }

                ExtensionsListView {
                    id: view2
                    anchors.top: label2.bottom
                    anchors.topMargin: 6
                    width: parent.width

                    model: extensiomListModel

                    search: root.search
                    searchRoles: ["name"]

                    filterRoles: ["status"]
                    filtersValues: [ExtensionStatus.NoInstalled]

                    onInstall: {
                        extensiomListModel.install(code)
                    }

                    onUpdate: {
                        extensiomListModel.update(code)
                    }

                    onUninstall: {
                        extensiomListModel.uninstall(code)
                    }
                }
            }
        }
    }
}
