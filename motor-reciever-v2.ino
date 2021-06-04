/*
 * Esteban Wu, 3/25/21
 * 2-Motor L9110 - v2
 * Note that the motors are meant to be oriented in 
 * opposite directions - i. e., facing away from each
 * other, contacts facing toward or away from each 
 * other. Now supports greater variety of directions
 * from the joystick remote controller (8).
 * 
 * Uses the RF24 library for the RF24 class.
 * 
 * Sources: 
 * - Dejan Nedelkovski, www.HowToMechatronics.com
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <math.h>

RF24 radio(7, 8);
// set channel for transmission/receiving
const byte address[6] = "00001";

// motor pins
const int A1A = 2;
const int A1B = 3;
const int B1A = 5;
const int B1B = 6;

const float threeQtr = 3.00/4.00;
const float qtr = 1.00/4.00;
const float half = 1.00/2.00;

void setup() {
  // set up serial monitor
  Serial.begin(9600);
  while (!Serial);
  // radio set up
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening(); // denotes receiver
  // motors
  pinMode(A1A, OUTPUT);
  pinMode(A1B, OUTPUT);
  pinMode(B1A, OUTPUT);
  pinMode(B1B, OUTPUT);
}

void loop() {
  double angle = 9999; // set to a value that will never occur
  if (radio.available()) { // get angle from remote
    radio.read(&angle, sizeof(angle));
    angle = angle + ((1.00/8.00) * M_PI); // shift to align
  }
  Serial.println(angle);
  /* To make the direction calculations a little easier, let's use
   * the following to determine direction:
   * Picture a circle, with 12 o'clock being angle pi (also -pi),
   * 6 o'clock 0, 3 o'clock 1/2 * pi and 9 o'clock -1/2 * pi.
   * 
   * Forward = -1/4 * pi to -pi
   * Right forward = pi to 3/4 * pi
   * Right = 3/4 * pi to 1/2 * pi
   * Right back = 1/2 * pi to 1/4 * pi
   * Back  = 1/4 * pi to 0
   * Left back = 0 to -1/4 * pi
   * Left = -1/4 * pi to -1/2 * pi
   * Left forward = -1/2 * pi to -3/4 * pi
   * 
   * Note that we have to shift the input angle, as we want 12 
   * o'clock on the joystick to align with the middle of the 
   * forward wedge (-7/8 * pi).
   */
   
  if ((angle <= (-threeQtr * M_PI) && angle >= (-M_PI)) || (angle >= M_PI && angle <= (2 * M_PI))) { // forward
    motorA('F');
    motorB('F');
  } else if (angle <= (threeQtr * M_PI) && angle >= (half * M_PI)) { // right
    motorA('B');
    motorB('F');
  } else if (angle >= (-half * M_PI) && angle <= (-qtr * M_PI)) { // left
    motorA('F');
    motorB('B');
  } else if (angle <= M_PI && angle >= (threeQtr * M_PI)) { // forward right
    motorA('O');
    motorB('F');
  } else if (angle >= (-threeQtr * M_PI) && angle <= (-half * M_PI)) { // forward left
    motorA('F');
    motorB('O');
  } else if (angle >= (0 * M_PI) && angle <= (qtr * M_PI)) { // back
    motorA('B');
    motorB('B');
  } else if (angle <= (half * M_PI) && angle >= (qtr * M_PI)) { // back right
    motorA('O');
    motorB('B');
  } else if (angle >= (-qtr * M_PI) && angle <= 0) { // back left
    motorA('B');
    motorB('O');
  } else { // off
    motorA('O');
    motorB('O');
  }
  delay(40);
}


/* 
 *  Controls motor A, and changes motor state based 
 *  on the following input:
 *  'R' - turns motor "forward"
 *  'L' - turns motor "backward"
 *  'O' - turns motor off
 *  
 */
void motorA(char c) {
  if (c == 'F') {
    digitalWrite(A1A, HIGH);
    digitalWrite(A1B, LOW);
  } else if (c == 'B') {
    digitalWrite(A1A, LOW);
    digitalWrite(A1B, HIGH);
  } else {
    digitalWrite(A1A, LOW);
    digitalWrite(A1B, LOW);
  }
}

/* 
 *  Controls motor B, and changes motor state based 
 *  on the following input:
 *  'F' - turns motor "forward"
 *  'B' - turns motor "backward"
 *  'O' - turns motor off
 *  
 */
void motorB(char c) {
  if (c == 'F') {
    digitalWrite(B1A, LOW);
    digitalWrite(B1B, HIGH);
  } else if (c == 'B') {
    digitalWrite(B1A, HIGH);
    digitalWrite(B1B, LOW);
  } else {
    digitalWrite(B1A, LOW);
    digitalWrite(B1B, LOW);
  }
}
