/// reference: http://www.ti.com/lit/ds/symlink/lm35.pdf

float history_temp = 0.0;

void setup() {
  Serial.begin(115200);
  
  // Serial.setDebugOutput(true);
  // Serial.print("Connected To ");
  history_temp = read_temperature();
}

float read_temperature() {
  // analogRead range: 0(0V, 0°C) - 1024(1V, 150°C)
  int analog_voltage = analogRead(A0);
  return float(analog_voltage * 150) /1024;
}

void loop() {
  // smoothing
  float read_temp = read_temperature();
  history_temp = history_temp * 0.8 + read_temp * 0.2;
  Serial.print("read value:");
  Serial.print(read_temp);
  Serial.print(" smooth_value:");
  Serial.println((int)(history_temp * 10) / 10.0);
  delay(1000);
}

