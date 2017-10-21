 /*
 * Purpose:     Sketch will run the auto PEZ dispensor. 
 *              The dispensor has a 3D printed base, a PEZ toy slides into the base. 
 *              A servo opens the head when motion is detected on the 3 wire ping HC-SR04
 * 
 * Written:     John Wiltse
 * Credits:     I started with a sample sketch for the HC-SR04 from David A. Mellis Adapted for HC-SR04 by Tautvidas Sipavicius 
 * Date:        10/01/2016
 * Updated:     10/21/2017 Cleaned up the code a bit, and documented better
 * Controllers: trinket pro 5v https://www.adafruit.com/product/2000  
 *              could have used a mini, but didn't have one at the time
 * Programmed:  Arduino IDE 1.8.1 
 * Mechanical:  Power in used a Micro USB breakout https://www.adafruit.com/product/1833
 *              Servo https://www.ebay.com/itm/10pcs-x-9G-SG90-Mini-Micro-Servo-For-RC-Robot-Helicopter-Airplane-Car-Boat/122641411156
 *              HC-SR04 VCC     -> trinket pin 5v
 *              HC-SR04 Trig    -> trinket pin 8
 *              HC-SR04 Echo    -> trinket pin 4
 *              HC-SR04 GND     -> trinket pin GND
 *              serov GND       -> trinket pin GND
 *              serov Signal    -> trinket pin 9
 *              serov VCC       -> USB 5V
 *              USB GND         -> trinket pin GND
  */
#include <Servo.h>

Servo myservo;  

const int trigPin = 8;
const int echoPin = 4;
int analogVal = 0;    
int tempVal = 0;
long duration, inches, cm;
bool pezUp = false;

/* do this only once on startup */
void setup() {
    myservo.attach(9);
    Serial.begin(9600);  
    myservo.write(0);
}

/* do this forever after startup */
void loop()
{
    /* open/close the pez unit */
    while(Serial.available() > 0) 
    {
        tempVal = Serial.parseInt();
        if(tempVal > 0) 
        {
            analogVal = tempVal;
            myservo.write(120);
            delay(250); /* this delay could be a little shorter. Makes it seem slow to respond, but not enough and it might false trigger */
            myservo.write(100); /* tweek to your mechanical setup */
        }
        else 
        {
            myservo.write(0);
        }
    }

    duration = readPing();
  
    /* convert the time into a distance */
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);
  
    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    if(inches <= 5 && !pezUp) 
    {
        myservo.write(120);
        delay(250);
        myservo.write(100); 
        pezUp = true;  
    }
  
    if(inches > 5 && pezUp) 
    {
        myservo.write(0);
        pezUp = false;
    }
    delay(700);
}

/* 
 *  Reads and returns the duration of the HC SR04 sensor
 */
long readPing()
{
    /* The sensor is triggered by a HIGH pulse of 10 or more microseconds.
     * Give a short LOW pulse beforehand to ensure a clean HIGH pulse: 
     */
    pinMode(trigPin, OUTPUT);
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    /* Read the signal from the sensor: a HIGH pulse whose
     * duration is the time (in microseconds) from the sending
     * of the ping to the reception of its echo off of an object. 
     */
    pinMode(echoPin, INPUT);
    return pulseIn(echoPin, HIGH);
}

/*
 * According to Parallax's datasheet for the PING))), there are
 * 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
 * second).  This gives the distance travelled by the ping, outbound
 * and return, so we divide by 2 to get the distance of the obstacle.
 * See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
 */
long microsecondsToInches(long microseconds)
{
    return microseconds / 74 / 2;
}

/*
 * The speed of sound is 340 m/s or 29 microseconds per centimeter.
 * The ping travels out and back, so to find the distance of the
 * object we take half of the distance travelled.
 */
long microsecondsToCentimeters(long microseconds)
{
    return microseconds / 29 / 2;
}
