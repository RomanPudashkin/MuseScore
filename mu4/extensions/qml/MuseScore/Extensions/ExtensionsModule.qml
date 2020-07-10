import QtQuick 2.7

import MuseScore.UiComponents 1.0
import MuseScore.Extensions 1.0

Rectangle {
    color: ui.theme.backgroundColor

    Component.onCompleted: {
        extensiomListModel.load()
    }

    ExtensionListModel {
        id: extensiomListModel
    }

    // TODO
    FlatButton {
        text: qsTrc("extensions", "Update")

        onClicked: {
            extensiomListModel.updateList()
        }
    }

    GridView {
        id: view

        anchors.fill: parent
        anchors.topMargin: 50

        model: extensiomListModel

        clip: true

        cellHeight: 150
        cellWidth: 200

        boundsBehavior: Flickable.StopAtBounds

        delegate: Item {
            height: view.cellHeight
            width: view.cellWidth

            Rectangle {

                anchors.centerIn: parent

                height: 130
                width: 180
                color: ui.theme.popupBackgroundColor

                StyledTextLabel {
                    anchors.fill: parent
                    text: name
                }

            }
        }
    }

}
