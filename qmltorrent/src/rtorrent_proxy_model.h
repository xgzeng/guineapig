#ifndef _QMLTORRENT_RTORRENT_PROXY_MODEL_H
#define _QMLTORRENT_RTORRENT_PROXY_MODEL_H

#include <QSortFilterProxyModel>

class RTorrentProxyModel : public QSortFilterProxyModel {
  Q_OBJECT
public:
  explicit RTorrentProxyModel(QObject *parent = 0);

  Q_PROPERTY(bool sortByCreationDate
             READ sortByCreationDate WRITE setSortByCreationDate);

  bool sortByCreationDate() {
    return sort_by_creation_date_;
  }

  void setSortByCreationDate(bool);

private:
  bool sort_by_creation_date_;
};

#endif // _QMLTORRENT_RTORRENT_PROXY_MODEL_H
