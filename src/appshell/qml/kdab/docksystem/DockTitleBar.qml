import QtQuick 2.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import "qrc:/kddockwidgets/private/quick/qml/" as KDDW

KDDW.TitleBarBase {
    id: root

    heightWhenVisible: 30
    visible: Boolean(titleBarCpp) && titleBarCpp.isFloating

    StyledTextLabel {
        anchors.centerIn: parent

        text: root.title
        font: ui.theme.bodyBoldFont
    }
}
