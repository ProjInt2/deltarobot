import QtQuick 2.4
import QtQuick.Controls 2.0 as QQC2

Row
{
    id: row
    width: parent.width
    spacing: 5
    property int tfw: (width - 3*spacing) / 3
    property vector3d value
    property bool loopLock: false
    onValueChanged: {if(!loopLock) setText(value.x, value.y, value.z);}
    signal edited
    
    QQC2.TextField
    {
        id: tf_x
        width: tfw
        text: "0.0"
        onTextEdited: evalFields()
        color: isNaN(text)? "red" : "black"
    }
    QQC2.TextField
    {
        id: tf_y
        width: tfw 
        text: "0.0"
        onTextEdited: evalFields()
        color: isNaN(text)? "red" : "black"
    }
    QQC2.TextField
    {
        id: tf_z
        width: tfw 
        text: "0.0"
        onTextEdited: evalFields()
        color: isNaN(text)? "red" : "black"
    }
    
    function setText(x, y, z)
    {
        tf_x.text = x
        tf_y.text = y
        tf_z.text = z
    }
    
    function evalFields()
    {
        if(isNaN(tf_x.text) || isNaN(tf_y.text) || isNaN(tf_z.text) )
            return;
        
        // set value withou triggering text change
        loopLock = true;
        value = Qt.vector3d(Number(tf_x.text),
                    Number(tf_y.text),
                    Number(tf_z.text));
        loopLock = false;
        edited();
    }
    
}
