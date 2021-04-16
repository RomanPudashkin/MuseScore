import QtQuick 2.15

import MuseScore.Dock 1.0

DockBase {
    id: root

    property var tabs: []

    allowedAreas: Qt.LeftDockWidgetArea | Qt.RightDockWidgetArea
    dockType: DockType.Panel

    function init() {
        initConstraints()

        for (var i in tabs) {
            addDockWidgetAsTab(tabs[i].dockWidget)
        }
    }
}
