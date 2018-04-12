#include "hello.pb.h"
#include "hello.grpc.pb.h"
#include <iostream>
#include <cassert>
#include <grpc++/channel.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/security/credentials.h>
//#include <grpc++/channel_arguments.h>
//#include <grpc++/status.h>

int main(int argc, char* argv[]) {
  grpc_init();

  auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
  assert(channel);

  auto hello_client = Greeter::NewStub(channel);
  assert(channel);


  HelloRequest request;
  request.set_name("world");

  HelloReply reply;

  {
    grpc::ClientContext context;
    auto status = hello_client->SayHello(&context, request, &reply);

    if (status.ok()) {
      std::cout << "grpc success: " << reply.message() << std::endl;
    } else {
      std::cout << "grpc failed" << std::endl;
    }
  }

  {
    request.set_name("hbrid world");
    grpc::ClientContext context;
    auto status = hello_client->SayHello2(&context, request, &reply);
    if (status.ok()) {
      std::cout << "grpc success: " << reply.message() << std::endl;
    } else {
      std::cout << "grpc failed" << std::endl;
    }
  }

  grpc_shutdown();
  return 0;
}
