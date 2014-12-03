import java.nio.charset.Charset;

import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttAsyncClient;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;


public class AsyncClientReceiver {
	
    MqttAsyncClient client;
    
    AsyncClientReceiver(String server_url) throws MqttException {
    	client = new MqttAsyncClient(server_url, MqttAsyncClient.generateClientId());
		
		// 设置回掉函数
		client.setCallback(new MqttCallback() {

			@Override
			public void connectionLost(Throwable arg0) {
				// TODO: 等待一段时间后重连
				// 处理连接中断
			}

			@Override
			public void deliveryComplete(IMqttDeliveryToken arg0) {
				// 处理发送完成, do nothing
			}

			@Override
			public void messageArrived(String arg0, MqttMessage arg1)
					throws Exception {
				// 处理收到消息
				System.out.println("收到主题" + arg0 + "的消息");
			}
			
		});
    }

	void run() throws MqttException, InterruptedException {
		client.connect().waitForCompletion();
		// 订阅消息
		client.subscribe("topic_test", 2);
		
		Thread.sleep(1000); // do nothing, 收到的消息在回调函数中处理
	}
	
	public static void main(String args[]) throws MqttException, InterruptedException {
		// 接收端
		AsyncClientReceiver receiver = new AsyncClientReceiver("tcp://localhost:1883");
		receiver.run();
	}
}
