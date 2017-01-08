import QtQuick 1.0

Rectangle {
    width: 360
    height: 240

    // test data model
    ListModel {
      id: sample_data
      ListElement {
        name: "abcdefghijklmnopqrstuvwxyz"; state: 0;
        bytes_total: 100; bytes_done: 0;
        completed: true;
        bytes_uploaded: 30
      }
      ListElement {
        name: "def"; state: 0;
        bytes_total: 100; bytes_done: 50;
        completed: true;
        bytes_uploaded: 20
      }
      ListElement { name: "hig";
        state: 1;
        bytes_total: 100; bytes_done: 100;
        completed: true;
        bytes_uploaded: 20
      }
    }

    // view filter

    // torrent list
    Component {
      id: torrentDelegate
      Torrent {
        id: item_view
        name: model.name
        progress: bytes_done/bytes_total
        opened: model.state == 1
        completed: model.completed
        bytes_done: model.bytes_done
        bytes_total:  model.bytes_total
        bytes_uploaded: model.bytes_uploaded
      }
    }

    ListView {
      anchors.fill:  parent
//      model: sample_data
      model: rtorrent.torrent_model
      delegate: torrentDelegate
    }

    // total bandwidth usage graph
}
