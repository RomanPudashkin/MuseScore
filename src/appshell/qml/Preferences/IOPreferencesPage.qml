import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.UiComponents 1.0
import MuseScore.Preferences 1.0

import "internal"

Flickable {
    id: root

    contentWidth: width
    contentHeight: content.height

    clip: true
    boundsBehavior: Flickable.StopAtBounds
    interactive: height < contentHeight

    ScrollBar.vertical: StyledScrollBar {}

    IOPreferencesModel {
        id: ioModel
    }

    Column {
        id: content

        width: parent.width
        height: childrenRect.height

        PortAudioSection {
            usePortAudio: ioModel.usePortAudio
            configuration: ioModel.portAudioConfiguration

            onConfigurationChangeRequested: {
                ioModel.portAudioConfiguration = newConfiguration
            }
        }
    }
}
