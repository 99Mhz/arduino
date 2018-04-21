/* main.cpp
 * Entry point for 4 digit seven segment display example for ACMW Bellevue
 * friday talk.
 */

/* Helpful libraries
 */
#include <Arduino.h>
#include <MsTimer2.h>

/* Application constants
 */
#define TEMPERATURE_READ_WAITS 250
#define TIMER_TWO_MS_TIMEOUT 4
#define SERIAL_SPEED 115200
#define DECIMAL_BIT 7

/* Variables Needed
 */
volatile bool switchSegment = false;    // used to signal a digit change
uint8_t segmentNumbers[5] =
    {2, 5, 0, 0, 0};                    //buffer for current numbers to display
uint8_t digitMap[] =
    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67};
uint8_t segmentIndex = 0;
uint8_t currentDigit = 0;
volatile uint8_t activeSegment = 1;     // 1 - 4
uint16_t interruptWaits = TEMPERATURE_READ_WAITS;
float temperature = 0.0;

/* function prototypes
 */
void setup();
void nextSegment();
void setDisplayBuffer(int number, int multiplier, int placement);
void setPorts(uint8_t segment);

/* loop
 * Main loop of the application. runs forever
 * Checks if the segment needs to be changed, and if we need to get another
 * tempeture reading. We only do this every so often to not spend too much
 * time reading the ADC.
 */
void loop() {

    if (switchSegment)
    {
        if (--interruptWaits == 0)
        {
            temperature = (analogRead(A1) * 5.0 / 1024.0);
                                // Read analog pin
                                // mV on A1 is a ratio of the full resolution of the ADC @ 5V
            temperature = temperature / 0.01; // we are left with .01 V/deg C

            interruptWaits = TEMPERATURE_READ_WAITS; //reset our counter to delay again
            //Serial.println((int)(temperature*100));
            setDisplayBuffer((int)(temperature*100), 1000, 0);
        }
        setPorts(activeSegment);
        switchSegment = false;
    }
}

/* setup
 * Set up our application.
 * Define the serial connection for debugging
 * Set up timer 2 for segment change
 * set all digital ports to outputs
 */
void setup() {
    Serial.begin(SERIAL_SPEED);
    MsTimer2::set(TIMER_TWO_MS_TIMEOUT, nextSegment); // 500ms period
    MsTimer2::start();

    for (int i=0; i<14; i++)
    {
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }
}

/* nextSegment ISR
 * bit shit segment to the left, or wrap right. signal to switch segments
 */
void nextSegment()
{
    activeSegment <<= 1;
    if (activeSegment > 0b00001000)
        activeSegment = 0b00000001;
    switchSegment = true; //flag for main loop to change segments
}

/* setDisplayBuffer
 * Recursive function to save off the individule digits in a integer
 * Example 21.45 would leave segmentNumbers as [2, 1, 4, 5]
 * Similar to sprintf without leaving the array values in ascii
 */
void setDisplayBuffer(int number, int multiplier, int placement)
{
    segmentNumbers[placement] = number/multiplier;
    uint16_t remainingNumber = number % multiplier;

    if (multiplier > 1)
        setDisplayBuffer(remainingNumber, multiplier/10, ++placement);
}

/* setPorts
 * sets the individual pins on portB and portD with the value of digit
 * and latches (sync low) the segment
 * Example if digit is 0x5B (2) and segment is 3 then port b and d are set:
 * -Digit Latches- ---------Segment Latches--------- -unused
 * PB5 PB4 PB3 PB2 PB1 PB0 : PD7 PD6 PD5 PD4 PD3 PD2 PD1 PD0
 *  1   1   0   1   0   1     0   1   1   0   1   1   x   x
 */
void setPorts(uint8_t segment)
{
    segmentIndex = log(segment)/log(2);
    // ^^^ same same
    //if (segment == 8) index = 3;
    //if (segment == 4) index = 2;
    //if (segment == 2) index = 1;
    //if (segment == 1) index = 0;

    currentDigit = digitMap[segmentNumbers[segmentIndex]]; //map ex. 2 to 0x5B
    PORTD = currentDigit << 2; //shift 2 left because PD0/1 are RDX/TXD
    PORTB = 0;

    /* TODO:
     * digit scan is left to right. (MSD --> LSD)
     * probably better to scan left to right (MSD <-- LSD) for consistancy
     * in that case decimal point will be at index 2 not 1.
     */
    if (segmentIndex == 1)
        currentDigit |= (1 << DECIMAL_BIT); //hard coded decimal point

    PORTB = ((currentDigit >> 6) | (segment << 2));

    // PORTB looks like this xx01:00xx and we want this xx10:11xx
    PORTB ^= 0x3C; // reverse bits 2:5 xx111100 = 0x3C
}
