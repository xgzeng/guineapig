#include "hello.pb.h"
#include "hello.grpc.pb.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>
#include <grpc++/server.h>

#include <cassert>

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

class AsyncGreeterServiceImpl;

class HelloProcessor :
    public AsyncRequestProcessor<Greeter::AsyncService, HelloRequest, HelloReply> {
public:
  HelloProcessor(Greeter::AsyncService* service, grpc::ServerCompletionQueue* cq);

  void ProcessRequest(const HelloRequest& req) {
    HelloReply reply;
    reply.set_message("Hello " + req.name());
    Finish(reply);
  }
};

class AsyncGreeterServiceImpl : public Greeter::AsyncService {
public:
  void RegisterTo(grpc::ServerBuilder& builder) {
    builder.RegisterService(this);
    cq_ = builder.AddCompletionQueue();
  }

  void CreateNewProcessor() {
    auto p = new HelloProcessor(this, cq_.get());

    p->OnRequestReceived([this]{
      this->CreateNewProcessor(); // wait for next request
    });

    p->Initiate();
  }

  void Run() {
    CreateNewProcessor();

    void* tag;  // uniquely identifies a request.
    bool ok;
    while (true) {
      // Block waiting to read the next event from the completion queue. The
      // event is uniquely identified by its tag, which in this case is the
      // memory address of a CallData instance.
      // The return value of Next should always be checked. This return value
      // tells us whether there is any kind of event or cq_ is shutting down.
      assert(cq_->Next(&tag, &ok));
      assert(ok);
      static_cast<CompletionQueueItem*>(tag)->Proceed();
    }
  }

private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
};

HelloProcessor::HelloProcessor(Greeter::AsyncService* service, grpc::ServerCompletionQueue* cq)
    : AsyncRequestProcessor(*service, &Greeter::AsyncService::RequestSayHello, cq) {
}

int main(int argc, char* argv[]) {
  grpc_init();

  AsyncGreeterServiceImpl greeter_service;

  // HelloServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

  greeter_service.RegisterTo(builder);

  auto server = builder.BuildAndStart();

  greeter_service.Run();
  //server->Wait();

  grpc_shutdown();    
}