import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import MuseScore.AppShell 1.0
import MuseScore.Dock 1.0
import MuseScore.Ui 1.0
import MuseScore.UiComponents 1.0

import MuseScore.Shortcuts 1.0
import MuseScore.Playback 1.0
import MuseScore.NotationScene 1.0

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
            loadPage("musescore://home")
        }

        readonly property int toolbarHeight: 48
        readonly property bool isNotationPage: currentPageUri === notationPage.uri

        toolBars: [
            DockToolBar {
                id: mainToolBar

                uniqueName: "mainToolBar"
                title: qsTrc("appshell", "Main Toolbar")

                width: root.width / 2
                height: window.toolbarHeight
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
            },

            DockToolBar {
                id: notationToolBar

                uniqueName: "notationToolBar"
                title: qsTrc("appshell", "Notation Toolbar")

                width: 198
                height: window.toolbarHeight
                minimumWidth: 198
                minimumHeight: height
                maximumHeight: height

                contentComponent: NotationToolBar {
                    id: notationToolBarContent

                    keynav.section: topToolKeyNavSec
                    keynav.order: 2
                    keynav.enabled: notationToolBar.visible
                    onActiveFocusRequested: {
                        if (keynav.active) {
                            notationToolBar.forceActiveFocus()
                        }
                    }

                    Connections {
                        target: notationToolBar

                        Component.onCompleted: {
                            notationPage.pageModel.isNotationToolBarVisible = notationToolBar.visible
                            notationToolBar.visible = Qt.binding(function() { return window.isNotationPage && notationPage.pageModel.isNotationToolBarVisible })
                        }
                    }
                }
            },

            DockToolBar {
                id: playbackToolBar

                uniqueName: "playbackToolBar"
                title: qsTrc("appshell", "Playback Controls")

                width: root.width / 3
                height: window.toolbarHeight
                minimumWidth: floating ? 526 : 476
                minimumHeight: floating ? 76 : window.toolbarHeight
                maximumHeight: height

                contentComponent: PlaybackToolBar {
                    id: playbackToolBarContent

                    keynav.section: topToolKeyNavSec
                    keynav.order: 3
                    keynav.enabled: window.isNotationPage

                    floating: playbackToolBar.floating

                    Connections {
                        target: playbackToolBar

                        Component.onCompleted: {
                            notationPage.pageModel.isPlaybackToolBarVisible = playbackToolBar.visible
                            playbackToolBar.visible = Qt.binding(function() { return window.isNotationPage && notationPage.pageModel.isPlaybackToolBarVisible})
                        }
                    }
                }
            },

            DockToolBar	{
                id: undoRedoToolBar

                uniqueName: "undoRedoToolBar"
                title: qsTrc("appshell", "Undo/Redo Toolbar")

                width: 74
                height: window.toolbarHeight
                minimumWidth: width
                minimumHeight: height
                maximumHeight: height

                movable: false

                contentComponent: UndoRedoToolBar {
                    id: undoRedoToolBarContent

                    Connections {
                        target: undoRedoToolBar

                        Component.onCompleted: {
                            notationPage.pageModel.isUndoRedoToolBarVisible = undoRedoToolBar.visible
                            undoRedoToolBar.visible = Qt.binding(function() { return window.isNotationPage && notationPage.pageModel.isUndoRedoToolBarVisible})
                        }
                    }
                }
            }
        ]

        pages: [
            HomePage {
                id: homePage

                uri: "musescore://home"
            },

            NotationPage {
                id: notationPage

                uri: "musescore://notation"
            },

            PublishPage {
                id: publishPage

                uri: "musescore://publish"
            },

            DevToolsPage {
                id: devtoolsPage

                uri: "musescore://devtools"
            }
        ]
    }
}
