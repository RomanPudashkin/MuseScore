import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import com.kdab.dockwidgets 1.0 as KDDW

import "../HomePage"

Page {
    id: root

    property string uri: ""
    property alias uniqueName: layout.uniqueName

    property list<DockPanel> panels

    property alias statusBar: root.footer
    property alias central: central.sourceComponent

    padding: 0

    background: Rectangle {
        color: ui.theme.backgroundPrimaryColor
    }

    contentItem: KDDW.MainWindowLayout {
        id: layout

        anchors.fill: parent

        KDDW.DockWidget {
            id: centralDock

            uniqueName: root.uniqueName + "_central"

            Loader {
                id: central

                anchors.fill: parent
            }
        }

        Component.onCompleted: {
            setAffinities([ root.uniqueName ])

            for (var i = 0; i < root.panels.length; ++i) {
                root.panels[i].setAffinities([ root.uniqueName ])
            }

            centralDock.setAffinities([ root.uniqueName ])

            for (i = 0; i < root.panels.length; ++i) {
                var panel = root.panels[i]
                panel.parent = layout
                addDockWidget(panel, KDDW.KDDockWidgets.Location_OnLeft)
                panel.init()
            }

            addDockWidget(centralDock, KDDW.KDDockWidgets.Location_OnRight)
        }
    }
}
