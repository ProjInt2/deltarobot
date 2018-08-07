import QtQuick 2.4
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.4 as Kirigami


Kirigami.ApplicationWindow {
    
    id: root
    
    title: qsTr("Exechon")
    width: 16*600/9
    height: 600
    visible: true
    
    property bool editingRobot: false
    property vector3d vecWADims: Qt.vector3d(0.5, 0.8, 0.5)
    property vector3d displacementNow: Qt.vector3d(0, 0, 0)
    /*
    property vector3d vecArmBPos: Qt.vector3d(0.38, 0.0, 0.25) 
    property vector3d vecArmCPos: Qt.vector3d(0.12, 0.0, 0.4)
    */
    
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
            
            Keys.onEscapePressed: Qt.quit()
            
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
                    
                    CoordinateRow 
                    {
                        id: pd
                        value: dmodel.performIK(pp.value)
                        onEdited: pp.value = dmodel.performFK( value )
                    }
                    
                    Item {height: 10; width: 1} // spacer
                    
                    QQC2.Label { text: "Platform Position" }
                    
                    CoordinateRow {
                        id: pp
                        value: vecWADims.times(0.5)
                        onEdited: pd.value = dmodel.performIK( value )
                    }
                }
            }
            
            Item
            {
                id: sceneContainer
                anchors.left: parent.left
                anchors.right: sidebar.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom         
                
                Rectangle {
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#333" }
                        GradientStop { position: 1.0; color: "#111" }
                    }
                }
                
                ExechonScene{
                    id: scene
                    anchors.fill: parent
                    focus: true
                    
                    workArea: vecWADims
                    jointAPosition: dmodel.armAPos 
                    jointBPosition: dmodel.armBPos
                    jointCPosition: dmodel.armCPos
                    
                    platformPosition: pp.value
                    armDisplacements: pd.value
                }
                
                MouseArea
                {
                    anchors.fill: parent
                    visible: !scene.focus
                    onPressed: {scene.focus = true; mouse.accepted = false}
                    propagateComposedEvents: true
                    acceptedButtons: Qt.AllButtons 
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
                
                QQC2.Label { text: "Work Area Dimensions" }
                CoordinateRow { id: crWADims; value: vecWADims }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm A Position" }
                CoordinateRow { id: crArmAPos; value: dmodel.armAPos }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm B Position" }
                CoordinateRow { id: crArmBPos; value: dmodel.armBPos }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "Arm C Position" }
                CoordinateRow { id: crArmCPos; value: dmodel.armCPos }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "A-C Joint Distance" }
                CoordinateRow { id: crACdist }
                
                Item {height: 10; width: 1} // spacer
                
                QQC2.Label { text: "B Joint Distance to A-C line" }
                CoordinateRow { id: crBdist }
                
                
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
            
            QQC2.Button
            {
                anchors.right: robot_param_ok.left
                anchors.bottom: parent.bottom
                anchors.rightMargin: 45
                
                id: robot_show_model
                text: "Show Model"
                onClicked: 
                {
                    modelModal.opacity = 1
                }
                
            }
        }
        
    }
    
    MouseArea
    {
        id: modelModal
        anchors.fill: parent
        
        Rectangle
        {
            color: "black"
            opacity: 0.8
            anchors.fill: parent
        }
        
        Image
        {
            source: "res/ed-mock-2.png"
            anchors.fill: parent
            anchors.margins: 20
            fillMode: Image.PreserveAspectFit
        }
        
        QQC2.Label
        {
            color: "white"
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            text: "<i>Source: </i> Z.M. Bi, Y. Jin. <b>Kinematic modeling of Exechon parallel kinematic machine.</b> doi:10.1016/j.rcim.2010.07.006"
        }
        
        Behavior on opacity { NumberAnimation {} }
        opacity: 0
        visible: opacity
        onClicked: opacity = 0
    }

}
