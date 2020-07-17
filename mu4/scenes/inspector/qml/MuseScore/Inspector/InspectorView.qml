import QtQuick 2.8

import MuseScore.Inspector 1.0

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
