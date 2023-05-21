#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

// LED pin definitions
#define SCLK_PIN PD5
#define RCLK_PIN PD4
#define DIO_PIN PD3

#define RELAY_PIN PB0
#define FLOW_SWITCH_PIN PB2
#define SENSOR_PIN PC0

const float V_MIN = 0.5;
const float V_MAX = 4.5;
const float P_MAX = 1.2;
// const float P_MIN = 0;

const float HIGH_PRESSURE = 0.27;  // The pressure of turning off the pump.
const float LOW_PRESSURE = 0.17;   // The pressure of turning on the pump.



// LED digit table
const unsigned char LED_0F[] = {
        // 0     1     2     3     4     5     6     7     8     9    empty  .  -
        0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF, 0x7F, 0xBF
};

// LED display buffer
unsigned char LED[4];

void setup() {
    // Set RELAY_PIN as output
    DDRB |= (1 << RELAY_PIN);
    // Set LED pins as output
    DDRD |= (1 << SCLK_PIN) | (1 << RCLK_PIN) | (1 << DIO_PIN);
    // Set FLOW_SWITCH_PIN as input with pull-up resistor enabled
    DDRB &= ~(1 << FLOW_SWITCH_PIN);
    PORTB |= (1 << FLOW_SWITCH_PIN);
    // Set SENSOR_PIN as input with pull-up resistor enabled
    // Set ADC port as input
    DDRC &= ~(1 << PC0);
    // Set ADC register
    ADMUX |= (1 << REFS0); // Use AVcc as reference
    ADCSRA |= (1 << ADEN); // Enable ADC
}

double analogRead(uint8_t pin) {
    ADMUX = (ADMUX & 0xF8) | (pin & 0x07); // Set ADC channel

    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete

    uint8_t low = ADCL; // Read ADC low bytes
    uint8_t high = ADCH; // Read ADC high bytes

    return (high << 8) | low; // Return 10-bit value
}

void digitalWrite(uint8_t pin, uint8_t value) {
    if (value) {
        PORTB |= (1 << pin);
    } else {
        PORTB &= ~(1 << pin);
    }
}

int digitalRead(uint8_t pin) {
    return (PINB & (1 << pin)) ? 1 : 0;
}

void delay(unsigned long ms) {
    while (ms--) {
        _delay_ms(1);
    }
}

// LED output
void LED_OUT(unsigned char X) {
    unsigned char i;
    for (i = 8; i >= 1; i--) {
        if (X & 0x80) {
            PORTD |= (1 << DIO_PIN);
        } else {
            PORTD &= ~(1 << DIO_PIN);
        }
        X <<= 1;

        PORTD &= ~(1 << SCLK_PIN);
        PORTD |= (1 << SCLK_PIN);
    }
}

// LED 4 digits output
void LED4_Display(void) {
    unsigned char i;

    // 1st digit
    i = LED[0];
    LED_OUT(i);
    LED_OUT(0x01);
    PORTD &= ~(1 << RCLK_PIN);
    PORTD |= (1 << RCLK_PIN);

    // 2nd digit
    i = LED[1];
    LED_OUT(i);
    LED_OUT(0x02);
    PORTD &= ~(1 << RCLK_PIN);
    PORTD |= (1 << RCLK_PIN);

    // 3rd digit
    i = LED[2];
    LED_OUT(i);
    LED_OUT(0x04);
    PORTD &= ~(1 << RCLK_PIN);
    PORTD |= (1 << RCLK_PIN);

    // 4th digit
    i = LED[3];
    LED_OUT(i);
    LED_OUT(0x08);
    PORTD &= ~(1 << RCLK_PIN);
    PORTD |= (1 << RCLK_PIN);
}

unsigned char get_LED_segment(char c) {
    switch (c) {
        case '0':
            return LED_0F[0];
        case '1':
            return LED_0F[1];
        case '2':
            return LED_0F[2];
        case '3':
            return LED_0F[3];
        case '4':
            return LED_0F[4];
        case '5':
            return LED_0F[5];
        case '6':
            return LED_0F[6];
        case '7':
            return LED_0F[7];
        case '8':
            return LED_0F[8];
        case '9':
            return LED_0F[9];
        case '.':
            return LED_0F[11];
        case '-':
            return LED_0F[12];
        default:
            return LED_0F[10]; // Default to empty
    }
}

// Reverse a string
void reverse_str(char str[]) {
    int n = strlen(str);
    int i;
    char temp;
    for (i = 0; i < (n / 2); i++) {
        temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

// Display a string
void display_string(char *str) {
    reverse_str(str);
    unsigned char i;
    unsigned char j;
    for (i = 0, j = 0; j < 4; i++) {
        if (str[i] == '\0')
            LED[j] = LED_0F[10];
        if (str[i] == '.') {
            LED[j] = get_LED_segment(str[i + 1]) & LED_0F[11];
            i++;
        } else {
            LED[j] = get_LED_segment(str[i]);
        }
        j++;
    }
}


int main() {
    setup();

    double v_out = 0;     // Variable to store the voltage output of the sensor
    double pressure = 0;  // Variable to store the calculated pressure value
    int flow_switch = 0;  // Variable to store the state of the flow switch
    char buf[] = "0.000";   // Buffer to store the pressure value as a string

    while (1) {

        v_out = analogRead(SENSOR_PIN) / 1023.0 * 5.0;

        // Calculate pressure from sensor output voltage
        pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * P_MAX;

        // If pressure has a minimum value, use the following formula instead:
        // pressure = ((v_out - V_MIN) / (V_MAX - V_MIN)) * (P_MAX - P_MIN) + P_MIN;

        flow_switch = digitalRead(FLOW_SWITCH_PIN);

        // Turn on the pump when the water is flowing and the pressure is below the LOW_PRESSURE.
        if (pressure < LOW_PRESSURE && flow_switch == 1) {
            digitalWrite(RELAY_PIN, 1);
            delay(1000);
        }

        // Turn off the pump when the water stops flowing or reaches the HIGH_PRESSURE.
        if (pressure > 0.27 || flow_switch == 0) {
            digitalWrite(RELAY_PIN, 0);
            delay(10);
        }

        // Convert pressure value to string with 2 decimal and 2 digits
        dtostrf(pressure, 5, 2, buf);

        display_string(buf);
        for (int i = 0; i < 10000; i++) {
            LED4_Display();
        }
        delay(50);
    }

    return 0;
}
