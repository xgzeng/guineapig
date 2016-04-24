#include "rtorrent_view.h"
#include <QDebug>
#include <QDragEnterEvent>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QMessageBox>

RTorrentView::RTorrentView(const QUrl& rtorrent_url, QWidget* parent)
: QDeclarativeView(parent) {
  setAcceptDrops(true);

  rtorrent_ = new RTorrent(rtorrent_url, this);
  rtorrent_->refresh();

  engine()->addImportPath("qml");
  setResizeMode(QDeclarativeView::SizeRootObjectToView);
  rootContext()->setContextProperty(
      QString::fromAscii("rtorrent"), rtorrent_);

  setSource(QUrl::fromLocalFile("qml/main.qml"));

  timer_ = new QTimer(this);
  timer_->setInterval(5000);
  connect(timer_, SIGNAL(timeout()), rtorrent_, SLOT(refresh()));

  timer_->start();
}

void RTorrentView::dragEnterEvent(QDragEnterEvent * event) {
//  qDebug() << "mimeData = " << event->mimeData()->formats();
  if (event->mimeData()->hasUrls()) {
    event->accept();
//    QList<QUrl> url_list = event->mimeData()->urls();
//    Q_FOREACH(QUrl url, url_list) {
//      qDebug() << url;
//    }
  }
}

void RTorrentView::dragMoveEvent(QDragMoveEvent * event) {
  if (event->mimeData()->hasUrls()) {
    event->accept();
  }
}

void RTorrentView::dropEvent(QDropEvent * event) {
  QList<QUrl> url_list = event->mimeData()->urls();
  Q_FOREACH(QUrl url, url_list) {
    if (!rtorrent_->load(url)) {
      QMessageBox::information(this, "",
          tr("Can't Load ") + url.toString());
    }
  }
}
