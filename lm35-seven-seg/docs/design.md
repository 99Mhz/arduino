# Project Planning Notes and Pseudo Code
### Assumptions:
1. Temp will be in degree C
2. 2 digits to right, and 2 digits to left of decimal point. Example 21.28 deg C
3. Decimal point will be fixed at second digit.
4. Union 8 IO lines to a logical PORT so we can just latch the port like: DISPLAY_PORT = digitValue; *Not implemented*
5. Segment display PORT ‘a’ = LSD ‘dp’ = MSD
6. Allow users to adjust the timer to slow down and see the multiplexing
7. Watch over driving the LEDs for when its slowed aim for ~20mA
8. Digits will multiplex on timer1 start out with ~75Hz or change digits every third interrupt.
9. 8 Pins for DISPLAY_PORT PD0-7 (Arduino pins 0 - 7)
10. 4 Pins for digits PB0-3 (Arduino pins 8 - 11)
11. Will write directly to AVR ports and not use digitalWrite to make this easy, clean and quick
12. 1 analog pin for LM35 (Arduino pin A0)

### Calculations

We need a mapping between an integer variable literal 1 to displaying it with the seven segment display. Using the following array
```C
char array digitMap = [0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7C, 0x07, 0x7F, 0x67]
```
#### Value calculations

digit | segments lit | BIN Value | HEX value
------|--------------|-----------|----------
zero | a, b, c, d, e, f | 0011:1111 | 0x3F
one | b + c | 0000:0110 | 0x06
two | a + b + d + e + g | 0101:1011 | 0x5B
three | a + b + c + d + g | 0100:1111 | 0x4F
four | b + c + f + g | 0110:0110 | 0x66
five | a + c + d + f + g | 0110:1101 | 0x6D
six | c + d + e + f + g | 0111:1100 | 0x7C
seven | a + b + c | 0000:0111 | 0x07
eight | a + b + c + d + e +f + g | 0111:1111 | 0x7F
nine | a + b + c + f + g | 0110:0111 | 0x67

### Pseudo Code

On MsTimer2 interrupt:<br />
*only change volatile variables, don’t set any pins/ports, delay too long etc.*
- Change the active segment to the next one
- switchSegment = true //flag for main loop to change segments. We want this ISR AFAP
- readADC = true
https://playground.arduino.cc/Code/Timer1

SplitDigits Method:
- mask off first digit from input and store in segmentNumbers[3]
- mask off second digit from input and store in segmentNumbers[2]
- mask off third digit from input and store in segmentNumbers[1]
- mask off forth digit from input and store in segmentNumbers[0]

getTemp Method:
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
