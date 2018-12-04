import QtQuick 2.4
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.4 as Kirigami


Kirigami.ApplicationWindow {
    
    id: root
    
    title: qsTr("Exechon")
    width: 1280
    height: 720
    visible: true
    
    property bool editingRobot: false
    property vector3d vecWADims: Qt.vector3d(0.5, 0.8, 0.5)
    property vector3d displacementNow: Qt.vector3d(0, 0, 0)
    property var consoleObj: null

    Connections
    {
        target: conn

        /*
         *     void connectedChanged( bool c );
         *
         *     void ackReceived( char opcode );
         *     void nackReceived( char opcode );
         *     void homingFinished();
         *     void moveFinished();
         *     void statusReply(int status, bool stopa, float a, bool stopb, float b, bool stopc, float c);
         *     void generalError();
         *
         *     void messageReceivedStr( QString msg );
         *     void messageSentStr( QString msg );
         */

        onConnectedChanged:
        {
            if(b)
                toConsole("### Now Connected");
            else
                toConsole("### Now Disconnected");
        }

        onMessageSentStr:
        {
            toConsole( "SENT: "+msg);
        }

        onMessageReceivedStr:
        {
            toConsole( "RECV: "+msg);
        }

    }


    function toConsole( line )
    {
        if(!consoleObj)
            return;
        consoleObj.text += line;
        if(!String(line).endsWith("\n"))
            consoleObj.text += "\n";
        if(consoleObj.text.len > 300)
            consoleObj.text = consoleObj.text.substr(-300);
    }
    
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

            Connections
            {
                target: conn

                onStatusReply:
                {
                    var statusesStr = ["IDLE", "HOMING", "MOVING", "ERROR"];
                    var statusesColor = ["darkgreen", "blue", "orange", "red"];
                    statusLabel.text = statusesStr[status];
                    statusLabel.color = statusesColor[status];
                    pd.value = Qt.vector3d(a,b,c);
                    stopALED.opacity = stopa? 1.0 : 0.5;
                    stopBLED.opacity = stopb? 1.0 : 0.5;
                    stopCLED.opacity = stopc? 1.0 : 0.5;
                    toConsole(status);
                }
            }
            
            Item
            {
                id: bottomConsole
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: Math.max(150, parent.height * 0.25)
                clip: true

                Rectangle
                {
                    id: consoleBg
                    anchors.fill: parent
                    color: "#222"
                }

                TextEdit
                {
                    id: consoleEdit
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: 12
                    font.family: "Iosevka, Hack, Monospace"
                    color: "#ddd"
                    wrapMode: TextEdit.WordWrap
                    selectByKeyboard: true
                    selectByMouse: true
                    readOnly: true
                    text: "deltarobot-gui version 0.1\nby Lucas Camargo\nUniversidade Federal de Santa Catarina\n\n";

                    Component.onCompleted: { consoleObj = consoleEdit; toConsole( conn.connected? "### Connected" : "### Disconnected"  ); if(conn.connected) conn.askStatus(); }

                }

                Rectangle {
                    width: parent.width
                    height: consoleEdit.anchors.margins
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: consoleBg.color }
                        GradientStop { position: 1.0; color: "#22222200" }
                    }
                }
            }

            QQC2.Pane
            {
                id: sidebar
                
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: bottomConsole.top
                width: 300
                
                Column
                {
                    width: parent.width

                    QQC2.Label
                    {
                        id: statusLabel
                        text: "(STATUS)"
                        font.pointSize: 20
                    }

                    Item {height: 10; width: 1} // spacer

                    Row
                    {
                        spacing: 10

                        Rectangle {
                            id: stopALED; width: 10; height: 10; color: "red"; opacity: 0.25; radius: 5
                        }
                        Rectangle {
                            id: stopBLED; width: 10; height: 10; color: "red"; opacity: 0.25; radius: 5
                        }
                        Rectangle {
                            id: stopCLED; width: 10; height: 10; color: "red"; opacity: 0.25; radius: 5
                        }

                    }

                    Item {height: 10; width: 1} // spacer

                    QQC2.Label { text: "Prismatic Displacement" }
                    
                    CoordinateRow 
                    {
                        id: pd
                        value: dmodel.performIK(pp.value)
                        //onEdited: pp.value = dmodel.performFK( value )
                    }
                    
                    Item {height: 10; width: 1} // spacer
                    
                    QQC2.Label { text: "Platform Position"; opacity: 0.5 }
                    
                    CoordinateRow {
                        id: pp
                        value: vecWADims.times(0.5)
                        onEdited: pd.value = dmodel.performIK( value )
                        opacity: 0.5
                    }

                    Item {height: 10; width: 1} // spacer

                    Column
                    {
                        spacing: 10
                        width: parent.width

                        QQC2.Button
                        {
                            text: "Go to Position"
                            width: parent.width
                            onClicked: conn.sendMove( pd.value.x, pd.value.y, pd.value.z )
                        }

                        QQC2.Button
                        {
                            text: "Go Home"
                            width: parent.width
                            onClicked: conn.sendHome()
                        }

                        QQC2.Button
                        {
                            text: "Ask Status"
                            width: parent.width
                            onClicked: conn.askStatus()
                        }

                        QQC2.Button
                        {
                            text: "Cancel"
                            width: parent.width
                            onClicked: conn.sendCancel()
                        }
                    }
                }
            }
            
            Item
            {
                id: sceneContainer
                anchors.left: parent.left
                anchors.right: sidebar.left
                anchors.top: parent.top
                anchors.bottom: bottomConsole.top
                
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
            id: modelImg
            source: "res/ed-mock-2.png"
            anchors.fill: parent
            anchors.margins: 35
            fillMode: Image.PreserveAspectFit
        }
        
        QQC2.Label
        {
            color: "white"
            anchors.top: modelImg.bottom
            anchors.margins: 5
            anchors.horizontalCenter: parent.horizontalCenter
            text: "<i>Source: </i> Z.M. Bi, Y. Jin. <b>Kinematic modeling of Exechon parallel kinematic machine.</b> doi:10.1016/j.rcim.2010.07.006"
        }
        
        Behavior on opacity { NumberAnimation {} }
        opacity: 0
        visible: opacity
        onClicked: opacity = 0
    }

}
