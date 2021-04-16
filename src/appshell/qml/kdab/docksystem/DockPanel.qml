import QtQuick 2.15

DockBase {
    id: root

    property var tabs: []

    allowedAreas: Qt.LeftDockWidgetArea | Qt.RightDockWidgetArea

    function init() {
        initConstraints()

        for (var i in tabs) {
            addDockWidgetAsTab(tabs[i].dockWidget)
        }
    }
}
