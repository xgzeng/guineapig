#pragma once
#include <grpc++/server.h>

class CompletionQueueItem {
public:
  virtual void Proceed() = 0;

  CompletionQueueItem() = default;

  // uncopyable and unmovable
  CompletionQueueItem(const CompletionQueueItem&) = delete;
  CompletionQueueItem(CompletionQueueItem&&) = delete;

  CompletionQueueItem& operator=(const CompletionQueueItem&) = delete;
  CompletionQueueItem& operator=(CompletionQueueItem&&) = delete;
  
protected:
  ~CompletionQueueItem() = default;
};

template<typename ASYNC_SERVICE, typename REQUEST_TYPE, typename REPLY_TYPE>
class AsyncRequestProcessor : public CompletionQueueItem {
  typedef void (ASYNC_SERVICE::* REQUESTOR)(grpc::ServerContext* context,
                                            REQUEST_TYPE* request, 
                                            grpc::ServerAsyncResponseWriter<REPLY_TYPE>*,
                                            grpc::CompletionQueue*,
                                            grpc::ServerCompletionQueue*, void *);
public:
  AsyncRequestProcessor(ASYNC_SERVICE& svc, REQUESTOR requestor,
                        grpc::ServerCompletionQueue* cq)
      : service_(svc), requestor_(requestor), cq_(cq) {
  }

  virtual ~AsyncRequestProcessor() = default;

  void Initiate() {
    assert(status_ == CREATED);
    status_ = WAIT_FOR_REQUEST;
    (service_.*requestor_)(&ctx_, &request_, &responder_, cq_, cq_, this);
  }

  void Proceed() override {
    if (status_ == WAIT_FOR_REQUEST) {
      if (request_received_handler_) request_received_handler_();
      status_ = PROCESSING;
      this->ProcessRequest(request_);
    } else if (status_ == WAIT_FOR_FINISH) {
      this->Finished();
      if (request_finished_handler_) request_finished_handler_();
      delete this;
    } else {
      assert(false);
    }
  }

  virtual void ProcessRequest(const REQUEST_TYPE& request) = 0;

  virtual void Finished() {}

  template<typename T>
  void OnRequestReceived(T&& handler) {
    assert(status_ == CREATED);
    request_received_handler_ = std::forward<T>(handler);
  }

  template<typename T>
  void OnRequestFinished(T&& handler) {
    assert(status_ == CREATED);
    request_finished_handler_ = std::forward<T>(handler);
  }
  
protected:
  void Finish(const REPLY_TYPE& reply) {
    assert(status_ == PROCESSING);
    // reply_ = reply;
    status_ = WAIT_FOR_FINISH;
    responder_.Finish(reply, grpc::Status::OK, this);
  }

private:
  grpc::ServerContext ctx_;
  grpc::ServerCompletionQueue* cq_;
  grpc::ServerAsyncResponseWriter<REPLY_TYPE> responder_{&ctx_};

  ASYNC_SERVICE& service_;
  REQUESTOR requestor_;
  REQUEST_TYPE request_;

  enum Status {
    CREATED, WAIT_FOR_REQUEST, PROCESSING, WAIT_FOR_FINISH
  } status_ = CREATED;

  std::function<void()> request_received_handler_;
  std::function<void()> request_finished_handler_;
};
