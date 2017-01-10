#include <ESP8266WiFi.h>

char ssid[] = "some_ssid";
char passcode[] = "somepasscode";

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  int count = WiFi.scanNetworks();

  for (int i = 0; i < count; ++i) {
    Serial.println(WiFi.SSID(i).c_str());
  }

  WiFi.mode(WIFI_AP);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    
    int status = WiFi.begin(ssid, passcode);
    Serial.print("WiFi.begin return ");
    Serial.println(status);

    delay(10000);
  }

  Serial.print("Connected To ");
  Serial.println(WiFi.SSID());
}

void loop() {
  
  delay(1000);
}

