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

import com.kdab.dockwidgets 1.0 as KDDW

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
            currentPageUri = uri
            mainWindowLayout.clearWindows()
            mainWindowLayout.showWindows()
        }
    }

    property KeyNavigationSection topToolKeyNavSec: KeyNavigationSection {
        id: keynavSec
        name: "TopTool"
        order: 1
    }

    readonly property int toolbarHeight: 48

    property string currentPageUri: "musescore://home"
    property bool isNotationPage: currentPageUri === notationPage.uri

    property var pages: (new Map([
        [ "musescore://home", homePage ],
        [ "musescore://notation", notationPage ],
        [ "musescore://publish", publishPage ],
        [ "musescore://devtools", devtoolsPage ]
    ]))

    KDDW.MainWindowLayout {
        id: mainWindowLayout
        anchors.fill: parent

        uniqueName: "mainWindowLayout"

        Component.onCompleted: {
            showWindows()
        }

        function clearWindows() {
            _kddwDockRegistry.clear();
        }

        function showWindows() {
            var page = pages.get(currentPageUri)
            page.init(mainWindowLayout)

            var prevToolbar = null
            for (var i = 0; i < toolbars.length; ++i) {
                var toolbar = toolbars[i]
                toolbar.parent = mainWindowLayout
                if (!prevToolbar) {
                    mainWindowLayout.addDockWidget(toolbar, KDDW.KDDockWidgets.Location_OnTop, null, Qt.size(toolbar.width, toolbar.height))
                } else {
                    mainWindowLayout.addDockWidget(toolbar, KDDW.KDDockWidgets.Location_OnRight, prevToolbar, Qt.size(toolbar.width, toolbar.height))
                }

                toolbar.init()
                prevToolbar = toolbar
            }
        }
    }

    HomePage {
        id: homePage

        uri: "musescore://home"
    }

    NotationPage {
        id: notationPage

        uri: "musescore://notation"
    }

    PublishPage {
        id: publishPage

        uri: "musescore://publish"
    }

    DevToolsPage {
        id: devtoolsPage

        uri: "musescore://devtools"
    }

    property list<DockToolBar> toolbars: [
        DockToolBar {
            id: mainToolBar

            uniqueName: "mainToolBar"
            title: qsTrc("appshell", "Main Toolbar")

            width: root.width / 2
            height: root.toolbarHeight
            minimumWidth: 296
            minimumHeight: height
            maximumHeight: height

            contentComponent: DOCKWINDOW.MainToolBar {
                keynav.section: topToolKeyNavSec
                keynav.order: 1
                currentUri: currentPageUri

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

            width: 192
            height: root.toolbarHeight
            minimumWidth: 192
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
                        notationToolBar.visible = Qt.binding(function() { return root.isNotationPage && notationPage.pageModel.isNotationToolBarVisible })
                    }
                }
            }
        },

        DockToolBar {
            id: playbackToolBar

            uniqueName: "playbackToolBar"
            title: qsTrc("appshell", "Playback Controls")

            width: root.width / 3
            height: root.toolbarHeight
            minimumWidth: floating ? 520 : 470
            minimumHeight: floating ? 76 : root.toolbarHeight
            maximumHeight: height

            contentComponent: PlaybackToolBar {
                id: playbackToolBarContent

                keynav.section: topToolKeyNavSec
                keynav.order: 3
                keynav.enabled: root.isNotationPage

                floating: playbackToolBar.floating

                Connections {
                    target: playbackToolBar

                    Component.onCompleted: {
                        notationPage.pageModel.isPlaybackToolBarVisible = playbackToolBar.visible
                        playbackToolBar.visible = Qt.binding(function() { return root.isNotationPage && notationPage.pageModel.isPlaybackToolBarVisible})
                    }
                }
            }
        },

        DockToolBar	{
            id: undoRedoToolBar

            uniqueName: "undoRedoToolBar"
            title: qsTrc("appshell", "Undo/Redo Toolbar")

            width: 72
            height: root.toolbarHeight
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
                        undoRedoToolBar.visible = Qt.binding(function() { return root.isNotationPage && notationPage.pageModel.isUndoRedoToolBarVisible})
                    }
                }
            }
        }
    ]
}
