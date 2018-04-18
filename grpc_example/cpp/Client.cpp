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
      std::cout << "SayHello: " << reply.message() << std::endl;
    } else {
      std::cout << "SayHello failed" << std::endl;
    }
  }

  { // async operation
    grpc::CompletionQueue cq;

    request.set_name("world");
    grpc::ClientContext context;
    
    auto response_reader = hello_client->AsyncSayHello2(&context, request, &cq);

    HelloReply reply;
    grpc::Status status;

    response_reader->Finish(&reply, &status, (void*)1);

    void* got_tag;
    bool ok = false;
    cq.Next(&got_tag, &ok);
    if (ok && got_tag == (void*)1) {
      // check reply and status
      if (status.ok()) {
        std::cout << "SayHello2: " << reply.message() << std::endl;
      } else {
        std::cout << "SayHello2 failed" << std::endl;
      }
    }
  }

  // sync stream reply
  {
    grpc::ClientContext context;
    auto reply_reader = hello_client->SayHello3(&context, request);

    HelloReply reply;
    while (reply_reader->Read(&reply)) {
      std::cout << "SayHello3: " << reply.message() << std::endl;
    }

    reply_reader->Finish();
  }

  grpc_shutdown();
  return 0;
}
