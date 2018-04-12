#include "hello.pb.h"
#include "hello.grpc.pb.h"

#include "grpc_async_processor.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>

#include <iostream>

typedef Greeter::WithAsyncMethod_SayHello2<Greeter::Service> GreeterHybridService;

class HelloProcessor :
    public AsyncRequestProcessor<GreeterHybridService, HelloRequest, HelloReply> {
public:
  HelloProcessor(GreeterHybridService* service, grpc::ServerCompletionQueue* cq)
    : AsyncRequestProcessor(*service, &GreeterHybridService::RequestSayHello2, cq) {
  }

  void ProcessRequest(const HelloRequest& req) {
    std::cout << "SayHello: " << req.name() << std::endl;
    HelloReply reply;
    reply.set_message("Hello " + req.name());
    Finish(reply);
  }
};

class HelloServiceImpl : public GreeterHybridService {
public:
  grpc::Status SayHello(grpc::ServerContext*, const HelloRequest* request,
                        HelloReply* reply) override {

    std::cout << "SayHello: " << request->name() << std::endl;
    reply->set_message("hello " + request->name());
    return grpc::Status::OK;
  }

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

int main(int argc, char* argv[]) {
  grpc_init();

  HelloServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());

  service.RegisterTo(builder);
  //builder.RegisterService(&service);

  auto server = builder.BuildAndStart();
  service.Run();

  //server->Wait();

  grpc_shutdown();
  return 0;
}
