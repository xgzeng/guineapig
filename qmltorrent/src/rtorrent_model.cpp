#include "rtorrent_model.h"
#include <QDebug>

RTorrentModel::RTorrentModel(XMLRpcClient* client, QObject* parent)
: QAbstractListModel(parent) {
  xmlrpc_client_ = client;

  QHash<int, QByteArray> role_names;
  role_names[ROLE_HASH] = "hash";
  role_names[ROLE_TORRENT_NAME] = "name";
  role_names[ROLE_TORRENT_STATE] = "state";
  role_names[ROLE_BYTES_TOTAL] = "bytes_total";
  role_names[ROLE_BYTES_DONE] = "bytes_done";
  role_names[ROLE_COMPLETED] = "completed";
  role_names[ROLE_BYTES_UPLOADED] = "bytes_uploaded";
  role_names[ROLE_UP_RATE] = "up_rate";
  role_names[ROLE_DOWN_RATE] = "down_rate";

  setRoleNames(role_names);
}

void RTorrentModel::updateDownloadList() {
  // use d.multicall to get all download item status efficently
  QList<xmlrpc::Variant> params;
  params << QString::fromAscii("main")
         << QString::fromAscii("d.get_hash=")
         << QString::fromAscii("d.get_name=")
         << QString::fromAscii("d.get_state=")  // 0 stopped 1 started
         << QString::fromAscii("d.get_size_bytes=")
         << QString::fromAscii("d.get_bytes_done=")
         << QString::fromAscii("d.get_complete=") // 0 not completed, 1 completed
         << QString::fromAscii("d.get_up_total=")
         << QString::fromAscii("d.get_creation_date=")
         << QString::fromAscii("d.get_up_rate=")
         << QString::fromAscii("d.get_down_rate=");
  xmlrpc_client_->request(params, "d.multicall",
                          this, "download_list_finished");
}

void RTorrentModel::download_list_finished(QVariant result) {
//  qDebug() << "download_list_finished";
  QList<QVariant> list = result.toList();
  Q_FOREACH(QVariant item, list) {
    QList<QVariant> torrent_item = item.toList();

    TorrentInfo ti;
    ti.hash = torrent_item[0].toString();
    ti.name = torrent_item[1].toString();
    ti.state = torrent_item[2].toInt();
    ti.bytes_total = torrent_item[3].toInt();
    ti.bytes_done = torrent_item[4].toInt();
    ti.completed = torrent_item[5].toInt() == 1;
    ti.bytes_uploaded = torrent_item[6].toInt();
    ti.creation_date = QDateTime::fromTime_t(torrent_item[7].toInt());
    ti.up_rate = torrent_item[8].toInt();
    ti.down_rate = torrent_item[9].toInt();

//    qDebug() << ti.name << ": " << ti.creation_date;

    Q_ASSERT(ti.state == 0 || ti.state == 1);
    //
    int i;
    for (i = 0; i < torrent_list_.size(); ++i) {
      if (torrent_list_[i].hash == ti.hash)
        break;
    }

    if (i < torrent_list_.size()) {
      // update torrent information
      torrent_list_[i] = ti;
      emit dataChanged(index(i, 0), index(i,0));
    } else {
      // got new torrent
      beginInsertRows(QModelIndex(), torrent_list_.size(), torrent_list_.size());
      torrent_list_.append(ti);
      endInsertRows();
    }
  }
}

int RTorrentModel::rowCount(const QModelIndex&) const {
  return torrent_list_.size();
}

QVariant RTorrentModel::data(const QModelIndex& index, int role) const {
  QVariant value;
  const TorrentInfo &ti = torrent_list_[index.row()];
  switch(role) {
  case ROLE_HASH:
    value = ti.hash;
    break;
  case ROLE_TORRENT_NAME:
    value = ti.name;
    break;
  case ROLE_TORRENT_STATE:
    value = ti.state;
    break;
  case ROLE_BYTES_TOTAL:
    value = ti.bytes_total;
    break;
  case ROLE_BYTES_DONE:
    value = ti.bytes_done;
    break;
  case ROLE_COMPLETED:
    value = ti.completed;
    break;
  case ROLE_BYTES_UPLOADED:
    value = ti.bytes_uploaded;
    break;
  case ROLE_CREATION_DATE:
    value = ti.creation_date;
    break;
  case ROLE_UP_RATE:
    value = ti.up_rate;
    break;
  case ROLE_DOWN_RATE:
    value = ti.down_rate;
    break;
  }
  return value;
}
