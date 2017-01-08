#ifndef _QMLTORRENT_XMLRPC_H
#define _QMLTORRENT_XMLRPC_H

#include "qxmlrpc/client.h"

class XMLRpcClient : public QObject {
  Q_OBJECT
public:
  XMLRpcClient(const QUrl &url, QObject* parent);

  void request(QList<xmlrpc::Variant> params, QString methodName,
               QObject *caller,
               const char* success_handler_method = "",
               const char* failed_handler_method = "");

  void request( QString methodName,
                QObject *caller,
                const char* success_handler_method = "",
                const char* failed_handler_method = "");

  void request( QString methodName, xmlrpc::Variant param1,
                QObject *caller,
                const char* success_handler_method = "",
                const char* failed_handler_method = "");

  void request( QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2,
                QObject *caller,
                const char* success_handler_method = "",
                const char* failed_handler_method = "");

  void request( QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2,
                xmlrpc::Variant param3,
                QObject *caller,
                const char* success_handler_method = "",
                const char* failed_handler_method = "");

  void request( QString methodName, xmlrpc::Variant param1, xmlrpc::Variant param2,
                xmlrpc::Variant param3, xmlrpc::Variant param4,
                QObject *caller,
                const char* success_handler_method = "",
                const char* failed_handler_method = "");

private slots:
  void xmlrpcDone(int requestId, QVariant res);
  void xmlrpcFailed(int requestId, int faultCode, QString faultString);

private:
  xmlrpc::Client *xmlrpc_;

  struct RequestInfo {
    QString methodName;
    QObject * caller;
    std::string doneHandler;
    std::string failedHandler;
  };

  QMap<int, RequestInfo> pending_request_;

};

#endif // _QMLTORRENT_XMLRPC_H
