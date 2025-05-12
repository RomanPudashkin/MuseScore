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
import Muse.GraphicalEffects 1.0

import MuseScore.NotationScene 1.0

Item {
    id: root

    property alias notationViewRect: regionsBeingProcessedModel.notationViewRect
    property alias notationViewMatrix: regionsBeingProcessedModel.notationViewMatrix

    enabled: false // ignore input events

    NotationRegionsBeingProcessedModel {
        id: regionsBeingProcessedModel
    }

    Component.onCompleted: {
        regionsBeingProcessedModel.load()
    }

    Repeater {
        anchors.fill: parent

        model: regionsBeingProcessedModel.regions

        delegate: Rectangle {
            id: backgroundRect

            x: modelData.x
            y: modelData.y
            width: modelData.width
            height: modelData.height

            color: regionsBeingProcessedModel.regionColor
            clip: true

            Rectangle {
                id: gradientRect

                width: parent.width / 2
                height: parent.height

                x: -gradientRect.width

                layer.enabled: ui.isEffectsAllowed
                layer.effect: EffectOpacityMask {
                    source: backgroundRect
                    maskSource: gradientRect
                }

                gradient: Gradient {
                    orientation: Qt.Horizontal

                    GradientStop { position: 0.0; color: "transparent" }
                    GradientStop { position: 0.4; color: Utils.colorWithAlpha("#FFFFFF", 0.5) }
                    GradientStop { position: 0.6; color: Utils.colorWithAlpha("#FFFFFF", 0.5) }
                    GradientStop { position: 1.0; color: "transparent" }
                }

                NumberAnimation on x {
                    from: -gradientRect.width
                    to: gradientRect.width

                    duration: 1200
                    running: true
                    loops: Animation.Infinite
                }
            }
        }
    }
}
