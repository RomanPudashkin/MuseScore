import QtQuick 2.15

import MuseScore.Dock 1.0

DockBase {
    id: root

    property int orientation: Qt.Horizontal

    allowedAreas: Qt.TopDockWidgetArea
    dockType: DockType.ToolBar

    function init() {
        initConstraints()
    }
}
