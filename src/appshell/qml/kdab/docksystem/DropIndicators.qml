import QtQuick 2.15

import com.kdab.dockwidgets 1.0

Item {
    id: root

    readonly property int outterMargin: 10
    readonly property int innerMargin: 10
    readonly property QtObject innerIndicators: innerIndicators

    anchors.fill: parent

    DropIndicator {
        anchors {
            left: parent.left
            leftMargin: root.outterMargin
            verticalCenter: parent.verticalCenter
        }

        indicatorType: DropIndicatorOverlayInterface.DropLocation_OutterLeft
        visible: _window.dropIndicators.outterLeftIndicatorVisible
    }

    DropIndicator {
        anchors {
            right: parent.right
            rightMargin: root.outterMargin
            verticalCenter: parent.verticalCenter
        }

        indicatorType: DropIndicatorOverlayInterface.DropLocation_OutterRight
        visible: _window.dropIndicators.outterRightIndicatorVisible
    }

    DropIndicator {
        anchors {
            top: parent.top
            topMargin: root.outterMargin
            horizontalCenter: parent.horizontalCenter
        }

        indicatorType: DropIndicatorOverlayInterface.DropLocation_OutterTop
        visible: _window.dropIndicators.outterTopIndicatorVisible
    }

    DropIndicator {
        anchors {
            bottom: parent.bottom
            bottomMargin: root.outterMargin
            horizontalCenter: parent.horizontalCenter
        }

        indicatorType: DropIndicatorOverlayInterface.DropLocation_OutterBottom
        visible: _window.dropIndicators.outterBottomIndicatorVisible
    }

    Item {
        id: innerIndicators

        objectName: "innerIndicators"

        x: _window.dropIndicators.hoveredFrameRect.x + (_window.dropIndicators.hoveredFrameRect.width / 2)
        y: _window.dropIndicators.hoveredFrameRect.y + (_window.dropIndicators.hoveredFrameRect.height / 2)

        width: (centerIndicator * 3) + (2 * innerMargin)
        height: width

        DropIndicator {
            id: centerIndicator

            anchors.centerIn: parent

            indicatorType: DropIndicatorOverlayInterface.DropLocation_Center
            visible: _window.dropIndicators.centralIndicatorVisible
        }

        DropIndicator {
            anchors {
                right: centerIndicator.left
                rightMargin: root.innerMargin
                verticalCenter: parent.verticalCenter
            }

            indicatorType: DropIndicatorOverlayInterface.DropLocation_Left
            visible: _window.dropIndicators.innerLeftIndicatorVisible
        }

        DropIndicator {
            anchors {
                left: centerIndicator.right
                leftMargin: root.innerMargin
                verticalCenter: parent.verticalCenter
            }

            indicatorType: DropIndicatorOverlayInterface.DropLocation_Right
            visible: _window.dropIndicators.innerRightIndicatorVisible
        }

        DropIndicator {
            anchors {
                bottom: centerIndicator.top
                bottomMargin: root.innerMargin
                horizontalCenter: parent.horizontalCenter
            }

            indicatorType: DropIndicatorOverlayInterface.DropLocation_Top
            visible: _window.dropIndicators.innerTopIndicatorVisible
        }

        DropIndicator {
            anchors {
                top: centerIndicator.bottom
                topMargin: root.innerMargin
                horizontalCenter: parent.horizontalCenter
            }

            indicatorType: DropIndicatorOverlayInterface.DropLocation_Bottom
            visible: _window.dropIndicators.innerBottomIndicatorVisible
        }
    }
}

