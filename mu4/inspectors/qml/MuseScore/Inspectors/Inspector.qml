import QtQuick 2.8

import "qrc:/view/qml/" as C

import MuseScore.Inspectors 3.3

Item {
    id: root

    width: 360

    InspectorListModel {
        id: model
    }

    C.InspectorForm {
        anchors.fill: parent
        inspectorListModel: model
    }

    Component.onCompleted: {
        model.load()
    }
}
