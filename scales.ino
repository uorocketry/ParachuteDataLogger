#include "HX711.h"

// HX711 Loadcell Amplifiers
#define LOADCELL_1_DOUT 5
#define LOADCELL_1_SCK 6
#define LOADCELL_2_DOUT 7
#define LOADCELL_2_SCK 8
HX711 scale1;
HX711 scale2;

void initScales() {
  scale1.begin(LOADCELL_1_DOUT, LOADCELL_1_SCK);
  scale1.set_scale(2280.f); // From calibration (not done yet)
  scale1.tare();
  scale2.begin(LOADCELL_2_DOUT, LOADCELL_2_SCK);
  scale2.set_scale(2280.f);
  scale2.tare();
  setScalePower(false);  
}

float getScale1() {
  return scale1.get_units(1);
}

float getScale2() {
  return scale2.get_units(1);
}

bool scalesReady() {
  return scale1.is_ready() && scale2.is_ready();
}

void setScalePower(bool on) {
  if (on) {
    scale1.power_up();
    scale2.power_up();
  } else {
    scale1.power_down();
    scale2.power_down();
  }
}
