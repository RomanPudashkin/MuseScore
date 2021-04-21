import QtQuick 2.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import "qrc:/kddockwidgets/private/quick/qml/" as KDDW

Item {
    id: root

    required property QtObject titleBarCpp

    property alias heightWhenVisible: titleBar.heightWhenVisible

    width: parent.width
    height: visible ? heightWhenVisible : 0

    visible: Boolean(titleBarCpp) && titleBarCpp.isFloating

    KDDW.TitleBarBase {
        id: titleBar

        anchors.fill: parent

        heightWhenVisible: 30

        StyledTextLabel {
            id: titleLabel

            anchors.centerIn: parent

            font: ui.theme.bodyBoldFont
            text: titleBar.title
        }
    }
}
