import QtQuick 2.8

import MuseScore.Inspectors 3.3

import "../Inspectors"

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
