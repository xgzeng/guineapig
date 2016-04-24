#ifndef _QMLTORRENT_RTORRENT_VIEW_H
#define _QMLTORRENT_RTORRENT_VIEW_H

#include <QDeclarativeView>
#include "rtorrent.h"

class RTorrentView : public QDeclarativeView {
  Q_OBJECT
public:
  RTorrentView(const QUrl& rtorrent_url,
               QWidget* parent = 0);

private:
  virtual void dragEnterEvent(QDragEnterEvent * event);
//  virtual void dragLeaveEvent(QDragLeaveEvent * event);
  virtual void dragMoveEvent(QDragMoveEvent * event);
  virtual void dropEvent(QDropEvent * event);
  RTorrent * rtorrent_;
  QTimer *timer_;
};

#endif // _QMLTORRENT_RTORRENT_VIEW_H
