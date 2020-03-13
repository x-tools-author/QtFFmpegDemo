import QtQuick 2.12
import QtQuick.Window 2.12
import QtMultimedia 5.12
import CustomType 1.0

Rectangle {
    visible: true
    width: 640
    height: 480
    color: "red"

    VideoOutput {
        id: videoViewer
        source: VideoOutputSource{}
        anchors.fill: parent
    }
}
