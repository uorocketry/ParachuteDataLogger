// Button and proximity sensor
const int PROXIMITY_PIN = 2;
const int BUTTON_PIN = 3;
const float METERS_PER_TICK = 0.5;

// Indicator LEDs
const int GREEN_PIN = 9;
const int RED_PIN = 10;

bool recording = false; // Logging active
bool startup = false;   // Starting/stopping logging

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
  
  initScales();
  
  Serial.println("Initialization complete");
  digitalWrite(RED_PIN, LOW);
}

unsigned long logStartTime;
const int LINES_PER_WRITE = 10;
float logData[LINES_PER_WRITE][4];
int logCount = 0;

void loop() {
  if (scalesReady() && recording) {
    logData[logCount][0] = (millis() - logStartTime)/1000.0;
    logData[logCount][1] = 0.0;
    logData[logCount][2] = getScale1();
    logData[logCount][3] = getScale2();
    
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

  if (startup) {
    digitalWrite(RED_PIN, HIGH);
    if (recording){
      // Stop recording
      sdWrite(logData, logCount);
      delay(1000);
      digitalWrite(GREEN_PIN, LOW);
      digitalWrite(RED_PIN, LOW);
    } else {
      // Start recording
      newFile();
      delay(1000);
      logStartTime = millis();
      digitalWrite(RED_PIN, LOW);
      digitalWrite(GREEN_PIN, HIGH);
    }
    recording = !recording;
    setScalePower(recording);
    startup = false;
  }
}

void tick() {
  speedometerTick();
}

void buttonDown() {
  if (!startup) {
    startup = true;
  }
}
