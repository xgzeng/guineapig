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
		
		// ���ûص�����
		client.setCallback(new MqttCallback() {
			@Override
			public void connectionLost(Throwable arg0) {
				// ���������ж�
				System.out.println("�����ж�");
				// TODO: �ȴ�һ��ʱ�������
				// ���������ж�
				try {
					client.connect().waitForCompletion();
				} catch (MqttException e) {
					// ��������ʧ��
					e.printStackTrace(); 
				}
			}

			@Override
			public void deliveryComplete(IMqttDeliveryToken arg0) {
				// ���������, do nothing
			}

			@Override
			public void messageArrived(String arg0, MqttMessage arg1)
					throws Exception {
				// �����յ���Ϣ
			}
			
		});
		
		client.connect().waitForCompletion(); // ���ӷ�����, ���ȴ����
    }

	void run() throws MqttException, InterruptedException {
		MqttMessage msg = new MqttMessage();
		
		String s = "abc";
		byte bytes[] = s.getBytes(Charset.forName("UTF-8")); // �ַ���ת��Ϊ�ֽ�����
		msg.setPayload(bytes);
		
		for (int i = 0; i < 1000; ++i) {
			try {
			    client.publish("topic_test", msg);
			} catch (MqttException e) {
				// ���Ӷ˻���̫��ʱ���׳��쳣
				e.printStackTrace();
			}
			System.out.println("������Ϣ");
			Thread.sleep(5000);
		}
	}
	
	public static void main(String args[]) throws MqttException, InterruptedException {
		// ���Ͷ�
		AsyncClientSender sender = new AsyncClientSender("tcp://localhost:1883");
		sender.run();
	}
}
