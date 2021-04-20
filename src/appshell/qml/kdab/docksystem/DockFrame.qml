import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import com.kdab.dockwidgets 1.0

import "qrc:/kddockwidgets/private/quick/qml/" as KDDW

Rectangle {
    id: root

    //! NOTE: please, don't rename those properties because they are used in c++
    property QtObject frameCpp
    readonly property QtObject titleBarCpp: frameCpp ? frameCpp.titleBar : null
    readonly property int nonContentsHeight: titleBar.visible ? titleBar.heightWhenVisible + tabs.height : 0

    anchors.fill: parent

    color: ui.theme.backgroundPrimaryColor

    onFrameCppChanged: {
        if (Boolean(frameCpp)) {
            frameCpp.setStackLayout(stackLayout)
        }
    }

    onNonContentsHeightChanged: {
        if (Boolean(frameCpp)) {
            frameCpp.geometryUpdated()
        }
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 8

        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: childrenRect.height

            visible: titleBar.visible

            readonly property QtObject titleBarCpp: root.titleBarCpp

            DockTitleBar {
                id: titleBar

                anchors.fill: parent
            }
        }

        RadioButtonGroup {
            id: tabs

            Layout.fillWidth: true
            Layout.preferredHeight: contentItem.childrenRect.height

            visible: count > 1
            spacing: 0

            readonly property QtObject tabBarCpp: root.frameCpp ? root.frameCpp.tabWidget.tabBar : null

            model: Boolean(root.frameCpp) ? root.frameCpp.tabWidget.dockWidgetModel : 0
            currentIndex: Boolean(root.frameCpp) ? root.frameCpp.currentIndex : 0

            delegate: FlatRadioButton {
                ButtonGroup.group: tabs.radioButtonGroup
                checked: model.index === tabs.currentIndex

                StyledTextLabel {
                    text: title
                }

                onClicked: {
                    if (Boolean(root.frameCpp)) {
                        root.frameCpp.tabWidget.setCurrentDockWidget(model.index)
                    }
                }
            }
        }

        StackLayout {
            id: stackLayout

            Layout.fillWidth: true
            Layout.fillHeight: true

            currentIndex: tabs.currentIndex
        }
    }
}
