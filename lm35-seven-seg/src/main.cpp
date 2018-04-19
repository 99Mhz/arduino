#include <Arduino.h>

#define SEGMENT_ONE 10
#define SEGMENT_TWO 11
#define SEGMENT_THREE 12
#define SEGMENT_FOUR 13

float temp = 0.0;
int output = 1;

void setup() {
    Serial.begin(9600);
    for (int i=0; i<14; i++)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
    digitalWrite(SEGMENT_ONE, LOW);
    digitalWrite(SEGMENT_TWO, LOW);
    digitalWrite(SEGMENT_THREE, LOW);
    digitalWrite(SEGMENT_FOUR, LOW);

}

void loop() {

    temp = (analogRead(A1) * 5.0 / 1024.0);
                        // Read analog pin
                        // mV on A1 is a ratio of the full resolution of the ADC @ 5V
    //temp = temp - 0.5;  // Adjust for 500mV output voltage at 0 deg C
    temp = temp / 0.01; // we are left with .01 V/deg C
    Serial.println(temp);


    //PORTD = output << 2;
    //PORTB = 0;
    //PORTB |= (output >> 6);

    //if (output == 0x40) { PORTB |= (1 << 0); } else { PORTB &= ~(1 << 0); }
    //if (output == 0x80) { PORTB |= (1 << 1); } else { PORTB &= ~(1 << 1); }
    //Serial.println(PORTB, BIN);
    delay(1000);
    output <<= 1;
    if (output > 0x80)
        output = 1;

}
