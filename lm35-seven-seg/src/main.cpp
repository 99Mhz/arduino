#include <Arduino.h>
#include <MsTimer2.h>
#include <math.h>

float temp = 0.0;
uint8_t output = 1;
uint8_t segmentNumbers[] = {1, 2, 3, 4};    //
uint8_t digitMap[] =
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
uint8_t decimalCharacter = 0x80;            // segment h = 1000:0000
volatile uint8_t activeSegment = 1;                  // 0 - 3
volatile bool switchSegment = false;        // used to signal a digit change
volatile bool readADC = false;              // should we get a new reading
uint16_t interruptWaits = 250;
uint8_t staticTestDigit = 0;

void nextSegment()
{
    activeSegment <<= 1;
    if (activeSegment > 0b00001000)
        activeSegment = 0b00000001;
    switchSegment = true; //flag for main loop to change segments. We want this ISR AFAP
}

void setup() {
    Serial.begin(9600);
    MsTimer2::set(4, nextSegment); // 500ms period
    MsTimer2::start();

    for (int i=0; i<14; i++)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

/*
    maskHighDigit
    recursive function to save off the individule digits in a integer
 */
void maskHighDigit(int number, int multiplier, int placement)
{
    uint8_t mostSignificantDigit = number/multiplier;
    segmentNumbers[placement] = mostSignificantDigit;

    uint8_t remainder = number % multiplier;

    if (multiplier > 1)
        maskHighDigit(remainder, multiplier/10, ++placement);
}

/*
    setPorts
    outputs portD and portB
 */
void setPorts(uint8_t digit, uint8_t segment)
{
    int index = log(segment)/log(2);
    digit = digitMap[segmentNumbers[index]];
    //Serial.println(segment);
    PORTD = digit << 2;
    PORTB = 0;
    PORTB = ((digit >> 6) | (segment << 2));
    PORTB ^= (1 << 2);
    PORTB ^= (1 << 3);
    PORTB ^= (1 << 4);
    PORTB ^= (1 << 5);
}

void loop() {

    if (switchSegment)
    {
        if (--interruptWaits == 0)
        {
            temp = (analogRead(A1) * 5.0 / 1024.0);
                                // Read analog pin
                                // mV on A1 is a ratio of the full resolution of the ADC @ 5V
            temp = temp / 0.01; // we are left with .01 V/deg C
            //Serial.println(temp);
            interruptWaits = 250;

            if(++staticTestDigit > 9)
                staticTestDigit = 0;

            maskHighDigit((int)(temp*100), 1000, 0);

            char msg[1024];
            sprintf(msg, "%d, %d, %d, %d\n", segmentNumbers[0], segmentNumbers[1], segmentNumbers[2], segmentNumbers[3]);

            Serial.print(msg);
        }
        setPorts(digitMap[staticTestDigit], activeSegment);
        switchSegment = false;
    }
}
