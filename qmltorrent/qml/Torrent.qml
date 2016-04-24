import QtQuick 1.0

Rectangle {
  property alias name: name_label.text
  property alias progress: progress_bar.value // real value between 0-1
  property bool opened: false
  property bool completed: false
  property int bytes_done: 0
  property int bytes_total: 0
  property int bytes_uploaded: 0

  function friendly_bytes(bytes_count) {
    var float_size;
    var unit;
    if (bytes_count < 1024) {
      float_size = bytes_count;
      unit = "B";
    } else if (bytes_count < 1024*1024) {
      float_size = bytes_count/1024;
      unit = "K";
    } else if (bytes_count < 1024*1024*1024) {
      float_size = bytes_count/(1024*1024);
      unit = "M";
    } else {
      float_size = bytes_count/(1024*1024*1024);
      unit = "G";
    }
    return Math.round(float_size*10)/10 + unit;
  }

  function status_string() {
    if (!opened)
      return "closed";

    if (completed)
      return "seeding";
    else
      return "downloading(" + Math.round(progress*100) +"%)";
  }

  border.color: "red"
  border.width: 2
  height: name_label.height + progress_bar.height
  anchors.left: parent.left
  anchors.right: parent.right

  Column {
    width: parent.width
    Text {
      id: name_label
      width: parent.width
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
      width: parent.width
      height: 30

      ProgressBar {
        id: progress_bar
        width: parent.width
        height: 30
        color: {
          if (opened)
            "lightgreen";
          else
            "yellow";
        }

      }

      Row {
        Text {
          width: 100
          height: 24
          verticalAlignment: Qt.AlignVCenter
          text: status_string()
        }

        Image {
          height: 24
          fillMode: Image.PreserveAspectFit
          source: "images/download.png"
        }

        Text {
          height: 24
          verticalAlignment: Qt.AlignVCenter
          text: (completed ? "" : down_rate+"B/s ")
                + friendly_bytes(bytes_done)
                + "/" + friendly_bytes(bytes_total)
        }

        Image {
          height: 24
          fillMode: Image.PreserveAspectFit
          source: "images/upload.png"
        }

        Text {
          height: 24
          verticalAlignment: Qt.AlignVCenter
          text: up_rate+ "B/s  " + friendly_bytes(bytes_uploaded)
        }
      }

      Image {
        anchors.right: parent.right
        source: "images/dropdown.png"

        MouseArea {
          anchors.fill: parent
          onClicked: {
            console.log("mouse clicked");
          }
        }
      }

    }
  }
}
