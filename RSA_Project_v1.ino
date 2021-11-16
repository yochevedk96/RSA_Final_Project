//Include Current Sensor Library
#include <Adafruit_INA260.h>
#include <SPI.h> //include this library for SPI communication
#include <Wire.h> //include this library since needed for the Adafruit library
#include <Adafruit_SSD1306.h> //include the Adarfruit library for this specific OLED screen

#define OLED_MOSI 9 //MOSI wire connected to digital pin 9
#define OLED_CLK 10 //clock wire connected to digital pin 10
#define OLED_DC 11 //__ wire connected to digital pin 11
#define OLED_CS 12 //__ wire connected to digital pin 12
#define OLED_RESET 13 //reset wire is connected ot pic 13

//define our OLED "display" to be used for displaying onto the screen
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//Initialize Sensor
Adafruit_INA260 ina260 = Adafruit_INA260();

int threshold = 5;
int direction_of_rotation; //hold value of 1 for right and -1 for left
float cellVoltage;

//Photoresistor Constants
const int ps1Pin = A0;
const int ps2Pin = A1;

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
  display.begin(SSD1306_SWITCHCAPVCC); //begin the OLED display communication
  
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
  
  display.clearDisplay(); //clear the OLED display from what was writted on it last
  display.setTextSize(1); //set the text size for the display
  display.setTextColor(WHITE); //set the text color for the display
  display.setCursor(0, 0); //set the pointer for the cursor

  float currentA = ina260.readCurrent() / 1000; //Read current and convert to Amps
  //Print values
  Serial.print("Current: ");
  Serial.print(currentA);
  Serial.println(" A");

  display.print("Current: "); 
  display.print(currentA);
  display.println(" A");

  
  //Read Photoresistors
  ps1 = analogRead(ps1Pin);
  ps2 = analogRead(ps2Pin);

  //Constrain photoresistors in case values are outside of calibration range
  ps1 = constrain(ps1, ps1Min, ps1Max);
  ps2 = constrain(ps2, ps2Min, ps2Max);

  //Apply calibrations
  ps1 = map(ps1, ps1Min, ps1Max, 0, 255);
  ps2 = map(ps2, ps2Min, ps2Max, 0, 255);

  Serial.print("Photoresistor 1:  ");
  Serial.print(ps1);
  Serial.print("   Photoresistor 2:  ");
  Serial.println(ps2);

  if (abs(ps1 - ps2) > threshold) {
    direction_of_rotation = find_direction(ps1, ps2);
    if (direction_of_rotation == 1) {
      Serial.println("Spinning Clockwise");
      display.println("Spinning Clockwise");
    } else {
      Serial.println("Spinning Counter-Clockwise");
      display.println("Spinning Counter-Clockwise");
    }
    //insert function to spin motor here @Richard
  }
  cellVoltage = analogRead(A0)*(5.0/1023.0);
  display.print("Cell Volatage:  ");
  display.println(cellVoltage);
  display.display();
}


int find_direction(int value1, int value2) {
  if (value1 - value2 > 0) { //if difference is positive, the right photoresistor is higher so move to the right
    return 1;
  }
  else { //if difference is negative, the left photoresistor is higher so move to the left
    return -1;
  }
}
