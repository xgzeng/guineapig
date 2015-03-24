#include "hello.pb.h"
#include <iostream>
#include <cassert>
#include <grpc++/channel_interface.h>
#include <grpc++/client_context.h>
#include <grpc++/create_channel.h>
#include <grpc++/credentials.h>
#include <grpc++/channel_arguments.h>
#include <grpc++/status.h>

int main(int argc, char* argv[]) {
  grpc_init();

  auto channel = grpc::CreateChannel("localhost:50051", grpc::InsecureCredentials(), {});
  assert(channel);

  auto hello_client = Greeter::NewStub(channel);
  assert(channel);


  HelloRequest request;
  request.set_name("world");

  HelloReply reply;
  grpc::ClientContext context;

  std::cout << "abc" << std::endl;

  auto status = hello_client->SayHello(&context, request, &reply);
  std::cout << "def" << std::endl;

  if (status.IsOk()) {
    std::cout << "grpc success" << std::endl;
    std::cout << reply.message();
  } else {
    std::cout << "grpc failed" << std::endl;
  }

  grpc_shutdown();
  return 0;
}
