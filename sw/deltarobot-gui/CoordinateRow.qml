import QtQuick 2.4
import QtQuick.Controls 2.0 as QQC2

Row
{
    id: row
    width: parent.width
    spacing: 5
    property int tfw: (width - 3*spacing) / 3
    
    QQC2.TextField
    {
        id: tf_x
        width: tfw
        text: "x"
    }
    QQC2.TextField
    {
        id: tf_y
        width: tfw 
        text: "y"
    }
    QQC2.TextField
    {
        id: tf_z
        width: tfw 
        text: "z"
    }
    
}
