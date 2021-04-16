import QtQuick 2.15

import MuseScore.Dock 1.0

DockBase {
    id: root

    allowedAreas: Qt.BottomDockWidgetArea
    dockType: DockType.StatusBar

    function init() {
        initConstraints()
    }
}
