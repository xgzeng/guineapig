import QtQuick 1.0

Rectangle {
  property alias text: inside_text.text
  property real value: 0.0
  property alias color: bar.color

  Rectangle {
    id: bar
    anchors.left: parent.left
    anchors.top:  parent.top
    anchors.bottom: parent.bottom
    width: parent.width * value
  }

  Text {
    id: inside_text
    anchors.fill: parent
    horizontalAlignment: Text.AlignHCenter
  }
}
