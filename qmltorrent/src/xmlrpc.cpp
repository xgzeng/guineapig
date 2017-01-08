#include "xmlrpc.h"

XMLRpcClient::XMLRpcClient(const QUrl &url, QObject* parent)
: QObject(parent), xmlrpc_(new xmlrpc::Client(this)) {
  connect(xmlrpc_, SIGNAL(done(int, QVariant)), SLOT(xmlrpcDone(int, QVariant)));
  connect(xmlrpc_, SIGNAL(failed(int, int, QString)), SLOT(xmlrpcFailed(int, int, QString)));

  Q_ASSERT(url.scheme() == "http");
  xmlrpc_->setHost(url.host(), url.port(80), url.path());
}

void XMLRpcClient::request(QList<xmlrpc::Variant> params, QString methodName,
                           QObject* caller,
                           const char* dh, const char* fh) {
  RequestInfo h = {methodName, caller, dh, fh};
  int requestId = xmlrpc_->request(params, methodName);
  pending_request_[requestId] = h;
}

/*
void RTorrentModel::request(QString methodName, DoneHandler dh, FailedHandler fh) {
  RequestInfo h = {methodName, dh, fh};
  int requestId = xmlrpc_->request(methodName);
  pending_request_[requestId] = h;
}

void RTorrentModel::request(QString methodName, xmlrpc::Variant param1,
                       DoneHandler dh, FailedHandler fh) {
  RequestInfo h = {methodName, dh, fh};
  int requestId = xmlrpc_->request(methodName, param1);
  pending_request_[requestId] = h;
}

void RTorrentModel::request(QString methodName, xmlrpc::Variant param1,
                       xmlrpc::Variant param2,
                       DoneHandler dh, FailedHandler fh) {
  RequestInfo h = {methodName, dh, fh};
  int requestId = xmlrpc_->request(methodName, param1, param2);
  pending_request_[requestId] = h;
}

void RTorrentModel::request(QString methodName, xmlrpc::Variant param1,
                       xmlrpc::Variant param2, xmlrpc::Variant param3,
                       DoneHandler dh, FailedHandler fh) {
  RequestInfo h = {methodName, dh, fh};
  int requestId = xmlrpc_->request(methodName, param1, param2, param3);
  pending_request_[requestId] = h;
}

void RTorrentModel::request(QString methodName, xmlrpc::Variant param1,
                       xmlrpc::Variant param2, xmlrpc::Variant param3,
                       xmlrpc::Variant param4,
                       DoneHandler dh, FailedHandler fh) {
  RequestInfo h = {methodName, dh, fh};
  int requestId = xmlrpc_->request(methodName, param1, param2, param3, param4);
  pending_request_[requestId] = h;
}
*/

void XMLRpcClient::xmlrpcDone(int requestId, QVariant result) {
  QMap<int, RequestInfo>::iterator it;
  it = pending_request_.find(requestId);
  Q_ASSERT(it != pending_request_.end());

  if (!it->doneHandler.empty()) {
    QMetaObject::invokeMethod(it->caller, it->doneHandler.c_str(),
                              Q_ARG(QVariant, result));
  } else {
    qDebug() << it->methodName << " success: " << result;
  }

  pending_request_.erase(it);
}

void XMLRpcClient::xmlrpcFailed(int requestId, int faultCode, QString faultString) {
  QMap<int, RequestInfo>::iterator it;
  it = pending_request_.find(requestId);
  Q_ASSERT(it != pending_request_.end());

  qDebug() << "request " << it->methodName << " failed: " << faultString;

  if (!it->failedHandler.empty()) {
    QMetaObject::invokeMethod(it->caller, it->failedHandler.c_str(),
                              Q_ARG(int, faultCode),
                              Q_ARG(QString, faultString));
  }

  pending_request_.erase(it);
}

