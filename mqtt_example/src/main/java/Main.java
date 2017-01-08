import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;

public class Main {
    static String topic        = "TEST";
    static int qos             = 2;
    static String broker       = "tcp://localhost:1883";

	private static MqttClient CreateClient(String client_id) throws MqttException {
        MqttClient sampleClient = new MqttClient(broker, client_id, new MemoryPersistence());
        sampleClient.setCallback(new MqttCallback() {
            	@Override
            	public void connectionLost(Throwable e) {
            	}
            	
            	@Override
            	public void deliveryComplete(IMqttDeliveryToken token) {
            	}
            	
            	@Override
            	public void messageArrived(String topic, MqttMessage msg) {
            		System.out.println("Message received:" + new String(msg.getPayload()));
            	}
        });

        MqttConnectOptions connOpts = new MqttConnectOptions();
        connOpts.setCleanSession(true);
        
        System.out.println("Connecting to broker: "+broker);
        sampleClient.connect(connOpts);

        System.out.println("Connected");
        return sampleClient;
	}
	
    public static void main(String[] args) throws InterruptedException {
        try {
            MqttClient client = CreateClient("sender");
            //
            client.subscribe(topic);
            
            String content = "Message from MqttPublishSample";
            System.out.println("Publish message: "+content);
            MqttMessage message = new MqttMessage(content.getBytes());
            // message.setQos(qos);
            client.publish(topic, message);
            
            Thread.sleep(2*1000);
            client.disconnect();
            System.out.println("Disconnected");
        } catch(MqttException me) {
            me.printStackTrace();
        }
        
        // test for retain
        try {
            MqttClient client = CreateClient("sender");
            String content = "Message from MqttPublishSample";
            System.out.println("Publish message: "+content);
            MqttMessage message = new MqttMessage(content.getBytes());
            // message.setQos(qos);
            message.setRetained(true);
            client.publish(topic, message);
            client.disconnect();
            
            // receive later
            client = CreateClient("receiver");
            client.subscribe(topic);
            
            Thread.sleep(2*1000);
            client.disconnect();
            System.out.println("Disconnected");
        } catch(MqttException me) {
            me.printStackTrace();
        }
    }
}
