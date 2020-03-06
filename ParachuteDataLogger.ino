#include <Servo.h> // For speedometer display

// Button and proximity sensor
#define PROXIMITY_PIN 2
#define BUTTON_PIN 3

// Indicators
#define SERVO_PIN 10
#define LED_PIN 9

bool recording = false; // Logging active
bool startup = false;   // Starting/stopping logging
Servo speedGauge;

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  pinMode(PROXIMITY_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PROXIMITY_PIN), tick, FALLING);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonDown, FALLING);
  speedGauge.attach(SERVO_PIN);
  
  delay(500);
  initSd();
  startup = false;
  
  initScales();
  
  Serial.println("Initialization complete");
  digitalWrite(LED_PIN, LOW);
}

unsigned long logStartTime;
const int LINES_PER_WRITE = 10;
float logData[LINES_PER_WRITE][4];
int logCount = 0;
bool ledToggle = true;

#define NUM_VELOCITIES 10
float velocities[NUM_VELOCITIES]; 
int velocityIdx = 0;
float velocity;
float totalVel;

void loop() {
  velocity  = getVelocity();

  // Moving average of velocites for speedometer display
  totalVel -= velocities[velocityIdx];
  velocities[velocityIdx] = velocity;
  totalVel += velocities[velocityIdx++];
  if (velocityIdx >= NUM_VELOCITIES) {
    velocityIdx = 0;
  }
  speedGauge.write(constrain((totalVel/NUM_VELOCITIES)*15 + 10, 10, 180));
  
  if (scalesReady() && recording) {
    digitalWrite(LED_PIN, ledToggle ? LOW : HIGH);
    ledToggle = !ledToggle;
    
    logData[logCount][0] = (millis() - logStartTime)/1000.0;
    logData[logCount][1] = velocity;
    logData[logCount][2] = getScale1();
    logData[logCount][3] = getScale2();
    
    Serial.print(logData[logCount][0], 3);
    Serial.print(", ");
    Serial.print(logData[logCount][1], 2);
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
    digitalWrite(LED_PIN, HIGH);
    if (recording){
      // Stop recording
      sdWrite(logData, logCount);
      delay(500);
      digitalWrite(LED_PIN, LOW);
    } else {
      // Start recording
      newFile();
      delay(500);
      logStartTime = millis();
      digitalWrite(LED_PIN, LOW);
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

void stopForError() {
  while(1) {
    digitalWrite(LED_PIN, LOW);
    delay(500);
    digitalWrite(LED_PIN, HIGH);
    delay(500);
  }
}
