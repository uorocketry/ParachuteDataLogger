#include "HX711.h"
#include <SPI.h>
#include <SD.h>

// HX711
const int LOADCELL_1_DOUT = 5;
const int LOADCELL_1_SCK = 6;
const int LOADCELL_2_DOUT = 7;
const int LOADCELL_2_SCK = 8;

const int PROXIMITY_PIN = 2;
const int BUTTON_PIN = 3;
const int GREEN_PIN = 9;
const int RED_PIN = 10;

HX711 scale1;
HX711 scale2;

bool recording = false;
bool startup = false;

void setup() {
  Serial.begin(115200);
  
  pinMode(RED_PIN, OUTPUT);
  digitalWrite(RED_PIN, HIGH);
  pinMode(GREEN_PIN, OUTPUT);
  digitalWrite(GREEN_PIN, LOW);
  
  pinMode(PROXIMITY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN), tick, FALLING);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonDown, FALLING);

  delay(1000);
  initSd();
  startup = false;

  scale1.begin(LOADCELL_1_DOUT, LOADCELL_1_SCK);
  scale1.set_scale(2280.f); // From calibration (not done yet)
  scale1.tare();
  scale2.begin(LOADCELL_2_DOUT, LOADCELL_2_SCK);
  scale2.set_scale(2280.f);
  scale2.tare();
  
  scale1.power_down();
  scale2.power_down();
  
  Serial.println("Initialization complete");
  digitalWrite(RED_PIN, LOW);
}

int tickCount = 0;
unsigned long lastTime = 0;
unsigned long interval = 1000;
const int LINES_PER_WRITE = 10;
float logData[LINES_PER_WRITE][4];
int logCount = 0;
unsigned long logStartTime;

void loop() {
  if (scale1.is_ready() && scale2.is_ready() && recording) {
    logData[logCount][0] = (millis() - logStartTime)/1000.0;
    logData[logCount][1] = 0.0;
    logData[logCount][2] = scale1.get_units(1);
    logData[logCount][3] = scale2.get_units(1);
    
    Serial.print(logData[logCount][0], 3);
    Serial.print(", ");
    Serial.print(logData[logCount][1], 1);
    Serial.print(", ");
    Serial.print(logData[logCount][2], 1);
    Serial.print(", ");
    Serial.println(logData[logCount][3], 1);
    
    logCount++;
    if (logCount >= LINES_PER_WRITE) {
      sdWrite(logData, LINES_PER_WRITE);
      logCount = 0;
    }
  }

  if (lastTime + interval <= millis()) {
    lastTime = lastTime + interval;
    //Serial.println(tickCount, 1);
    tickCount = 0;
  }

  if (startup) {
    digitalWrite(RED_PIN, HIGH);
    if (recording){
      // Stop recording
      scale1.power_down();
      scale2.power_down();
      sdWrite(logData, logCount);
      delay(1000);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
    } else {
      // Start recording
      startup = true;
      scale1.power_up();
      scale2.power_up();
      newFile();
      delay(1000);
      logStartTime = millis();
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
    }
    recording = !recording;
    startup = false;
  }
}

void tick() {
  tickCount++;
}

void buttonDown() {
  if (!startup) {
    startup = true;
  }
}
