import QtQuick 2.4
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.4 as Kirigami

// for some reason Canvas3D is not included in the distro-packaged Qt 5.11
// import QtCanvas3D 1.1
// import "glcode.js" as GLCode


Kirigami.ApplicationWindow {
    
    id: root
    
    title: qsTr("Exechon")
    width: 16*600/9
    height: 600
    visible: true
    
    property bool editingRobot: false
    
    globalDrawer: Kirigami.GlobalDrawer {
        title: "Exechon"
        //titleIcon: "applications-development"
        bannerImageSource: "res/menu-header.jpg"
        
        actions: [
        Kirigami.Action {
            text: "Connect"
            iconName: "network-connect"
            shortcut: "F2"
        },
        Kirigami.Action {
            text: "Disconnect"
            iconName: "network-disconnect"
            shortcut: "F3"
        },
        Kirigami.Action {
            text: "Edit robot parameters..."
            iconName: "preferences-other"
            shortcut: "ctrl+R"
            onTriggered: {
                pageStack.push(robotParametersDialog, null, null)
                editingRobot = true
            }
        },
        Kirigami.Action {
            text: "About"
            iconName: "help-about"
            shortcut: "F1"
            onTriggered: console.log("About")
        }
        ]
    }
    
    pageStack.initialPage: mainPageComponent
    
    Component {
        id: mainPageComponent
        
        Item
        {
            
            QQC2.Pane
            {
                id: sidebar
                
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 300
                
                Column
                {
                    width: parent.width
                    
                    QQC2.Label { text: "Prismatic Displacement" }
                    
                    CoordinateRow {}
                    
                    Item {height: 10; width: 1} // spacer
                    
                    QQC2.Label { text: "Platform Position" }
                    
                    CoordinateRow {}
                }
            }
            
            Item
            {
                id: sceneContainer
                anchors.left: parent.left
                anchors.right: sidebar.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom         
                
//                 Rectangle
//                 {
//                     color: "royalblue"
//                     anchors.fill: parent
//                 }
                ExechonScene{
                    id: scene
                    anchors.fill: parent
                }
            }
            
            
            MouseArea
            {
                anchors.fill: parent
                visible: editingRobot
                
                Rectangle
                {
                    color: "black"
                    opacity: 0.5
                    visible: editingRobot
                    anchors.fill: parent
                }
                
            }
            
//             contextualActions: [
//             Kirigami.Action {
//                 iconName: "edit"
//                 text: "Action text"
//                 onTriggered: {
//                     // do stuff
//                 }
//             },
//             Kirigami.Action {
//                 iconName: "edit"
//                 text: "Action text"
//                 onTriggered: {
//                     // do stuff
//                 }
//             }
//             ]
        }
    }
    
//     contextDrawer: Kirigami.ContextDrawer
//     {}
    
    Component
    {
        id: robotParametersDialog
        
        Kirigami.Page
        {
            id: settingsPage
            
            Column
            {
                anchors.fill: parent
                
                Kirigami.Heading
                {
                    text: "Edit robot parameters"
                }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm A Position" }
                
                CoordinateRow {}
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm B Position" }
                
                CoordinateRow {}
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm C Position" }
                
                CoordinateRow {}
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "A-C Joint Distance" }
                
                CoordinateRow {}
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "B Joint Distance to A-C line" }
                
                CoordinateRow {}
                
                
            }
            
            QQC2.Button
            {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                
                id: robot_param_cancel
                text: "Cancel"
                onClicked: 
                {
                    editingRobot = false;
                    root.pageStack.pop()
                }
                
            }
            
            QQC2.Button
            {
                anchors.right: robot_param_cancel.left
                anchors.bottom: parent.bottom
                anchors.rightMargin: 15
                
                id: robot_param_ok
                text: "OK"
                onClicked: 
                {
                    editingRobot = false;
                    root.pageStack.pop()
                }
                
            }
        }
        
    }
    
    /*
    Canvas3D {
        id: canvas3d
        anchors.fill: parent
        focus: true

        onInitializeGL: {
            GLCode.initializeGL(canvas3d);
        }

        onPaintGL: {
            GLCode.paintGL(canvas3d);
        }

        onResizeGL: {
            GLCode.resizeGL(canvas3d);
        }
    }
    */
}
