import QtQuick 2.15

DockBase {
    id: root

    property int orientation: Qt.Horizontal

    function init() {
        initConstraints()
    }
}
