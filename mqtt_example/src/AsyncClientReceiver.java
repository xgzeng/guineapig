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
		
		// ���ûص�����
		client.setCallback(new MqttCallback() {

			@Override
			public void connectionLost(Throwable arg0) {
				// TODO: �ȴ�һ��ʱ�������
				// ���������ж�
			}

			@Override
			public void deliveryComplete(IMqttDeliveryToken arg0) {
				// ���������, do nothing
			}

			@Override
			public void messageArrived(String arg0, MqttMessage arg1)
					throws Exception {
				// �����յ���Ϣ
				System.out.println("�յ�����" + arg0 + "����Ϣ");
			}
			
		});
    }

	void run() throws MqttException, InterruptedException {
		client.connect().waitForCompletion();
		// ������Ϣ
		client.subscribe("topic_test", 2);
		
		Thread.sleep(1000); // do nothing, �յ�����Ϣ�ڻص������д���
	}
	
	public static void main(String args[]) throws MqttException, InterruptedException {
		// ���ն�
		AsyncClientReceiver receiver = new AsyncClientReceiver("tcp://localhost:1883");
		receiver.run();
	}
}
