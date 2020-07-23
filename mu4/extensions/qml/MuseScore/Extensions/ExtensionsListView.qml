import QtQuick 2.7

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Rectangle {
    id: root

    color: ui.theme.backgroundColor

    height: view.height

    property alias model: filterModel.sourceModel

    property alias search: filterModel.searchString
    property alias searchRoles: filterModel.searchRoles

    property alias filterRoles: filterModel.filterRoles
    property alias filtersValues: filterModel.filtersValues

    property int count: view.count

    signal install(string code)
    signal update(string code)
    signal uninstall(string code)

    FilterProxyModel {
        id: filterModel
    }

    GridView {
        id: view

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: contentHeight

        model: filterModel

        clip: true

        cellHeight: 150
        cellWidth: 200

        boundsBehavior: Flickable.StopAtBounds

        delegate: Item {
            id: item

            height: view.cellHeight
            width: view.cellWidth

            ExtensionItem {
                anchors.centerIn: parent

                height: 130
                width: 180

                name: model.name
                status: model.status
                code: model.code

                onInstall: {
                    root.install(code)
                }

                onUpdate: {
                    root.update(code)
                }

                onUninstall: {
                    root.uninstall(code)
                }
            }
        }
    }
}
