import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.UserScores 1.0

Row {
    id: row

    property alias keySignature: infoModel.keySignature
    property alias timeSignature: infoModel.timeSignature
    property alias timeSignatureType: infoModel.timeSignatureType

    property alias withTempo: infoModel.withTempo
    property alias tempo: infoModel.tempo

    property alias withPickupMeasure: infoModel.withPickupMeasure
    property alias pickupTimeSignature: infoModel.pickupTimeSignature
    property alias measureCount: infoModel.measureCount

    spacing: 20

    QtObject {
        id: privatesProperties

        property real childWidth: (width / 4) - 15
    }

    Component.onCompleted: {
        infoModel.init()
    }

    AdditionalInfoModel {
        id: infoModel
    }

    Column {
        id: column

        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: privatesProperties.childWidth

        spacing: 10

        StyledTextLabel {
            anchors.left: parent.left
            anchors.right: parent.right

            font.bold: true
            horizontalAlignment: Text.AlignLeft
            text: qsTrc("instruments", "Key Signature")
        }

        KeySignatureSettings {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 120

            model: infoModel
            arrowX: Boolean(oppened) ? popupPositionX + (width / 2) : 0
            popupPositionX: Boolean(oppened) ? mapToGlobal(column.x, column.y).x - mapToGlobal(x, y).x : 0
        }
    }

    Column {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: privatesProperties.childWidth

        spacing: 10

        StyledTextLabel {
            anchors.left: parent.left
            anchors.right: parent.right

            font.bold: true
            horizontalAlignment: Text.AlignLeft
            text: qsTrc("instruments", "Time Signature")
        }

        TimeSignatureSettings {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 120

            model: infoModel
            arrowX: Boolean(oppened) ? popupPositionX + (width / 2) : 0
            popupPositionX: Boolean(oppened) ? mapToGlobal(column.x, column.y).x - mapToGlobal(x, y).x : 0
        }
    }

    Column {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: privatesProperties.childWidth

        spacing: 10

        StyledTextLabel {
            anchors.left: parent.left
            anchors.right: parent.right

            font.bold: true
            horizontalAlignment: Text.AlignLeft
            text: qsTrc("instruments", "Tempo")
        }

        TempoSettings {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 120

            model: infoModel
            arrowX: Boolean(oppened) ? popupPositionX + (width / 2) : 0
            popupPositionX: Boolean(oppened) ? mapToGlobal(column.x, column.y).x - mapToGlobal(x, y).x : 0
        }
    }

    Column {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: privatesProperties.childWidth

        spacing: 10

        StyledTextLabel {
            anchors.left: parent.left
            anchors.right: parent.right

            font.bold: true
            horizontalAlignment: Text.AlignLeft
            text: qsTrc("instruments", "Measures")
        }

        MeasuresSettings {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 120

            model: infoModel
            arrowX: Boolean(oppened) ? popupPositionX + (width / 2) : 0
            popupPositionX: Boolean(oppened) ? mapToGlobal(column.x, column.y).x - mapToGlobal(x, y).x : 0
        }
    }
}
