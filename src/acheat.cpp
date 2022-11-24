/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/dctop/Documents/git/acheat/src/acheat.ino"
/*
 * Project acheat
 * Description: Controls aircraft heat 
 * Author: Christopher Kovacs 
 * Date: 11/24/2022
 */

// This #include statement was automatically added by the Particle IDE.
#include <thermistor-library.h>
// -----------------------------------
// Controlling LEDs over the Internet
// -----------------------------------

// First, let's create our "shorthand" for the pins
// Same as in the Blink an LED example:
// led1 is D0, led2 is D7

void setup();
void loop();
void turn_heat_off();
int readTemp(String args);
int is_heat_on(String args);
int ledToggle(String command);
#line 18 "c:/Users/dctop/Documents/git/acheat/src/acheat.ino"
int heat = D6;
int eled = D6;

int thermPin = A0;

int thermRes = 1000;

bool heatOn = false;

int maxElapsedSeconds = 3 * 60 * 60 * 1000; // 3 hours

Thermistor Thermistor(thermPin, thermRes);
Timer timer(maxElapsedSeconds, turn_heat_off, true);

// Last time, we only needed to declare pins in the setup function.
// This time, we are also going to register our Particle function

void setup()
{

  Serial.begin(9600);

  Thermistor.begin();

  // Here's the pin configuration, same as last time
  pinMode(heat, OUTPUT);
  pinMode(eled, OUTPUT);

  // We are also going to declare a Particle.function so that we can turn the LED on and off from the cloud.
  Particle.function("heat", ledToggle);
  Particle.function("temp", readTemp);
  Particle.function("status", is_heat_on);
  // This is saying that when we ask the cloud for the function "led", it will employ the function ledToggle() from this app.

  // For good measure, let's also make sure both LEDs are off when we start:
  digitalWrite(eled, LOW);
}

// Last time, we wanted to continously blink the LED on and off
// Since we're waiting for input through the cloud this time,
// we don't actually need to put anything in the loop

void loop()
{
}

void turn_heat_off()
{
  Log.info("Callback::Turning heat off\n");
  ledToggle("off");
}

int readTemp(String args) {
  Serial.printlnf("Temp C: %f", Thermistor.getTempC());
  Serial.printlnf("Temp F: %f", Thermistor.getTempF());
  Serial.printlnf("Temp K: %f", Thermistor.getTempK());
  return Thermistor.getTempC();
}

int is_heat_on(String args) {
  return heatOn;
}

int ledToggle(String command)
{
  /* Particle.functions always take a string as an argument and return an integer.
    Since we can pass a string, it means that we can give the program commands on how the function should be used.
    In this case, telling the function "on" will turn the LED on and telling it "off" will turn the LED off.
    Then, the function returns a value to us to let us know what happened.
    In this case, it will return 1 for the LEDs turning on, 0 for the LEDs turning off,
    and -1 if we received a totally bogus command that didn't do anything to the LEDs.
    */

  if (command == "on")
  {
    Serial.write("Turning heat on\n");
    heatOn = true;
    digitalWrite(eled, HIGH);
    timer.start();
    return 1;
  }
  else if (command == "off")
  {
    Serial.write("Turning heat off\n");
    heatOn = false;
    digitalWrite(eled, LOW);
    heatOn = false;
    timer.stop();
    return 0;
  }
  else
  {
    return -1;
  }
}
