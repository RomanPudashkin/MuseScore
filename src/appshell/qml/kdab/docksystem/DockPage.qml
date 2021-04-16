import QtQuick 2.15

import MuseScore.Dock 1.0 as Dock

Dock.DockPage {
    id: root

    property alias central: central.sourceComponent

    centralDock: Dock.DockCentral {
        uniqueName: root.uniqueName + "_central"

        Loader {
            id: central

            anchors.fill: parent
        }
    }
}
