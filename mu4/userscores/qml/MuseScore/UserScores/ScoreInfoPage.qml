import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.UserScores 1.0

Item {
    id: root

    function info() {
        var result = []

        result.push("title", generalInfo.title)
        result.push("subtitle", generalInfo.subtitle)
        result.push("composer", generalInfo.composer)
        result.push("lyricist", generalInfo.lyricist)
        result.push("copiright", generalInfo.copiright)

        result.push("keySignature", additionalInfo.keySignature)
        result.push("timeSignature", additionalInfo.timeSignature)
        result.push("withTempo", additionalInfo.withTempo)
        result.push("tempo", additionalInfo.tempo)
        result.push("withPickupMeasure", additionalInfo.withPickupMeasure)
        result.push("pickupTimeSignature", additionalInfo.pickupTimeSignature)
        result.push("measureCount", additionalInfo.measureCount)

        return result
    }

    StyledTextLabel {
        id: title

        anchors.top: parent.top
        anchors.topMargin: 24
        anchors.horizontalCenter: parent.horizontalCenter

        font.bold: true
        text: qsTrc("instruments", "Additional Score Information")
    }

    ColumnLayout {
        anchors.top: title.bottom
        anchors.topMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.bottom: parent.bottom

        spacing: 30

        AdditionalInfoView {
            id: additionalInfo

            anchors.left: parent.left
            anchors.right: parent.right

            Layout.preferredHeight: 150
        }

        Rectangle {
            anchors.left: parent.left
            anchors.right: parent.right

            height: 2

            color: ui.theme.buttonColor
        }

        GeneralInfoView {
            id: generalInfo

            Layout.fillHeight: true

            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
