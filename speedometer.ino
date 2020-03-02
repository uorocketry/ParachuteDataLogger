#define MEASURE_TIME 1000 // Time over which ticks are logged in ms, ticks older than this number will be discarded
#define MEASURE_TIMES_LENGTH 80

int tickCount = 0; // Number of ticks in a time interval no longer than MEASURE_TIME
unsigned long measureTimes[MEASURE_TIMES_LENGTH]; // Stores times at which ticks occurred
int head = 0;
int tail = 0;

void enqueueTime(float tickTime) {
  measureTimes[head] = tickTime;
  head = (head + 1) % MEASURE_TIMES_LENGTH;
}

void dequeueTime() {
  tail = (tail - 1) % MEASURE_TIMES_LENGTH;
}

void speedometerTick() {
  if (++tickCount < MEASURE_TIMES_LENGTH) {
    // Tick count should always be less that MEASURE_TIMES_LENGTH, otherwise the array should be made longer 
    unsigned long timeNow = millis();
    enqueueTime(timeNow); // Add a tick
    // Remove ticks older than MEASURE_TIME
    while (measureTimes[tail] > timeNow + MEASURE_TIME) {
      dequeueTime();
      tickCount--;
    }
    // Remove ticks older than MEASURE_TIME
    while (tickCount > 0 && measureTimes[tail] > timeNow + MEASURE_TIME) {
      dequeueTime();
      tickCount--;
    }
  } else {
    Serial.println("Tick time array is full!");
    stopForError();
  }
}

float getTicksPerSecond() {
  if (head >= tail) {
    // Number of ticks in the specified capture time
    return (1000.0 * tickCount)/(measureTimes[head] - measureTimes[tail]);
  } else {
    return 0;
  }
}

void getVelocity() {
  
}
