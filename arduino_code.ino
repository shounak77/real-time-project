#include <Wire.h>

#include <LiquidCrystal_I2C.h>

#include <string.h>

#define YLED 6  // Setting up LEDs
#define RLED 7

LiquidCrystal_I2C lcd(0x27, 20, 4); // Set the LCD I2C address

const int numReadings = 10;

int readings[numReadings]; // the readings from the analog input
int readIndex = 0; // the index of the current reading
int total = 0; // the running total
int sensor_average = 0; // the average

int inputPin = A0; // pin for reading the acholol level

int yellowledState = LOW; // ledState used to set the LED
int redledState = LOW; // ledState used to set the LED

unsigned long ypreviousMillis = 0; // will store last time LED was updated
unsigned long rpreviousMillis = 0; // will store last time LED was updated

// constants won't change:
const long interval = 1000; // interval at which to blink (milliseconds)

void setup() {
  pinMode(YLED, OUTPUT); // LED on pin 7
  pinMode(RLED, OUTPUT);
  lcd.init(); // Initiate the LCD module
  lcd.backlight(); // Turn on the backlight
  Serial.begin(9600); // just for checkin output, can be removed later
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {

  unsigned long currentMillis = millis();
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(inputPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  sensor_average = total / numReadings;  // Here we are using concept of Moving Window Average for obtaining better output 
  // send it to the computer as ASCII digits

  // STABLE REading = 214
  Serial.println(sensor_average);
  lcd.setCursor(0, 0); // Set the cursor on the first column and first row.
  lcd.print("Alcohol Level");
  lcd.setCursor(0, 2); //Set cursor to 2nd column and 2nd row (counting starts at 0)
  lcd.print(sensor_average);
  lcd.setCursor(4, 2);
  lcd.print("ppm");

  if (sensor_average < 350) { // low value no lights 

    digitalWrite(RLED, LOW); // turn the LED off by making the voltage LOW
    digitalWrite(YLED, HIGH); // turn the LED off by making the voltage LOW
    lcd.setCursor(0, 3);
    clearLCDLine(3);
    lcd.setCursor(0, 3);

  } else if (sensor_average >= 350 && sensor_average < 450) { // 

    if (currentMillis - ypreviousMillis >= interval) {
      // save the last time you blinked the LED
      ypreviousMillis = currentMillis;

      // if the LED is off turn it on and vice-versa:
      if (yellowledState == LOW) {
        yellowledState = HIGH;
      } else {
        yellowledState = LOW;
      }

      // set the LED with the ledState of the variable:
      digitalWrite(YLED, yellowledState);
    }
    clearLCDLine(3);
    lcd.setCursor(0, 3);
    lcd.print("Moderate");

    digitalWrite(RLED, LOW); // turn the LED on (HIGH is the voltage level)

  } else if (sensor_average >= 450 && sensor_average < 550) {
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, HIGH);
    clearLCDLine(3);
    lcd.setCursor(0, 3);
    lcd.print("exceeds tolerance");

  } else {
    if (currentMillis - rpreviousMillis >= interval) {

      rpreviousMillis = currentMillis;

      if (redledState == LOW) {
        redledState = HIGH;
      } else {
        redledState = LOW;
      }

      digitalWrite(RLED, redledState);
    }
    clearLCDLine(3);
    lcd.setCursor(0, 3);
    lcd.print("Too High");

    digitalWrite(YLED, LOW);

  }

  delay(500); // delay in between reads for stability  in display
}

void clearLCDLine(int line) {
  lcd.setCursor(0, line);
  for (int n = 0; n < 20; n++) // 20 indicates symbols in line. For 2x16 LCD write - 16
  {
    lcd.print(" ");
  }
}
