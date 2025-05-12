/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited
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

import Muse.Ui 1.0
import Muse.UiComponents 1.0

import MuseScore.NotationScene 1.0

Loader {
    id: root

    property rect notationViewRect: Qt.rect(0, 0, 0, 0)
    property alias notationViewMatrix: regionsBeingProcessedModel.notationViewMatrix

    NotationRegionsBeingProcessedModel {
        id: regionsBeingProcessedModel
    }

    active: !regionsBeingProcessedModel.isEmpty
    enabled: false // ignore input events

    Component.onCompleted: {
        regionsBeingProcessedModel.load()
    }

    onLoaded: {
        item.x = root.notationViewRect.x
        item.y = root.notationViewRect.y
        item.height = root.notationViewRect.height
        item.width = root.notationViewRect.width
        item.model = regionsBeingProcessedModel
    }

    sourceComponent: Item {
        property alias model: repeater.model

        clip: true

        Repeater {
            id: repeater

            anchors.fill: parent

            delegate: Item {
                id: delegateItem

                readonly property real sideMargin: iconLabel.width / 3
                readonly property real spacing: iconLabel.width / 2
                readonly property real freeSpace: background.width - iconLabel.width - delegateItem.sideMargin - delegateItem.spacing

                x: model.rect.x
                y: model.rect.y - model.rect.height
                width: Math.max(model.rect.width, iconLabel.width * 2 + delegateItem.sideMargin) // add extra space for better look...
                height: model.rect.height * 0.7

                Rectangle {
                    id: background

                    anchors.fill: parent

                    radius: background.height * 0.7
                    color: repeater.model.progressBackgroundColor

                    SequentialAnimation on opacity {
                        loops: Animation.Infinite

                        NumberAnimation {
                            from: 0.3
                            to: 0.6
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }

                        NumberAnimation {
                            from: 0.6
                            to: 0.3
                            duration: 1000
                            easing.type: Easing.InOutQuad
                        }
                    }
                }

                StyledIconLabel {
                    id: iconLabel

                    anchors.left: parent.left
                    anchors.leftMargin: delegateItem.sideMargin
                    anchors.verticalCenter: parent.verticalCenter

                    z: background.z + 1

                    opacity: 1
                    color: repeater.model.progressFontColor

                    horizontalAlignment: Text.AlignLeft
                    font.pixelSize: background.height * 0.8

                    iconCode: IconCode.AUDIO

                    states: [
                        State {
                            name: "CENTERED"
                            when: textLabel.empty

                            AnchorChanges {
                                target: iconLabel
                                anchors.left: undefined
                                anchors.horizontalCenter: parent.horizontalCenter
                            }
                        }
                    ]
                }

                StyledTextLabel {
                    id: textLabel

                    readonly property string longText: qsTrc("global", "Processing %1%…")
                    readonly property string shortText: qsTrc("global", "%1%")
                    readonly property bool empty: textLabel.text.length === 0

                    anchors.left: iconLabel.right
                    anchors.leftMargin: delegateItem.spacing
                    anchors.verticalCenter: parent.verticalCenter

                    z: background.z + 1

                    opacity: 1
                    color: repeater.model.progressFontColor

                    horizontalAlignment: Text.AlignLeft
                    font.pixelSize: background.height * 0.6

                    text: {
                        if (longTextMetrics.width < delegateItem.freeSpace) {
                            return textLabel.longText.arg(model.progress)
                        } else if (shortTextMetrics.width < delegateItem.freeSpace) {
                            return textLabel.shortText.arg(model.progress)
                        }

                        return ""
                    }

                    TextMetrics {
                        id: longTextMetrics

                        font: textLabel.font
                        text: textLabel.longText.arg(100) // make sure 100% fits
                    }

                    TextMetrics {
                        id: shortTextMetrics

                        font: textLabel.font
                        text: textLabel.shortText.arg(100) // make sure 100% fits
                    }
                }
            }
        }
    }
}
