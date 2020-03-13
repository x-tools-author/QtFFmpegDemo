import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.5

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    title: qsTr("ImageProvider")

    menuBar: MenuBar {
        Menu {
            id: loadMenu
            title: qsTr("显示方式")
            Repeater {
                model: [qsTr("ImageProvider"), qsTr("QuickPaintedItem"), qsTr("VideoOutput")]
                MenuItem {
                    text: modelData
                    onClicked: {
                        if (index == 0){
                            windowContentLoader.source = "qrc:/ui/ImageProvider.qml"
                        }else if (index == 1){
                            windowContentLoader.source = "qrc:/ui/QuickPaintedItem.qml"
                        }else if (index == 2){
                            windowContentLoader.source = "qrc:/ui/CustomVideoOutput.qml"
                        }

                        root.title = modelData
                    }
                }
            }
        }
    }

    Loader {
        id: windowContentLoader
        anchors.fill: parent
        source: "qrc:/ui/ImageProvider.qml"
    }
}
