package websocket;
import java.io.IOException;

import javax.websocket.server.ServerEndpoint;
import javax.websocket.*;

@ServerEndpoint(value="/echo")
public class Echo {
	private Session session;
	
	@OnOpen
	public void start(Session session) {
		this.session = session;
	}
	
	@OnMessage
	public void incoming(String message) throws IOException {
		this.session.getBasicRemote().sendText("echo " + message);
	}
}
