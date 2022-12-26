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
#include <CE_BME280.h>
#include "google-maps-device-locator.h"

#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

void setup();
void display_text(char* text);
void loop();
void display_loop();
void turn_heat_off();
float tempC();
float tempF();
float pressureH();
float pressureM();
float altitudeM();
float altitudeFt();
float humidity();
float dewPoint(float tempC, float humidity);
int toggle_heat(String command);
void startTimer();
void stopTimer();
char* timerRemaining();
#line 15 "c:/Users/dctop/Documents/git/acheat/src/acheat.ino"
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_SSD1306 display(0);

int button = D6;
int heat = D5;
bool heatOn = false;

int maxElapsedSeconds = 3 * 60 * 60 * 1000; // 3 hours
char timerRemainingStr[50];

Timer timer(maxElapsedSeconds, turn_heat_off, true);
int timer_end = 0;
CE_BME280 bme;
GoogleMapsDeviceLocator locator;
SerialLogHandler logHandler;

void setup()
{

  Particle.variable("heatOn", heatOn);
  Particle.variable("tempC", tempC);
  Particle.variable("tempF", tempF);
  Particle.variable("pressureH", pressureH);
  Particle.variable("pressureM", pressureM);
  Particle.variable("altitudeM", altitudeM);
  Particle.variable("altitudeFt", altitudeFt);
  Particle.variable("humidity", humidity);
  Particle.variable("timerRemaining", timerRemaining);
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done

  display.clearDisplay();
  display.display();
  int writeError = display.getWriteError();
  if(display.getWriteError()) {
    Log.error("Display write error: %i", writeError);
  }

  Log.info("BME280 test");

  if (!bme.begin()) {
    Log.error("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  } else {
    Log.info("BME280 sensor initialized.");
  }

  locator.withLocateOnce();

  pinMode(button, INPUT_PULLUP);

  pinMode(heat, OUTPUT);
  Particle.function("toggle_heat", toggle_heat);

  digitalWrite(heat, LOW);

  new Thread("display_loop", display_loop);
}

void display_text(char* text) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(text);
  display.display();
  delay(3000);
}

void loop()
{
  locator.loop();
  static int lastButtonState = HIGH;
  int currentButtonState = digitalRead(button);
  if(currentButtonState != lastButtonState) {
    Log.info("Button state changed from %d to %d", lastButtonState, currentButtonState);
    lastButtonState = currentButtonState;
    if(currentButtonState == HIGH) {
      toggle_heat("");
    }
  }
}

void display_loop() {
  do
  {
    display.setTextColor(WHITE);
    display.setTextSize(2);
  
    float humidity_value = humidity();
    float tempC_value = tempC(); 
  
    char buffer[50];
    sprintf(buffer, "Temp/DwPt\n%.0f\370C/%.0f\370C", tempC_value, dewPoint(tempC_value, humidity_value));
    display_text(buffer);
    sprintf(buffer, "Altimeter\n%.2f\" Hg", pressureM());
    display_text(buffer);
    sprintf(buffer, "Humidity\n%.0f%%", humidity_value);
    display_text(buffer);
    if(heatOn) {
      sprintf(buffer, "Heat Time\n%s", timerRemaining());
      display_text(buffer);
    }
  } while (true);
}

void turn_heat_off()
{
  Log.info("Callback::Turning heat off");

  toggle_heat("off");
}

float tempC() {
  return bme.readTemperature();
}

float tempF() {
  return bme.readTemperature() * 1.8 + 32.0;
}

float pressureH() {
  return (bme.readPressure() / 100.0);
}

float pressureM() {
  return pressureH() * 0.029529983071445;
}

float altitudeM() {
  return bme.readAltitude(SEALEVELPRESSURE_HPA);
}

float altitudeFt() {
  return altitudeM() * 3.28084;
}

float humidity() {
  return bme.readHumidity();
}

float dewPoint(float tempC, float humidity) {
  return tempC - (100 - humidity)/5;
}

int toggle_heat(String command)
{
  if(command == NULL || command == "") {
    heatOn = !heatOn;
  } else {
    heatOn = command == "on";
  }
    
  if (heatOn)
  {
    Log.info("Turning heat on");
    digitalWrite(heat, HIGH);
    startTimer();
    return 1;
  }
  else
  {
    Log.info("Turning heat off");
    digitalWrite(heat, LOW);
    stopTimer();
    return 0;
  }
}

void startTimer() {
  timer_end = Time.now() + maxElapsedSeconds / 1000;
  timer.start();
}

void stopTimer() {
  timer.stop();
}

char* timerRemaining() {
  int hours, minutes, seconds;
  seconds = timer_end - Time.now();
  minutes = seconds / 60;
  hours = minutes / 60;
  sprintf(timerRemainingStr, "%i:%2i:%2i", hours, minutes%60, seconds%60);
  return timerRemainingStr;
}
