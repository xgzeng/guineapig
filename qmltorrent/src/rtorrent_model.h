#ifndef _QMLTORRENT_RTORRENT_MODEL_H
#define _QMLTORRENT_RTORRENT_MODEL_H

#include <QObject>
#include <QVariant>
#include <QUrl>
#include <QDateTime>
#include <QAbstractListModel>
#include "xmlrpc.h"

class RTorrentModel : public QAbstractListModel {
  Q_OBJECT
public:
  RTorrentModel(XMLRpcClient*, QObject* parent = 0);

  enum {
    ROLE_HASH = Qt::UserRole,
    ROLE_TORRENT_NAME,
    ROLE_TORRENT_STATE,
    ROLE_BYTES_TOTAL,
    ROLE_BYTES_DONE,
    ROLE_COMPLETED,
    ROLE_BYTES_UPLOADED,
    ROLE_CREATION_DATE,
    ROLE_UP_RATE,
    ROLE_DOWN_RATE
  };

  virtual int rowCount(const QModelIndex&) const;
  virtual QVariant data(const QModelIndex&, int role) const;

  void updateDownloadList();

private slots:
  void download_list_finished(QVariant result);

private:
  XMLRpcClient* xmlrpc_client_;

  struct TorrentInfo {
    QString hash;
    QString name;
    int state;
    int bytes_total;
    int bytes_done;
    bool completed;
    int bytes_uploaded;
    QDateTime creation_date;
    int up_rate;
    int down_rate;
  };

  QList<TorrentInfo> torrent_list_;
};

#endif // _QMLTORRENT_RTORRENT_MODEL_H
