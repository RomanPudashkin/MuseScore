import QtQuick 2.15

import MuseScore.Ui 1.0

Rectangle {
    id: root

    anchors.fill: parent

    color: ui.theme.strokeColor

    readonly property QtObject separatorCpp: parent

    MouseArea {
        anchors.fill: parent
        anchors.margins: -5 //! NOTE: extra space for user convenience

        cursorShape: separatorCpp ? (separatorCpp.isVertical ? Qt.SizeVerCursor : Qt.SizeHorCursor)
                                   : Qt.SizeHorCursor
        onPressed: {
            separatorCpp.onMousePressed()
        }

        onReleased: {
            separatorCpp.onMouseReleased()
        }

        onPositionChanged: {
            separatorCpp.onMouseMoved(Qt.point(mouse.x, mouse.y))
        }

        onDoubleClicked: {
            separatorCpp.onMouseDoubleClicked()
        }
    }
}
