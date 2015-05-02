package netty_example;

import java.nio.CharBuffer;

import com.google.gson.Gson;
import com.google.gson.JsonElement;
import com.google.gson.JsonParser;
import com.google.gson.JsonSerializer;
import com.google.gson.JsonSyntaxException;

import io.netty.bootstrap.ServerBootstrap;
import io.netty.buffer.ByteBuf;
import io.netty.buffer.ByteBufAllocator;
import io.netty.buffer.ByteBufProcessor;
import io.netty.buffer.ByteBufUtil;
import io.netty.buffer.CompositeByteBuf;
import io.netty.channel.ChannelFuture;
import io.netty.channel.ChannelHandlerContext;
import io.netty.channel.ChannelInboundHandlerAdapter;
import io.netty.channel.ChannelInitializer;
import io.netty.channel.ChannelOutboundHandlerAdapter;
import io.netty.channel.ChannelPromise;
import io.netty.channel.EventLoopGroup;
import io.netty.channel.nio.NioEventLoopGroup;
import io.netty.channel.socket.SocketChannel;
import io.netty.channel.socket.nio.NioServerSocketChannel;
import io.netty.util.CharsetUtil;
import io.netty.util.ReferenceCountUtil;

public class NettyExample {
  private static final byte VERTICAL_BAR_CODE = 124;

  class InboundDecoder extends ChannelInboundHandlerAdapter {
    JsonParser jsonParser = new JsonParser();

    CompositeByteBuf incomplete_buf = null;
    
    private void addPartialData(ChannelHandlerContext ctx, ByteBuf buf) {
      if (buf.readableBytes() == 0)
        return;
      if (incomplete_buf == null) {
        incomplete_buf = ctx.alloc().compositeBuffer();
      }
      incomplete_buf.addComponent(buf);
      incomplete_buf.writerIndex(incomplete_buf.writerIndex() + buf.readableBytes());
      buf.retain();
    }
    
    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) { // (2)
      // Discard the received data silently.
      try {
        ByteBuf in_buf = (ByteBuf) msg;
        System.out.println("received: " + ByteBufUtil.hexDump(in_buf));
        while (in_buf.isReadable()) {
          // find '|'
          int sep_index = in_buf.bytesBefore(VERTICAL_BAR_CODE);
          
          // no '|' found
          if (sep_index == -1) {
            addPartialData(ctx, in_buf.readSlice(in_buf.readableBytes()));
            continue;
          }
          
          // '|' found
          if (sep_index != 0) {
            // '|' is found
            addPartialData(ctx, in_buf.readSlice(sep_index));
          }
          
          if (incomplete_buf != null && incomplete_buf.isReadable()) {
            // extract data
            String json_str = incomplete_buf.toString(CharsetUtil.UTF_8);
            incomplete_buf.skipBytes(incomplete_buf.readableBytes());
            
            try {
              JsonElement je = jsonParser.parse(json_str);
              ctx.fireChannelRead(je);
            } catch(JsonSyntaxException e) {
              System.out.println("invalid json string: " + json_str);
            }
            incomplete_buf.discardReadComponents();
          }
          
          // skip '|'
          assert in_buf.getByte(0) == VERTICAL_BAR_CODE;
          in_buf.skipBytes(1);
        }
      } finally {
        ReferenceCountUtil.release(msg); // (3)
      }
    }
  }
  
  class JsonProcessor extends ChannelInboundHandlerAdapter {
    @Override
    public void channelRead(ChannelHandlerContext ctx, Object msg) {
      JsonElement je = (JsonElement) msg;
      System.out.println(je);
      // Test Output
      ctx.write(je);
    }
    
    @Override
    public void exceptionCaught(ChannelHandlerContext ctx, Throwable cause) {
      // Close the connection when an exception is raised.
      cause.printStackTrace();
      ctx.close();
    }
    
    @Override
    public void channelInactive(ChannelHandlerContext ctx) {
      System.out.println("connection broken");
    }
  }
  
  class OutboundEncoder extends ChannelOutboundHandlerAdapter {
    private Gson gson = new Gson();
    
    @Override
    public void write(ChannelHandlerContext ctx, Object msg,
                      ChannelPromise promise) throws Exception {
      String s = gson.toJson((JsonElement)msg);
      ByteBuf buf = ByteBufUtil.encodeString(ctx.alloc(),
                                             CharBuffer.wrap(s),
                                             CharsetUtil.UTF_8);
      buf.writeByte(VERTICAL_BAR_CODE);
      ctx.writeAndFlush(buf);
    }
   }

  void run() throws InterruptedException {
    EventLoopGroup bossGroup = new NioEventLoopGroup(); // (1)
    EventLoopGroup workerGroup = new NioEventLoopGroup();
    try {
      ServerBootstrap b = new ServerBootstrap(); // (2)
      b.group(bossGroup, workerGroup)
          .channel(NioServerSocketChannel.class) // (3)
          .childHandler(new ChannelInitializer<SocketChannel>() { // (4)
              @Override
              public void initChannel(SocketChannel ch) throws Exception {
                ch.pipeline().addLast(new InboundDecoder());
                ch.pipeline().addLast(new OutboundEncoder());
                ch.pipeline().addLast(new JsonProcessor());
              }
          });
      //         .option(ChannelOption.SO_BACKLOG, 128)          // (5)
      //         .childOption(ChannelOption.SO_KEEPALIVE, true); // (6)

      // Bind and start to accept incoming connections.
      ChannelFuture f = b.bind(3000).sync(); // (7)

      // Wait until the server socket is closed.
      // In this example, this does not happen, but you can do that to gracefully
      // shut down your server.
      f.channel().closeFuture().sync();
    } finally {
      workerGroup.shutdownGracefully();
      bossGroup.shutdownGracefully();
    }	  
  }


  public static void main(String args[]) throws InterruptedException {
    NettyExample example = new NettyExample();
    example.run();
  }
}
