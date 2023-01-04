import QtQuick 2.13
import QtQuick.Window 2.13

Window {
    id: window
    visible: true
    width: 800
    height: 480
    title: qsTr("Projecto")

    property var app: null
    Component.onCompleted: {
        createApp();
    }

    Connections{
        target: fileWatcher
        function onFileChanged(){
            console.log("Reloading");
            createApp();
        }
    }

    function createApp(){
        if(app !== null){
            app.destroy();
        }
        var component = Qt.createComponent("/home/youcef/workspace/c++/delivery_app/delivery_app/App.qml");
        if (component.status === Component.Ready){
            app = component.createObject(window);
        }
    }
}










