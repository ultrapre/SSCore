import QtQuick 2.6
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
//import QtQuick.Controls.Styles 2.2

//Window {
//    width: 640
//    height: 480
//    visible: true
//    title: qsTr("Hello World")
//}

Window {
    width: 640
    height: 480
    visible: true

    Button{
        id:btn1;
        height:20;
        width:60;
        text:"按钮";
        onClicked: {
        }
    }
    Label{
        id:label1;
        text:"初始值";
        anchors.left:btn1.right;
        anchors.leftMargin: 12;
    }
}
