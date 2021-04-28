import QtQuick 2.15
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.Dock 1.0 as Dock

Dock.DockToolBar {
    id: root

    property Component contentComponent

    property bool floating: false

    Rectangle {
        anchors.fill: parent

        color: ui.theme.backgroundPrimaryColor

        Loader {
            id: loader

            anchors.fill: parent
            anchors.margins: 2

            sourceComponent: orientation === Qt.Horizontal ? horizontalView : verticalView

            onLoaded: {
                root.setDraggableMouseArea(loader.item.gripMouseArea)
            }
        }
    }

    Component {
        id: horizontalView

        RowLayout {
            spacing: 2

            property var gripMouseArea: gripButton.mouseArea

            FlatButton {
                id: gripButton

                Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter

                mouseArea.objectName: root.objectName + "_toolBarMouseAreaHorizontal"

                normalStateColor: "transparent"
                icon: IconCode.TOOLBAR_GRIP

                visible: root.movable
            }

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                sourceComponent: root.contentComponent
            }
        }
    }

    Component {
        id: verticalView

        ColumnLayout {
            spacing: 2

            property var gripMouseArea: gripButton.mouseArea

            FlatButton {
                id: gripButton

                Layout.alignment: Qt.AlignTop | Qt.AlignHCenter

                mouseArea.objectName: root.objectName + "_toolBarMouseAreaVertical"

                normalStateColor: "transparent"
                icon: IconCode.TOOLBAR_GRIP
                rotation: 90

                visible: root.movable
            }

            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true

                sourceComponent: root.contentComponent
            }
        }
    }
}
