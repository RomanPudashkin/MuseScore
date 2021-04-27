import QtQuick 2.7

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import "../docksystem"

DockPage {
    id: publishPage

    objectName: "Publish"
    uri: "musescore://publish"

    central: Rectangle {
        color: ui.theme.backgroundSecondaryColor

        StyledTextLabel {
            anchors.centerIn: parent
            text: "Publish"
        }
    }
}
