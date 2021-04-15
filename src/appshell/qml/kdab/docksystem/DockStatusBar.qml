import QtQuick 2.15

import com.kdab.dockwidgets 1.0 as KDDW

KDDW.DockWidget {
    id: root

    property int minimumWidth: 0
    property int minimumHeight: 0
    property int maximumWidth: 0
    property int maximumHeight: 0

    signal closed()

    function init() {
        var minWidth = minimumWidth > 0 ? minimumWidth : minimumSize.width
        var minHeight = minimumHeight > 0 ? minimumHeight : minimumSize.height
        var maxWidth = maximumWidth > 0 ? maximumWidth : maximumSize.width
        var maxHeight = maximumHeight > 0 ? maximumHeight : maximumSize.height

        minimumSize = Qt.size(minWidth, minHeight)
        maximumSize = Qt.size(maxWidth, maxHeight)
    }
}
