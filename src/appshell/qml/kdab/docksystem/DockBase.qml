import QtQuick 2.15

import MuseScore.Dock 1.0

import com.kdab.dockwidgets 1.0 as KDDW

KDDW.DockWidget {
    id: root

    property int minimumWidth: 0
    property int minimumHeight: 0
    property int maximumWidth: 0
    property int maximumHeight: 0

    property bool floatable: true
    property bool closable: true

    property alias allowedAreas: properties.allowedAreas
    property alias dockType: properties.dockType

    signal closed()

    QtObject {
        id: properties

        objectName: "properties"

        property int allowedAreas: Qt.AllDockWidgetAreas
        property int dockType: DockType.Undefined
    }

    Component.onCompleted: {
        dockWidget.data.push(properties)
    }

    function initConstraints() {
        var minWidth = minimumWidth > 0 ? minimumWidth : minimumSize.width
        var minHeight = minimumHeight > 0 ? minimumHeight : minimumSize.height
        var maxWidth = maximumWidth > 0 ? maximumWidth : maximumSize.width
        var maxHeight = maximumHeight > 0 ? maximumHeight : maximumSize.height

        minimumSize = Qt.size(minWidth, minHeight)
        maximumSize = Qt.size(maxWidth, maxHeight)
    }
}
