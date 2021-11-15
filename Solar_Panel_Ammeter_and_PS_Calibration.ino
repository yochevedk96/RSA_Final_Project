//This code uses INA260 current sensor and photoresistors
//Developed by Max Muss
//https://www.amazon.com/gp/product/B07S8QYDF8/ref=ppx_yo_dt_b_asin_title_o01_s00?ie=UTF8&psc=1
//https://learn.adafruit.com/adafruit-ina260-current-voltage-power-sensor-breakout/arduino
//https://www.arduino.cc/en/Tutorial/BuiltInExamples/Calibration

//Include Current Sensor Library
#include <Adafruit_INA260.h>

//Initialize Sensor
Adafruit_INA260 ina260 = Adafruit_INA260();

//Photoresistor Constants
const int ps1Pin = A0;
const in ps2Pin = A1;

//Photorsisor Calibration Variables
int ps1Min = 1023;
int ps1Max = 0;
int ps2Min = 1023;
int ps2Max = 0;

//Photoresistor Measurements
int ps1 = 0;
int ps2 = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10); // wait for serial port to connect. Needed for native USB port only
  }

  if (!ina260.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");

  int start = millis(); //take start time

  while (millis() < 5000) { //first five seconds
    ps1 = analogRead(ps1Pin); //read photoresistor 1
    ps2 = analogRead(ps2Pin); //read photoresistor 2

    //PS1 Ccalibration
    if (ps1 > ps1Max) {
      ps1Max = ps1;
    }
    if (ps1 < ps1Min) {
      ps1Min = ps1;
    }

    //PS2 Calibration
    if (ps2 > ps2Max) {
      ps2Max = ps2;
    }
    if (ps2 < ps2Min) {
      ps2Min = ps2;
    }
  }

}

void loop() {
  float currentA = ina260.readCurrent() / 1000; //Read current and convert to Amps
  //Print values
  Serial.print("Current: ");
  Serial.print(currentA);
  Serial.println(" A");

  //Read Photoresistors
  ps1 = analogRead(ps1Pin);
  ps2 = analogRead(ps2Pin);

  //Constrain photoresistors in case values are outside of calibration range
  ps1 = constrain(ps1, ps1Min, ps1Max);
  ps2 = constrain(ps2, ps2Min, ps2Max);

  //Apply calibrations
  ps1 = map(ps1, ps1Min, ps1Max, 0, 255);
  ps2 = map(ps2, ps2Min, ps2Max, 0, 255);
}
