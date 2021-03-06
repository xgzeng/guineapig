import example.GreeterGrpc.GreeterBlockingStub;
import example.Hello.HelloReply;
import example.Hello.HelloRequest;
import io.grpc.ManagedChannel;
import io.grpc.ManagedChannelBuilder;
/*
import io.grpc.ChannelImpl;
import io.grpc.transport.netty.NegotiationType;
import io.grpc.transport.netty.NettyChannelBuilder;
*/

public class Main {
  public static void main(String args[]) {
    ManagedChannel channel = ManagedChannelBuilder.forAddress("localhost", 50051).usePlaintext(true).build();

//	ChannelImpl channel = NettyChannelBuilder.forAddress("localhost", 50051).negotiationType(NegotiationType.PLAINTEXT).build();
    GreeterBlockingStub blockingStub = example.GreeterGrpc.newBlockingStub(channel);

    HelloRequest request = HelloRequest.newBuilder().setName("abc").build();
    HelloReply reply = blockingStub.sayHello(request);
    System.out.println(reply.getMessage());
  }
}
