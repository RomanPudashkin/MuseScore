import QtQuick 2.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import "qrc:/kddockwidgets/private/quick/qml/" as KDDW

Item {
    id: root

    //! NOTE: please, don't rename those properties because they are used in c++
    readonly property QtObject floatingWindowCpp: parent
    readonly property QtObject titleBarCpp: Boolean(floatingWindowCpp) ? floatingWindowCpp.titleBar : null
    readonly property QtObject dropAreaCpp: Boolean(floatingWindowCpp) ? floatingWindowCpp.dropArea : null
    readonly property int titleBarHeight: titleBar.heightWhenVisible

    anchors.fill: parent

    onTitleBarHeightChanged: {
        if (Boolean(floatingWindowCpp)) {
            floatingWindowCpp.geometryUpdated()
        }
    }

    DockTitleBar {
        id: titleBar

        anchors.top: parent.top

        titleBarCpp: root.titleBarCpp
    }

    KDDW.DropArea {
        id: dropArea

        anchors.top: titleBar.bottom
        anchors.bottom: parent.bottom

        width: parent.width

        dropAreaCpp: root.dropAreaCpp
    }

    onDropAreaCppChanged: {
        if (Boolean(dropAreaCpp)) {
            dropAreaCpp.parent = dropArea
            dropAreaCpp.anchors.fill = dropArea
        }
    }
}
