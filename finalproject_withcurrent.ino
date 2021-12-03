int threshold = 5;
int direction_of_rotation; //hold value of 1 for right and -1 for left
float cellVoltage;

#include <Servo.h>
#include <EEPROM.h>
#include <EEWrap.h>
#include <Wire.h>

//Include Current Sensor Library
#include <Adafruit_INA219.h>
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

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 23;    // variable to store the servo position

int16_e ps1Min EEMEM;
int16_e ps1Max EEMEM;
int16_e ps2Min EEMEM;
int16_e ps2Max EEMEM;

//Photoresistor Constants
const int ps1Pin = A0;
const int ps2Pin = A1;
int ps1;
int ps2;

//declare current sensor
Adafruit_INA219 ina219 = Adafruit_INA219();

//States for state machine
enum states {normal, calibrate};
enum states state;

void setup() {
  Serial.begin(115200);
  myservo.attach(6);
  display.begin(SSD1306_SWITCHCAPVCC); //begin the OLED display communication
  pinMode(4, INPUT_PULLUP);
  state = normal;
  if (!ina219.begin()) {
    Serial.println("Couldn't find INA260 chip");
    while (1);
  }
  Serial.println("Found INA260 chip");

  
}


void loop() {
  switch (state)
  {
    case normal:
      while (digitalRead(4) == HIGH) {
        display.clearDisplay(); //clear the OLED display from what was writted on it last
        display.setTextSize(1); //set the text size for the display
        display.setTextColor(WHITE); //set the text color for the display
        display.setCursor(0, 0); //set the pointer for the cursor
        ps1 = analogRead(A0);
        //Serial.println("Analog value right: ");
        //Serial.println(value1);
        //delay(1000);
        ps2 = analogRead(A1);
        Serial.println("Start");
        // Serial.println(ps1);

        //Constrain photoresistors in case values are outside of calibration range
        ps1 = constrain(ps1, ps1Min, ps1Max);
        ps2 = constrain(ps2, ps2Min, ps2Max);
        //Serial.println(ps1);

        //Apply calibrations
        ps1 = map(ps1, ps1Min, ps1Max, 0, 255);
        ps2 = map(ps2, ps2Min, ps2Max, 0, 255);
        Serial.println(ps1Min);
        Serial.println(ps1Max);

        //Serial.print("Photoresistor 1:  ");
        //Serial.print(ps1);
        //Serial.print("   Photoresistor 2:  ");
        //Serial.println(ps2);
        //Serial.println("Analog value left: ");
        //Serial.println(value2);
        //delay(1000);
        if (abs(ps1 - ps2) > threshold) {
          direction_of_rotation = find_direction(ps1, ps2);
          if (direction_of_rotation > 0 && pos < 45) {
            pos++;
            myservo.write(pos);
            delay(15);
          } else if (direction_of_rotation < 0 && pos > 0)
            pos--;
          myservo.write(pos);
          delay(15);
        }
        //Serial.println("Direction of rotation: ");
        //Serial.println(direction_of_rotation);
        delay(100);

        cellVoltage = analogRead(A2) * (5.0 / 700.0);
        display.print("Cell Volatage:  ");
        display.println(cellVoltage);
        
       // float currentA = ina260.readCurrent() / 1000; //Read current and convert to Amps
        //Serial.println(analogRead(A2));
        display.print("Current: ");
       // display.print(currentA);
       display.print(ina219.getCurrent_mA()/1000.0);
        display.println(" A");
        display.display();

      }
      state = calibrate;
      break;
    case calibrate:
      //calibrating
      Serial.println("calibrating");
      int start = millis(); //take start time
      ps1Min = 1023;
      ps1Max = 0;
      ps2Min = 1023;
      ps2Max = 0;

      while (millis() - start < 10000) { //first five seconds
        ps1 = analogRead(ps1Pin); //read photoresistor 1
        ps2 = analogRead(ps2Pin); //read photoresistor 2

        //PS1 Ccalibration

        //Serial.println(ps1);
        Serial.println(ps1Max);
        Serial.println(ps1Min);

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
      state = normal;
      delay(500);
      break;
  }

}

int find_direction(int value1, int value2) {
  if (value1 - value2 > 0) { //if difference is positive, the right photoresistor is higher so move to the right
    return 1;
  }
  else { //if difference is negative, the left photoresistor is higher so move to the left
    return -1;
  }
}
