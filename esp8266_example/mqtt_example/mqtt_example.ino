// Libraries Required
// - Adafruit MQTT Library by Adafruit

#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

char ssid[] = "foo";
char passcode[] = "bar";

WiFiClient client;

Adafruit_MQTT_Client mqtt(&client, "192.168.0.3", 1883, "anonymous", "password");

Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, "feeds/onoff");

void setup() {
  Serial.begin(115200);
  // Serial.setDebugOutput(true);

  int count = WiFi.scanNetworks();

  for (int i = 0; i < count; ++i) {
    Serial.println(WiFi.SSID(i).c_str());
  }

  WiFi.mode(WIFI_AP);

  WiFi.begin(ssid, passcode);
  int status = WiFi.status();
  while (status != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.print(ssid);
    Serial.print(" status=");
    Serial.println(status);

    delay(5000);
    status = WiFi.status();
  }

  Serial.print("Connected To ");
  Serial.println(WiFi.SSID());

  mqtt.subscribe(&onoffbutton);
}

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // send mqtt message
  mqtt.publish("test_topic", "hello");
  
  delay(1000);
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

