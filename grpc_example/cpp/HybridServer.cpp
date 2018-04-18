#include "HybridServer.h"

#include "grpc_async_processor.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>

#include <iostream>
#include <thread>

class HelloProcessor :
    public AsyncRequestProcessor<GreeterHybridService, HelloRequest, HelloReply> {
  HybridHelloService& service_;
public:
  HelloProcessor(HybridHelloService& service, grpc::ServerCompletionQueue* cq)
    : AsyncRequestProcessor(service, &GreeterHybridService::RequestSayHello2, cq),
      service_(service) {
  }

  void HandleRequest(const HelloRequest& req) override {
    service_.WaitNextHello2();

    std::cout << "SayHello2: " << req.name() << std::endl;
    HelloReply reply;
    reply.set_message("Hello " + req.name());
    Finish(reply);
  }
};

class Hello3Processor :
    public AsyncHandler_StreamReply<GreeterHybridService, HelloRequest, HelloReply> {
  HybridHelloService& service_;
public:
  Hello3Processor(HybridHelloService& service, grpc::ServerCompletionQueue* cq)
    : AsyncHandler_StreamReply(service, &GreeterHybridService::RequestSayHello3, cq),
      service_(service) {
  }

  ~Hello3Processor() {
    if (thr_.joinable()) thr_.join();
  }

  void HandleRequest(const HelloRequest& req) override {
    service_.WaitNextHello3();

    auto name = req.name();
    thr_ = std::thread([this, name]{
      for (int i = 0; i < 10; ++i) {
        HelloReply reply;
        reply.set_message("Hello " + name);
        this->Write(reply);
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
      this->Finish();
    });
  }

  void HandleWriteResult(bool ok) override {
    if (!ok) printf("Write error\n");
  }

  std::thread thr_;
};

grpc::Status HybridHelloService::SayHello(grpc::ServerContext*,
    const HelloRequest* request, HelloReply* reply) {
  std::cout << "SayHello: " << request->name() << std::endl;
  reply->set_message("hello " + request->name());
  return grpc::Status::OK;
}

void HybridHelloService::RegisterTo(grpc::ServerBuilder& builder) {
  builder.RegisterService(this);
  cq_ = builder.AddCompletionQueue();
}

void HybridHelloService::WaitNextHello2() {
  printf("WaitNextHello2\n");
  auto p = new HelloProcessor(*this, cq_.get());
  p->Initiate();
}

void HybridHelloService::WaitNextHello3() {
  printf("WaitNextHello3\n");
  auto p = new Hello3Processor(*this, cq_.get());
  p->Initiate();
}

void HybridHelloService::Run() {
  WaitNextHello2();
  WaitNextHello3();

  void* tag;  // uniquely identifies a request.
  bool ok;
  while (true) {
    assert(cq_->Next(&tag, &ok));

    auto p = reinterpret_cast<uintptr_t>(tag) & (~(0x03)); // retrieve pointer part
    int small_tag = reinterpret_cast<uintptr_t>(tag) & 0x03;
    reinterpret_cast<CompletionQueueItem*>(p)->Proceed(ok, small_tag);
  }
}

int main(int argc, char* argv[]) {
  grpc_init();

  HybridHelloService service;

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
