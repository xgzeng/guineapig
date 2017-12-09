int pin2 = 2;

void setup() {
  // set GPIO0 as output mode
  pinMode(pin0, OUTPUT);
}

void loop() {
  // switch GPIO high/low with 1s interval
  digitalWrite(pin2, HIGH);
  delay(1000);
  digitalWrite(pin0, LOW);
  delay(1000);
}

