import QtQuick 2.15

import com.kdab.dockwidgets 1.0 as KDDW

KDDW.DockWidget {
    id: root

    property int minimumWidth: 0
    property int minimumHeight: 0
    property int maximumWidth: 0
    property int maximumHeight: 0

    property bool floatable: true
    property bool closable: true

    property var tabifyPanels: []

    signal closed()

    function tabify() {
        for (var i in tabifyPanels) {
            addDockWidgetAsTab(tabifyPanels[i].dockWidget)
        }
    }
}
