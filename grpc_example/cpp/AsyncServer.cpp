#include "hello.pb.h"
#include "hello.grpc.pb.h"

#include "grpc_async_processor.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>
#include <grpc++/server.h>

#include <cassert>

class AsyncGreeterServiceImpl;

class HelloProcessor :
    public AsyncRequestProcessor<Greeter::AsyncService, HelloRequest, HelloReply> {
public:
  HelloProcessor(Greeter::AsyncService* service, grpc::ServerCompletionQueue* cq)
    : AsyncRequestProcessor(*service, &Greeter::AsyncService::RequestSayHello, cq) {
  }

  void HandleRequest(const HelloRequest& req) override {
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

    // p->OnRequestReceived([this]{
    //   this->CreateNewProcessor(); // wait for next request
    // });

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
      static_cast<CompletionQueueItem*>(tag)->Proceed(ok, 0);
    }
  }

private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
};

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