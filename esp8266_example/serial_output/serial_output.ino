
int pin0 = 0;

void setup() {
  Serial.begin(9600);

  pinMode(pin0, OUTPUT);
}

void loop() {
  Serial.println("hello");
  delay(1000);
}

