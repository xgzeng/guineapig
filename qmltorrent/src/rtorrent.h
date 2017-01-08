#ifndef _QMLTORRENT_RTORRENT_H
#define _QMLTORRENT_RTORRENT_H

#include <QObject>
#include <QAbstractItemModel>
#include <QUrl>
#include "xmlrpc.h"

class RTorrentModel;
class RTorrentProxyModel;

class RTorrent : public QObject {
  Q_OBJECT
public:
  RTorrent(const QUrl& url, QObject* parent = 0);

  Q_PROPERTY(QObject* torrent_model READ torrent_model)

  Q_INVOKABLE bool load(const QUrl& torrent_file);

  // download rate, get_down_rate

  // upload rate, get_up_total

  QAbstractItemModel* torrent_model();

public slots:
  void refresh();

private slots:
  void loadFinished(QVariant result);

private:
  RTorrentModel* source_model_;
  RTorrentProxyModel* proxy_model_;
  XMLRpcClient* xmlrpc_client_;
};

#endif // _QMLTORRENT_RTORRENT_H
