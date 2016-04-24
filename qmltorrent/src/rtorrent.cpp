#include "rtorrent.h"
#include "rtorrent_model.h"
#include "rtorrent_proxy_model.h"

RTorrent::RTorrent(const QUrl& url, QObject *parent)
: QObject(parent) {
  xmlrpc_client_ = new XMLRpcClient(url, this);

  source_model_ = new RTorrentModel(xmlrpc_client_, this);
  proxy_model_ = new RTorrentProxyModel(this);

  proxy_model_->setSourceModel(source_model_);
  proxy_model_->setSortByCreationDate(true);
}

QAbstractItemModel* RTorrent::torrent_model() {
  return proxy_model_;
}

void RTorrent::refresh() {
  source_model_->updateDownloadList();
}

bool RTorrent::load(const QUrl& torrent_url) {
  QByteArray torrent_content;
  if (torrent_url.scheme() == QString::fromAscii("file")) {
    QFile file(torrent_url.toLocalFile());
    if (!file.open(QIODevice::ReadOnly)) {
      return false;
    }
    torrent_content = file.readAll();
  }

  // check torrent_content
  if (torrent_content.length() < 3
      || !torrent_content.startsWith("d8:")) {
    return false;
  }

  QList<xmlrpc::Variant> params;
  params << torrent_content;
  xmlrpc_client_->request(params, "load_raw_start", this, "loadFinished");
  return true;
}

void RTorrent::loadFinished(QVariant result) {
  qDebug() << "load torrent ok" << result;
}
