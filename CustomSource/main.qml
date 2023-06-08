import QtQuick 
import QtQuick.Window 
import QtMultimedia
import CustomSource 1.0
Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    CameraSource {
        id: producer
        videoSink: videoOutput.videoSink
    }
    VideoOutput{
        id: videoOutput
        anchors.fill: parent
    }
    Component.onCompleted: producer.start()
}
