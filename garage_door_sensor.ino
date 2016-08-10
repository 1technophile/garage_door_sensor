/*
  This program enables to send state of a garage door thanks to a sonar:
 - send open signal every 10 seconds if the sonar return a value > to 15cm
 - send close signal if after sending open signal the sonar return a value < 15cm
  Contributors:
  - 1technophile
  Based on the libraries:
  - RCSwitch
  - NewPing
  
  Documentation:
  Project home: https://github.com/1technophile/garage_door_sensor
  Blog: http://1technophile.blogspot.com/2016/08/garage-door-sensor.html
Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
and associated documentation files (the "Software"), to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
subject to the following conditions:
The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED 
TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <NewPing.h>
#include <RCSwitch.h>
#include <string.h>

RCSwitch mySwitch = RCSwitch();

//Time used to wait for an interval before resending data
unsigned long time;

//Pin on which the sensors are connected
const int EmitPin = 3;
const int EmitPowerPin = 2;
const int LedPin = 13;

#define TRIGGER_PIN  8
#define ECHO_PIN     7
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

//Do we want to see trace for debugging purposes
#define TRACE 1  // 0= trace off 1 = trace on

//Value to send following the states
#define GDOOR_OPEN   "123456"
#define GDOOR_CLOSE  "654321"
boolean closeSent= false;
  
void setup()
{
  Serial.begin(9600);

  // Launch traces for debugging purposes
  trc("Start of the program");

}

void loop()
{
  int cM = sonar.ping_cm();
  Serial.print("Ping: ");
  Serial.print(cM);
  Serial.println("cm");

  if ( cM > 15 && cM!=0){ // >15 means that the garage door is open, we send an open signal by RF 433Mhz
    pinMode(EmitPowerPin,OUTPUT);
    pinMode(LedPin,OUTPUT);
    digitalWrite(EmitPowerPin, HIGH);
    digitalWrite(LedPin, HIGH);
    mySwitch.enableTransmit(EmitPin);  // 
    mySwitch.setRepeatTransmit(10); //increase transmit repeat to avoid lost of rf sendings
    sendData(atol(GDOOR_OPEN));
    delay(1000);
    digitalWrite(EmitPowerPin, LOW);
    digitalWrite(LedPin, LOW);
    closeSent = false;
  } else if ( cM < 15 && cM!=0 && closeSent == false){ // <15 the signal "closed" is sent only once to avoid spreading unecessary RF signal
    pinMode(EmitPowerPin,OUTPUT);
    pinMode(LedPin,OUTPUT);
    digitalWrite(EmitPowerPin, HIGH);
    digitalWrite(LedPin, HIGH);
    mySwitch.enableTransmit(EmitPin);  // 
    mySwitch.setRepeatTransmit(10); //increase transmit repeat to avoid lost of rf sendings
    sendData(atol(GDOOR_CLOSE));
    delay(1000);
    closeSent = true;
    digitalWrite(EmitPowerPin, LOW);
    digitalWrite(LedPin, LOW);
  }
  
  delay(10000); // wait 10 seconds between each measure
}

void sendData(long dataTosend){
  
  trc(String(dataTosend));
 
  //sending value by RF
  mySwitch.send(dataTosend,24);
  
}

//trace function
void trc(String msg){
  if (TRACE) {
  Serial.println(msg);
  }
}
