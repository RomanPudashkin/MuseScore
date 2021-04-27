import QtQuick 2.15
import QtQuick.Controls 2.15

import MuseScore.AppShell 1.0
import MuseScore.Dock 1.0
import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import "./HomePage"
import "./NotationPage"
import "./PublishPage"
import "./DevTools"
import "docksystem"

import "../../qml" as DOCKWINDOW

ApplicationWindow {
    id: root

    width: 800
    height: 600

    visible: true

    title: qsTrc("appshell", "MuseScore 4")

    property var provider: InteractiveProvider {
        topParent: root

        onRequestedDockPage: {
            window.loadPage(uri)
        }
    }

    property KeyNavigationSection topToolKeyNavSec: KeyNavigationSection {
        id: keynavSec
        name: "TopTool"
        order: 1
    }

    DockWindow {
        id: window

        anchors.fill: parent

        Component.onCompleted: {
            api.launcher.open(homePage.uri)
        }

        toolBars: [
            DockToolBar {
                id: mainToolBar

                objectName: "mainToolBar"
                title: qsTrc("appshell", "Main Toolbar")

                width: root.width / 2
                height: 48
                minimumWidth: 304
                minimumHeight: height
                maximumHeight: height

                contentComponent: DOCKWINDOW.MainToolBar {
                    keynav.section: topToolKeyNavSec
                    keynav.order: 1
                    currentUri: window.currentPageUri

                    keynav.onActiveChanged: {
                        if (keynav.active) {
                            mainToolBar.forceActiveFocus()
                        }
                    }

                    onSelected: {
                        api.launcher.open(uri)
                    }
                }
            }
        ]

        pages: [
            HomePage {
                id: homePage
            },

            NotationPage {
                topToolKeyNavSec: root.topToolKeyNavSec
            },

            PublishPage {},

            DevToolsPage {}
        ]
    }
}
