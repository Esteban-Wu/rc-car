/*
 * Esteban Wu, 3/25/21
 * 
 * NRF24L01 Controller for Arduino - Transmitter
 * Interprets and transmits joystick inputs to 
 * control a small car. v2, meant to make use
 * of the joystick's wide range of motion.
 * 
 * Utilizes the RF24 library for the RF24 class.
 * 
 * Sources: 
 * - Dejan Nedelkovski, www.HowToMechatronics.com
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <math.h>

RF24 radio(7, 8);
// set transmission channel
const byte address[6] = "00001";

const int joyPinX = A0;
const int joyPinY = A1;
const int sensitivity = 200;

void setup() {
  // set up serial
  Serial.begin(9600);
  while (!Serial);
  // radio setup
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); // denotes transmitter
  // for clarity
  pinMode(joyPinX, INPUT);
  pinMode(joyPinY, INPUT);
  delay(1000);
}

void loop() {
  // map analog input to new range (0 to 1023 --> -511 to 512) 
  int intX = analogRead(joyPinX);
  int intY = analogRead(joyPinY);
  // Note: please change range to calibrate for your joystick
  intX = map(intX, 0, 1023, -508, 515); // range is off due to calibration
  intY = map(intY, 0, 1023, -511, 512);
  double dist = sqrt(pow(intX, 2) + pow(intY, 2)); // did joystick move?
  double angle = atan2(intX, intY);
  if (dist < sensitivity) {
    return;
  }
  radio.write(&angle, sizeof(angle));
  delay(5); // create some time for transmissinos to go through
}
