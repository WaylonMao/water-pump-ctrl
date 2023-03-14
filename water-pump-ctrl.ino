#include <FourBitDisplay.h>
#define RELAY 8
#define FLOW_SWITCH 10
#define SENSOR A0

FourBitDisplay fbd(4, 5, 3);  // RCLK: PIN4, SCLK: PIN5, DIO: PIN3

/*
* Water Pressure Sensor
* Working voltage: 5.0V
* Output voltage(V_MIN, V_MAX): 0.4~4.5V
* Measuring range(P_MIN, P_MAX): 0~1.2MPa
*/
const float V_MIN = 0.5;
const float V_MAX = 4.5;
const float P_MAX = 1.2;
// const float P_MIN = 0;

const float HIGH_PRESSURE = 0.27;  // The pressure of turning off the pump.
const float LOW_PRESSURE = 0.17;   // The pressure of turning on the pump.

double v_out = 0;     // Variable to store the voltage output of the sensor
double pressure = 0;  // Variable to store the calculated pressure value
int flow_switch = 0;  // Variable to store the state of the flow switch

void setup() {
  pinMode(RELAY, OUTPUT);
  pinMode(FLOW_SWITCH, INPUT);
}

void loop() {
  char buf[4];  // Character buffer to store pressure value as string

  v_out = analogRead(SENSOR) / 1023. * 5.0;

  // Calculate pressure from sensor output voltage
  pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * P_MAX;

  // If pressure has a minimum value, use the following formula instead:
  // pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * (P_MAX - P_MIN) + P_MIN;

  flow_switch = digitalRead(FLOW_SWITCH);

  // Turn on the pump when the water is flowing and the pressure is below the LOW_PRESSURE.
  if (pressure < LOW_PRESSURE && flow_switch == HIGH) {
    digitalWrite(RELAY, HIGH);
    delay(1000);
  }

  // Turn off the pump when the water stops flowing or reaches the HIGH_PRESSURE.
  if (pressure > HIGH_PRESSURE || flow_switch == LOW) {
    digitalWrite(RELAY, LOW);
    delay(10);
  }

  // Convert pressure value to string with 1 decimal and 3 digits
  dtostrf(pressure, 1, 3, buf);

  for (int i = 0; i < 100; i++) {
    fbd.printToDisplay(buf, 5, "r");
  }
  delay(10);
}
