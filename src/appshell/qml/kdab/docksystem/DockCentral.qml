import QtQuick 2.15

import MuseScore.Dock 1.0

DockBase {
    id: root

    floatable: false
    closable: false

    allowedAreas: Qt.NoDockWidgetArea
    dockType: DockType.Central

    function init() {
        initConstraints()
    }
}
