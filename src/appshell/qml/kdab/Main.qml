import QtQuick 2.15
import QtQuick.Window 2.15

import MuseScore.UiComponents 1.0

AppWindow {
    id: root

    flags: Qt.FramelessWindowHint | Qt.Window
    color: "transparent"

    property int sideMargin: root.visibility === Window.Windowed ? 8 : 0

    function toggleMaximized() {
        if (root.visibility === Window.Maximized) {
            root.showNormal()
        } else {
            root.showMaximized()
        }
    }

    DragHandler {
        id: resizeHandler

        grabPermissions: TapHandler.TakeOverForbidden
        target: null

        onActiveChanged: {
            if (!active) {
                return
            }

            const p = resizeHandler.centroid.position
            let e = 0
            if (p.x / width < 0.10) { e |= Qt.LeftEdge }
            if (p.x / width > 0.90) { e |= Qt.RightEdge }
            if (p.y / height < 0.10) { e |= Qt.TopEdge }
            if (p.y / height > 0.90) { e |= Qt.BottomEdge }
            root.startSystemResize(e)
        }
    }

    Rectangle {
        id: content

        anchors.fill: parent

        anchors.margins: root.sideMargin

        MainMenu {
            id: mainMenu

            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: 48

            onShowWindowMinimizedRequested: {
                root.showMinimized()
            }

            onToggleWindowMaximizedRequested: {
                root.toggleMaximized()
            }

            onCloseWindowRequested: {
                root.close()
            }

            onStartSystemMoveRequested: {
                root.startSystemMove()
            }
        }

        WindowContent {
            id: window

            anchors.top: mainMenu.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    StyledDropShadow {
        anchors.fill: content
        source: content
        samples: 20
    }
}
