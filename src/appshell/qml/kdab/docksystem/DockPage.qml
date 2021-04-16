import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import com.kdab.dockwidgets 1.0 as KDDW

Item {
    id: root

    property string uri: ""
    property string uniqueName: ""

    property list<DockToolBar> toolbars
    property list<DockPanel> panels
    property alias central: central.sourceComponent
    property list<DockStatusBar> statusbars

    DockCentral {
        id: centralDock

        uniqueName: root.uniqueName + "_central"

        Loader {
            id: central

            anchors.fill: parent
        }
    }

    function init(layout) {
        for (var i = 0; i < root.panels.length; ++i) {
            var panel = root.panels[i]
            panel.parent = layout
            layout.addDockWidget(panel, KDDW.KDDockWidgets.Location_OnLeft)
            panel.init()
        }

        layout.addDockWidget(centralDock, KDDW.KDDockWidgets.Location_OnRight)

        var prevToolbar = null
        for (i = 0; i < root.toolbars.length; ++i) {
            var toolbar = root.toolbars[i]
            toolbar.parent = layout

            if (!prevToolbar) {
                layout.addDockWidget(toolbar, KDDW.KDDockWidgets.Location_OnTop, null, Qt.size(toolbar.width, toolbar.height))
            } else {
                layout.addDockWidget(toolbar, KDDW.KDDockWidgets.Location_OnRight, prevToolbar, Qt.size(toolbar.width, toolbar.height))
            }

            toolbar.init()
            prevToolbar = toolbar
        }

        var prevStatusBar = null
        for (i = 0; i < root.statusbars.length; ++i) {
            var statusbar = root.statusbars[i]
            statusbar.parent = layout

            if (!prevStatusBar) {
                layout.addDockWidget(statusbar, KDDW.KDDockWidgets.Location_OnBottom, null, Qt.size(statusbar.width, statusbar.height))
            } else {
                layout.addDockWidget(statusbar, KDDW.KDDockWidgets.Location_OnRight, prevStatusBar, Qt.size(statusbar.width, statusbar.height))
            }

            statusbar.init()
            prevStatusBar = statusbar
        }
    }
}
