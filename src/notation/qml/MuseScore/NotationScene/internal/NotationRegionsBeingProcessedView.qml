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
                x: model.rect.x
                y: model.rect.y - model.rect.height
                width: model.rect.width + lablesRow.margin
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

                Row {
                    id: lablesRow

                    readonly property real margin: iconLabel.width / 3
                    readonly property real freeSpace: background.width - iconLabel.width - lablesRow.margin * 2 - lablesRow.spacing

                    anchors.left: parent.left
                    anchors.leftMargin: lablesRow.margin
                    anchors.verticalCenter: parent.verticalCenter

                    z: background.z + 1
                    spacing: iconLabel.width / 2

                    StyledIconLabel {
                        id: iconLabel

                        opacity: 1
                        color: repeater.model.progressFontColor

                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: background.height * 0.8

                        iconCode: IconCode.AUDIO
                    }

                    StyledTextLabel {
                        id: textLabel

                        readonly property string longText: qsTrc("global", "Processing %1%…").arg(model.progress)
                        readonly property string shortText: qsTrc("global", "%1%").arg(model.progress)

                        opacity: 1
                        color: repeater.model.progressFontColor

                        horizontalAlignment: Text.AlignLeft
                        font.pixelSize: background.height * 0.6

                        text: {
                            if (longTextMetrics.width < lablesRow.freeSpace) {
                                return textLabel.longText
                            } else if (shortTextMetrics.width < lablesRow.freeSpace) {
                                return textLabel.shortText
                            }

                            return ""
                        }

                        TextMetrics {
                            id: longTextMetrics

                            font: textLabel.font
                            text: textLabel.longText
                        }

                        TextMetrics {
                            id: shortTextMetrics

                            font: textLabel.font
                            text: textLabel.shortText
                        }
                    }
                }
            }
        }
    }
}
