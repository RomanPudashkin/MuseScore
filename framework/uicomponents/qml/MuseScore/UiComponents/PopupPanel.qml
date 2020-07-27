import QtQuick 2.9
import QtGraphicalEffects 1.15

import MuseScore.Ui 1.0

Rectangle {
    id: root

    property alias content: loader.sourceComponent

    signal close()

    height: loader.height + 84
    color: ui.theme.popupBackgroundColor
    border.width: 1
    border.color: ui.theme.strokeColor
    radius: 20

    anchors.bottomMargin: -20

    function setContentData(data) {
        if (loader.status === Loader.Ready) {
            loader.item.setData(data)
        }
    }

    function hide() {
        visible = false
    }

    Loader {
        id: loader

        anchors.left: parent.left
        anchors.leftMargin: 68
        anchors.right: parent.right
        anchors.rightMargin: 68
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 42

        height: sourceComponent.height
    }

    StyledIconLabel {
        anchors.top: parent.top
        anchors.topMargin: 20
        anchors.right: parent.right
        anchors.rightMargin: 20

        width: 32
        height: width

        font.pixelSize: 32

        iconCode: IconCode.CLOSE_X_ROUNDED

        MouseArea {
            anchors.fill: parent

            onClicked: {
                root.close()
            }
        }
    }
}
