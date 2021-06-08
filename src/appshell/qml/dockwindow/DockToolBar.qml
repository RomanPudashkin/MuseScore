/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick 2.15
import QtQuick.Layouts 1.15

import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0
import MuseScore.Dock 1.0 as Dock

Dock.DockToolBar {
    id: root

    property alias contentItem: contentLoader.sourceComponent
    property alias movable: gripButton.visible

    Component.onCompleted: {
        console.debug("--- obj: " + objectName)
        console.debug("--- contentSize: " + Qt.size(contentLoader.width, contentLoader.height))
    }

    Rectangle {
        anchors.fill: parent

        color: ui.theme.backgroundPrimaryColor

        Item {
            id: contentRect

            property int margins: 2

            property int gripButtonWidth: gripButton.visible ? gripButton.width : 0
            property int gripButtonHeight: gripButton.visible ? gripButton.height : 0

            anchors.fill: parent
            anchors.margins: contentRect.margins

            FlatButton {
                id: gripButton

                mouseArea.objectName: root.objectName + "_gripButton"

                normalStateColor: "transparent"
                icon: IconCode.TOOLBAR_GRIP

                Component.onCompleted: {
                    root.setDraggableMouseArea(mouseArea)
                }
            }

            Loader {
                id: contentLoader
            }
        }
    }

    states: [
        State {
            name: "HORIZONTAL"
            when: root.orientation === Qt.Horizontal

            PropertyChanges {
                target: root

                minimumWidth: contentLoader.item ? 2 * contentRect.margins + contentRect.gripButtonWidth + contentLoader.width : 0
                minimumHeight: contentLoader.item ? 2 * contentRect.margins + contentLoader.height : 0
            }

            PropertyChanges {
                target: gripButton

                anchors.left: parent.left
                anchors.top: undefined
            }

            PropertyChanges {
                target: contentLoader

                anchors.left: gripButton.visible ? gripButton.right : parent.left
                anchors.leftMargin: contentRect.margins
                anchors.top: parent.top
            }
        },

        State {
            name: "VERTICAL"
            when: root.orientation === Qt.Vertical

            PropertyChanges {
                target: root

                minimumWidth: contentLoader.item ? 2 * contentRect.margins + contentLoader.width : 0
                minimumHeight: contentLoader.item ? 2 * contentRect.margins + contentRect.gripButtonHeight + contentLoader.height : 0
            }

            PropertyChanges {
                target: gripButton

                anchors.top: parent.top
                anchors.left: undefined

                rotation: 90
            }

            PropertyChanges {
                target: contentLoader

                anchors.top: gripButton.bottom
                anchors.topMargin: contentRect.margins
                anchors.left: parent.left
            }
        }
    ]
}
