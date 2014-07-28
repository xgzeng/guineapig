import java.nio.charset.Charset;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;


public class AsyncClientSender {
	
    MqttAsyncClient client;
    
    AsyncClientSender(String server_url) throws MqttException {
    	client = new MqttAsyncClient(server_url, MqttAsyncClient.generateClientId());
		
		// 设置回掉函数
		client.setCallback(new MqttCallback() {
			@Override
			public void connectionLost(Throwable arg0) {
				// 处理连接中断
				System.out.println("连接中断");
				// TODO: 等待一段时间后重连
				// 处理连接中断
				try {
					client.connect().waitForCompletion();
				} catch (MqttException e) {
					// 处理连接失败
					e.printStackTrace(); 
				}
			}

			@Override
			public void deliveryComplete(IMqttDeliveryToken arg0) {
				// 处理发送完成, do nothing
			}

			@Override
			public void messageArrived(String arg0, MqttMessage arg1)
					throws Exception {
				// 处理收到消息
			}
			
		});
		
		client.connect().waitForCompletion(); // 连接服务器, 并等待完成
    }

	void run() throws MqttException, InterruptedException {
		MqttMessage msg = new MqttMessage();
		
		String s = "abc";
		byte bytes[] = s.getBytes(Charset.forName("UTF-8")); // 字符串转换为字节数组
		msg.setPayload(bytes);
		
		for (int i = 0; i < 1000; ++i) {
			try {
			    client.publish("topic_test", msg);
			} catch (MqttException e) {
				// 连接端或发送太快时会抛出异常
				e.printStackTrace();
			}
			System.out.println("发送消息");
			Thread.sleep(5000);
		}
	}
	
	public static void main(String args[]) throws MqttException, InterruptedException {
		// 发送端
		AsyncClientSender sender = new AsyncClientSender("tcp://localhost:1883");
		sender.run();
	}
}
