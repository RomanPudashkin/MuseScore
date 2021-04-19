import QtQuick 2.15
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.Dock 1.0 as Dock

Dock.DockToolBar {
    id: root

    property Component contentComponent

    property bool movable: true
    property bool floating: false

    Component.onCompleted: {
        toolBarGripModel.setToolBarWidget(root.dockWidget)
    }

    Rectangle {
        anchors.fill: parent

        color: ui.theme.backgroundPrimaryColor

        QtObject {
            id: privateProperties

            readonly property int gripWidth: 32
            readonly property int gripHeight: 36
        }

        Dock.ToolBarGripModel {
            id: toolBarGripModel
        }

        Loader {
            id: loader

            anchors.fill: parent

            sourceComponent: orientation === Qt.Horizontal ? horizontalView : verticalView

            onLoaded: {
                toolBarGripModel.gripMouseArea = loader.item.gripMouseArea;
            }
        }
    }

    Component {
        id: horizontalView

        RowLayout {
            spacing: 0

            property var gripMouseArea: gripButton.mouseArea

            FlatButton {
                id: gripButton

                mouseArea.objectName: root.uniqueName + "toolBarMouseAreaHorizontal"

                normalStateColor: "transparent"
                Layout.alignment: Qt.AlignLeft
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

        Column {
            spacing: 0

            property var gripMouseArea: gripButton.mouseArea

            FlatButton {
                id: gripButton

                Layout.alignment: Qt.AlignTop

                mouseArea.objectName: root.uniqueName + "toolBarMouseAreaVertical"

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
