#include <QtGui/QApplication>

#include "rtorrent.h"
#include "rtorrent_view.h"

int main(int argc, char *argv[]) {
  if (argc < 2)
    return -1;
  QApplication app(argc, argv);

  QUrl xmlrpc_endpoint(argv[1]);

  RTorrentView viewer(xmlrpc_endpoint);
  viewer.show();

  return app.exec();
}
