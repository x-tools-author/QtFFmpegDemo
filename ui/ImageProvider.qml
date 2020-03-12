import QtQuick          2.12
import QtQuick.Window   2.12

Rectangle {
    visible: true
    width: 640
    height: 480

    Image {
        id: videoFrame
        fillMode: Image.PreserveAspectFit
        source: "qrc:/resources/F.png"
        cache: false
        anchors.centerIn: parent
    }

    Timer {
        interval: 1000/30
        running: true
        repeat: true
        onTriggered: {
            videoFrame.source = ""
            videoFrame.source = "image://videoprovider/"
        }
    }
}
