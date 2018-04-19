#include <Arduino.h>
#include <MsTimer2.h>

#define TEMPERATURE_READ_WAITS 250

float temp = 0.0;
uint8_t output = 1;
volatile char segmentNumbers[5] = {2, 5, 0, 0, 0};    //
uint8_t digitMap[] =
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
uint8_t decimalBit = 7;                     // segment h = 1000:0000
volatile uint8_t activeSegment = 1;         // 0 - 3
volatile bool switchSegment = false;        // used to signal a digit change
volatile bool readADC = false;              // should we get a new reading
uint16_t interruptWaits = TEMPERATURE_READ_WAITS;
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
void setDisplayBuffer(int number, int multiplier, int placement)
{
    segmentNumbers[placement] = number/multiplier;
    int remainingNumber = number % multiplier;

    if (multiplier > 1)
        setDisplayBuffer(remainingNumber, multiplier/10, ++placement);
}

/*
    setPorts
    outputs portD and portB
 */
void setPorts(uint8_t digit, uint8_t segment)
{
    int index = log(segment)/log(2);
    // ^^^ same same
    //if (segment == 8) index = 3;
    //if (segment == 4) index = 2;
    //if (segment == 2) index = 1;
    //if (segment == 1) index = 0;

    digit = digitMap[segmentNumbers[index]];
    PORTD = digit << 2;
    PORTB = 0;
    if (index == 1)
        digit |= (1 << decimalBit); //hard coded to second digit

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

            interruptWaits = TEMPERATURE_READ_WAITS; //reset our counter to delay again
            Serial.println((int)(temp*100));
            for(int i=0; i<5; i++)
                segmentNumbers[i] = 0;
            setDisplayBuffer((int)(temp*100), 1000, 0);
        }
        setPorts(digitMap[staticTestDigit], activeSegment);
        switchSegment = false;
    }
}
