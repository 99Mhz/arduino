Assumptions:
- Temp will be in degree C
- 2 digits to right, and 2 digits to left of decimal point. Example 21.28 deg C
- Decimal point will be fixed at second digit.
- Union 8 IO lines to a logical PORT so we can just latch the port like: DISPLAY_PORT = digitValue;
- Segment display PORT ‘a’ = LSD ‘dp’ = MSD
- Allow users to adjust the timer to slow down and see the multiplexing
- Watch over driving the LEDs for when its slowed aim for ~20mA
- Digits will multiplex on timer1 start out with ~75Hz or change digits every third interrupt.
- 8 Pins for DISPLAY_PORT PD0-7 (Arduino pins 0 - 7)
- 4 Pins for digits PB0-3 (Arduino pins 8 - 11)
- Will write directly to AVR ports and not use digitalWrite to make this easy, clean and quick
- 1 analog pin for LM35 (Arduino pin A0)

Basic Workflow

#define REFRESH_DIVIDE 3

char[] digitMap = [0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67]
// zero   = segments a, b, c, d, e, f         = 0011:1111 = 0x3F
// one    = segments b + c                    = 0000:0110 = 0x06
// two    = segments a + b + d + e + g        = 0101:1011 = 0x5B
// three  = segments a + b + c + d + g        = 0100:1111 = 0x4F
// four   = segments b + c + f + g            = 0110:0110 = 0x66
// five   = segments a + c + d + f + g        = 0110:1101 = 0x6D
// six    = segments c + d + e + f + g        = 0111:1100 = 0x7C
// seven  = segments a + b + c                = 0000:0111 = 0x07
// eight  = segments a + b + c + d + e +f + g = 0111:1111 = 0x7F
// nine   = segments a + b + c + f + g        = 0110:0111 = 0x67

char segmentNumbers[] = [2, 1, 3, 4]
char decimalCharacter = 0x80 // segment h = 1000:0000
int activeSegment = 0 to 3
volatile Bool switchSegment = false;  //used to signal a digit change
volatile bool readADC = false


On MsTimer2 interrupt:
//only change volatile variables, don’t set any pins/ports, delay too long etc.
    - Change the active segment to the next one
    - switchSegment = true //flag for main loop to change segments. We want this ISR AFAP

On timer1 interrupt:
https://playground.arduino.cc/Code/Timer1
- readADC = true

SplitDigits:
- mask off first digit from input and store in segmentNumbers[3]
- mask off second digit from input and store in segmentNumbers[2]
- mask off third digit from input and store in segmentNumbers[1]
- mask off forth digit from input and store in segmentNumbers[0]

getTemp:
- read the ADC
- Calculate the Temp
- Call split up the digits with the temperature value

Loop:

- If readADC:
    - Call getTemp
    - readADC = false

- If switchSegment:
    - activeSegment == two?
        - YES:
            - DISPLAY_PORT = Logical OR the decimalCharacter with whatever number is in segmentNumbers[activeSegment]
        - NO:
            - DISPLAY_PORT = segmentNumbers[activeSegment]
    - Deactivate the last segment
    - Activate the current segment // activate then deactivate might cause ghosting
    - switchSegment = false
