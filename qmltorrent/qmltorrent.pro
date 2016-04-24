QT += declarative network xml

SOURCES += src/main.cpp \
    src/rtorrent_model.cpp \
    src/rtorrent.cpp \
    src/rtorrent_proxy_model.cpp \
    src/xmlrpc.cpp \
    src/rtorrent_view.cpp

HEADERS += \
    src/rtorrent_model.h \
    src/rtorrent.h \
    src/rtorrent_proxy_model.h \
    src/xmlrpc.h \
    src/rtorrent_view.h

unix:LIBS += -lqxmlrpc
win32:LIBS += qxmlrpc_debug.lib

OTHER_FILES += \
    qml/main.qml \
    qml/Torrent.qml \
    qml/ProgressBar.qml
