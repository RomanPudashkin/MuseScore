import QtQuick 2.8

import MuseScore.Inspector 3.3

import "../Inspector"

Item {
    id: root

    InspectorListModel {
        id: model
    }

    InspectorForm {
        anchors.fill: parent
        inspectorListModel: model
    }

    Component.onCompleted: {
        model.load()
    }
}
