/*
 * Purpose:     Sketch will run the scarecrow RGB led. 
 *              It simulated a flickering orange color like a candle in a pumpkin. 
 * 
 * Written:     John Wiltse
 * Date:        10/01/2015
 * Updated:     10/21/2017 Cleaned up the code a bit, and documented better
 * Controllers: Tested on an arduino micro, and a pro trinkit 3.3v 12Mhz. 
 *              Many others should work fine.  
 * Programmed:  Arduino IDE 1.8.1 
 * Mechanical:  red wire    -> pin 9
 *              green wire  -> pin 10
 *              black wire  -> gnd (pin G on the trinket) 
 */

/* define the pins on the controller to attach red and green wire. Black goes to a ground pin */
const int redPin = 9;       
const int greenPin = 10;    

/* define a brightness variables to work with */
int randomBrightness = 0;
int randomDelay = 0;


/* do this only once on startup */
void setup() 
{
    /* pins are output to drive the leds */
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
}

/* do this forever after startup */
void loop()
{
    /* get a random number and assign to randomBrightness */
    randomBrightness = random(25, 100);

    /* output random red, and not as much random green */
    analogWrite(redPin, randomBrightness);          
    analogWrite(greenPin, randomBrightness/4);

    /* get a random delay min = 50 max = 200 to simulate the flicker. 
     * Play with this a bit to change the effect. Maybe put on some potentiometers! */
    randomDelay = random(50, 200);
    delay(randomDelay);
}

