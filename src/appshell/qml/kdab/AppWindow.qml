import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.Shortcuts 1.0

ApplicationWindow {
    id: root

    width: 1024
    height: 800

    minimumWidth: 1024
    minimumHeight: 600

    visible: true

    title: qsTrc("appshell", "MuseScore 4")

    //! NOTE Need only create
    Shortcuts { }
}
