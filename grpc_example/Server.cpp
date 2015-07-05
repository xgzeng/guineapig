#include "hello.pb.h"
#include "hello.grpc.pb.h"
#include <grpc++/server_builder.h>
#include <grpc++/server_credentials.h>
#include <grpc++/server.h>
#include <iostream>

class HelloServiceImpl : public Greeter::Service {
  grpc::Status SayHello(grpc::ServerContext*, const HelloRequest* request,
                        HelloReply* reply) override {

    std::cout << "SayHello: " << request->name() << std::endl;
    reply->set_message("hello " + request->name());
    return grpc::Status::OK;
  }

};

int main(int argc, char* argv[]) {
  grpc_init();

  HelloServiceImpl service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort("0.0.0.0:50051", grpc::InsecureServerCredentials());
  builder.RegisterService(&service);

  auto server = builder.BuildAndStart();
  server->Wait();

  grpc_shutdown();
  return 0;
}

