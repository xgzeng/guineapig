#pragma once
#include <grpc++/server.h>

class CompletionQueueItem {
public:
  virtual void Proceed(bool ok, int small_tag) = 0;

  CompletionQueueItem() = default;

  // uncopyable and unmovable
  CompletionQueueItem(const CompletionQueueItem&) = delete;
  CompletionQueueItem(CompletionQueueItem&&) = delete;

  CompletionQueueItem& operator=(const CompletionQueueItem&) = delete;
  CompletionQueueItem& operator=(CompletionQueueItem&&) = delete;
  
  void* make_tag(int small_tag = 0) const {
    uintptr_t p = reinterpret_cast<uintptr_t>(this);
    assert((p & 0x03) == 0); // assert lowest 2 bits is zero
    assert(small_tag >= 0 && small_tag <= 3);
    return reinterpret_cast<void*>(p | small_tag);
  }

protected:
  ~CompletionQueueItem() = default;
};

template<typename ASYNC_SERVICE, typename REQUEST_TYPE, typename REPLY_TYPE>
class AsyncRequestProcessor : public CompletionQueueItem {
  typedef void (ASYNC_SERVICE::* INITIATE_METHOD)(
      grpc::ServerContext* context,
      REQUEST_TYPE* request,
      grpc::ServerAsyncResponseWriter<REPLY_TYPE>*,
      grpc::CompletionQueue*,
      grpc::ServerCompletionQueue*,
      void *);

public:
  AsyncRequestProcessor(ASYNC_SERVICE& svc, INITIATE_METHOD init_method,
                        grpc::ServerCompletionQueue* cq)
      : service_(svc), init_method_(init_method), cq_(cq) {
  }

  virtual ~AsyncRequestProcessor() = default;

  void Initiate() {
    assert(status_ == CREATED);
    status_ = WAIT_FOR_REQUEST;
    (service_.*init_method_)(&ctx_, &request_, &writer_, cq_, cq_, make_tag());
  }

  void Proceed(bool ok, int tag) override {
    if (status_ == WAIT_FOR_REQUEST) {
      status_ = PROCESSING;
      this->HandleRequest(request_);
    } else if (status_ == WAIT_FOR_FINISH) {
      status_ = FINISHED;
      this->HandleFinisheResult();
      delete this;
    } else {
      assert(false);
    }
  }

  virtual void HandleRequest(const REQUEST_TYPE& request) = 0;

  virtual void HandleFinisheResult() {}
  
protected:
  void Finish(const REPLY_TYPE& reply) {
    assert(status_ == PROCESSING);
    status_ = WAIT_FOR_FINISH;
    writer_.Finish(reply, grpc::Status::OK, make_tag());
  }

private:
  grpc::ServerContext ctx_;
  grpc::ServerCompletionQueue* cq_;
  grpc::ServerAsyncResponseWriter<REPLY_TYPE> writer_{&ctx_};

  ASYNC_SERVICE& service_;
  INITIATE_METHOD init_method_;
  REQUEST_TYPE request_;

  enum Status {
    CREATED, WAIT_FOR_REQUEST,
    PROCESSING,
    WAIT_FOR_FINISH,
    FINISHED
  } status_ = CREATED;
};

/// Stream Reply
template<typename ASYNC_SERVICE, typename REQUEST_TYPE, typename REPLY_TYPE>
class AsyncHandler_StreamReply : public CompletionQueueItem {
  typedef void (ASYNC_SERVICE::* INITIATE_METHOD) (
      grpc::ServerContext* context,
      REQUEST_TYPE* request, 
      grpc::ServerAsyncWriter<REPLY_TYPE>*,
      grpc::CompletionQueue*,
      grpc::ServerCompletionQueue*, void *);

public:
  AsyncHandler_StreamReply(ASYNC_SERVICE& svc,
                           INITIATE_METHOD init_method,
                           grpc::ServerCompletionQueue* cq)
      : service_(svc), init_method_(init_method), cq_(cq) {
    ctx_.AsyncNotifyWhenDone(make_tag(1));
  }

  virtual ~AsyncHandler_StreamReply() = default;

  void Initiate() {
    assert(status_ == CREATED);
    status_ = WAIT_FOR_REQUEST;
    (service_.*init_method_)(&ctx_, &request_, &writer_, cq_, cq_, make_tag());
  }

  void Proceed(bool ok, int tag) override {
    if (tag == 1) {
      assert(ok);
      if (ctx_.IsCancelled()) {
        printf("operation aborted\n");
      } else {
        printf("operation is done\n");
      }
      return;
    }

    if (status_ == WAIT_FOR_REQUEST) {
      assert(ok);
      status_ = PROCESSING;
      this->HandleRequest(request_);
    } else if (status_ == WAIT_FOR_WRITE_RESULT) {
      // write operation is over
      status_ = PROCESSING;
      this->HandleWriteResult(ok);
    } else if (status_ == WAIT_FOR_FINISH_RESULT) {
      status_ = FINISHED;
      this->HandleFinishResult(ok);
      delete this;
    } else {
      assert(false);
    }
  }

  virtual void HandleRequest(const REQUEST_TYPE& request) {}

  virtual void HandleWriteResult(bool ok) {}

  virtual void HandleFinishResult(bool ok) {}
  
protected:
  void Write(const REPLY_TYPE& reply) {
    assert(status_ == PROCESSING);
    status_ = WAIT_FOR_WRITE_RESULT;
    return writer_.Write(reply, make_tag());
  }

  void Finish(const REPLY_TYPE& reply) {
    assert(status_ == PROCESSING);
    status_ = WAIT_FOR_FINISH_RESULT;
    writer_.WriteAndFinish(reply, {}, grpc::Status::OK, make_tag());
  }

  void Finish() {
    assert(status_ == PROCESSING);
    status_ = WAIT_FOR_FINISH_RESULT;
    writer_.Finish(grpc::Status::OK, make_tag());    
  }

private:
  grpc::ServerContext ctx_;
  grpc::ServerCompletionQueue* cq_;
  grpc::ServerAsyncWriter<REPLY_TYPE> writer_{&ctx_};

  ASYNC_SERVICE& service_;
  INITIATE_METHOD init_method_;

  REQUEST_TYPE request_;

  enum Status {
    CREATED, WAIT_FOR_REQUEST, PROCESSING,
    WAIT_FOR_WRITE_RESULT, // waiting for last write operation over
    WAIT_FOR_FINISH_RESULT,
    FINISHED
  } status_ = CREATED;
};
