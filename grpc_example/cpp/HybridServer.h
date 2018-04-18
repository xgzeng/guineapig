#pragma once

#include "hello.pb.h"
#include "hello.grpc.pb.h"

typedef Greeter::WithAsyncMethod_SayHello3<
        Greeter::WithAsyncMethod_SayHello2<Greeter::Service>> GreeterHybridService;


class HybridHelloService : public GreeterHybridService {
public:
  grpc::Status SayHello(grpc::ServerContext*, const HelloRequest* request,
                        HelloReply* reply) override;

  void RegisterTo(grpc::ServerBuilder& builder);

  void WaitNextHello2();

  void WaitNextHello3();

  void Run();

private:
  std::unique_ptr<grpc::ServerCompletionQueue> cq_;
};