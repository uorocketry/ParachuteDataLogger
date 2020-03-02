unsigned long lastTickTime = 0;
int tickCount = 0;
float velocities[100];

void speedometerTick() {
  if (tickCount < 100) {
    // Tick count should always be less that 100 
    unsigned long thisTickTime = millis();
    float velocity = (1000 * METERS_PER_TICK)/(thisTickTime - lastTickTime);
    lastTickTime = thisTickTime;
  }
}
