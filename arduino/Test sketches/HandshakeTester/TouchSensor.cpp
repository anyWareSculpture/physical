#include "TouchSensor.h"
#include <Arduino.h>
#include <Wire.h>
#include "./MPR121.h"

void TouchSensor::setup()
{
  Wire.begin();
  MPR121.begin(address);
  
  MPR121.setInterruptPin(irq_pin);
  
  // Enable _only_ electrode 0
  MPR121.setRegister(ECR, 0x81);
  
  MPR121.setTouchThreshold(TOUCH_THRESHOLD);
  MPR121.setReleaseThreshold(RELEASE_THRESHOLD);  
}

bool TouchSensor::update()
{
  bool changed = MPR121.touchStatusChanged();
  if (changed) {
    MPR121.updateTouchData();
    if (MPR121.isNewTouch(0)) status = true;
    else if (MPR121.isNewRelease(0)) status = false;
  }
  return changed;
}
