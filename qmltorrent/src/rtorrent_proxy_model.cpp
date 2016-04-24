#include "rtorrent_proxy_model.h"
#include "rtorrent_model.h"

RTorrentProxyModel::RTorrentProxyModel(QObject *parent)
: QSortFilterProxyModel(parent) {
  sort_by_creation_date_ = false;
  setDynamicSortFilter(true);
}

void RTorrentProxyModel::setSortByCreationDate(bool value) {
  sort_by_creation_date_ = value;
  if (sort_by_creation_date_) {
    setSortRole(RTorrentModel::ROLE_CREATION_DATE);
    sort(0, Qt::DescendingOrder);
  } else {
  }
}
