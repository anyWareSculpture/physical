#ifndef SHARPSENSOR_H_
#define SHARPSENSOR_H_

#include "./Bounce2.h"

// Sensitivity in millisecond. Less is more sensitive
#define IR_SENSITIVITY 10

class SharpSensor {
public:
  SharpSensor(int8_t pin)
    : sensorPin(pin), state(false) {}

  void setup() {
    if (sensorPin >= 0) {
      pinMode(sensorPin, INPUT_PULLUP);
      irState.attach(sensorPin);
      irState.interval(IR_SENSITIVITY);
    }
  }
  
  // Returns true if sensor state changed
  bool readSensor() {
    if (irState.update()) {
      this->state = !irState.read();
      return true;
    }
    return false;
  }

  bool getState() {
    return this->state;
  }

protected:
  int8_t sensorPin;
  Bounce irState;
  bool state;
};

#endif
