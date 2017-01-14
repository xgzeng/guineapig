/// reference: http://www.ti.com/lit/ds/symlink/lm35.pdf

float history_temp = 0.0;

// ADC_MODE(ADC_TOUT);
void setup() {
  Serial.begin(115200);

  history_temp = analogReadToCelsius(analogRead(A0));
}

/// nodecmu board has two resistor wired on ADC pin, so max input voltage is not 1V.
///   3.422V is measured for my board, default should be 3.2V.
const int MAX_ANALOG_INPUT_VOLTAGE_MV = 3422;

float analogReadToVoltage(int value) {
  return analogRead(A0) * MAX_ANALOG_INPUT_VOLTAGE_MV / 1023;  
}

float analogReadToCelsius(int value) {
  return analogReadToVoltage(value)/10 + 2;
}

void loop() {
  
  int analog_read_value = analogRead(A0);
  
  float current_temp = analogReadToCelsius(analog_read_value);
  
  // smoothing
  history_temp = history_temp * 0.8 + current_temp * 0.2;
  
  Serial.print("read value:");
  Serial.print(analog_read_value);
  Serial.print(" ");
  Serial.print(current_temp);
  Serial.print(" smooth_value:");
  Serial.println((int)(history_temp * 10) / 10.0);
  delay(2000);
}

