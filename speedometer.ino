#include <Servo.h>

#define MEASURE_TIME 500 // Time over which ticks are logged in ms, ticks older than this number will be discarded
#define MEASURE_TIMES_LENGTH 50
#define METERS_PER_TICK 0.21844 // 2.1844 m/rev, 10 ticks/rev

int tickCount = 0; // Number of ticks in a time interval no longer than MEASURE_TIME
unsigned long measureTimes[MEASURE_TIMES_LENGTH]; // Stores times at which ticks occurred
int head = MEASURE_TIMES_LENGTH;
int tail = 0;

void enqueueTime(float tickTime) {
  head = (head + 1) % MEASURE_TIMES_LENGTH;
  measureTimes[head] = tickTime;
}

void dequeueTime() {
  tail = (tail + 1) % MEASURE_TIMES_LENGTH;
}

void speedometerTick() {
  if (++tickCount < MEASURE_TIMES_LENGTH) {
    // Tick count should always be less that MEASURE_TIMES_LENGTH, otherwise the array should be made longer 
    unsigned long timeNow = millis();
    enqueueTime(timeNow); // Add a tick
    // Remove ticks older than MEASURE_TIME
    removeOldTicks(timeNow);
  } else {
    Serial.println("Tick time array is full!");
    stopForError();
  }  
  /*
  Serial.print(tickCount);
  Serial.print(", ");
  Serial.print(head);
  Serial.print(", ");
  Serial.print(tail);
  Serial.print(", ");
  Serial.println(getTicksPerSecond(), 2);
  */
}

void removeOldTicks(unsigned long timeNow) {
  // Remove ticks older than MEASURE_TIME
  while (tickCount > 0 && measureTimes[tail] + MEASURE_TIME < timeNow) {
    dequeueTime();
    tickCount--;
  }
}

float getTicksPerSecond() {
  removeOldTicks(millis());
  if (tickCount > 0) {
    // Number of ticks in the specified capture time
    return (1000.0 * tickCount)/(measureTimes[head] - measureTimes[tail]);
  } else {
    return 0;
  }
}

float getVelocity() {
  return METERS_PER_TICK * getTicksPerSecond();
}
