import QtQuick 2.15

Image {
    id: root

    property int indicatorType: 0
    readonly property bool isHovered: _window.dropIndicators.currentDropLocation === indicatorType

    source: "qrc:/img/classic_indicators/" + _window.iconName(indicatorType, isHovered) + ".png";
    width: 64
    height: 64
}
