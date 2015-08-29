#ifndef SHARPSENSOR_H_
#define SHARPSENSOR_H_

class SharpSensor {
public:
  SharpSensor(int8_t pin)
    : sensorPin(pin), state(false) {}

  void setup() {
    if (sensorPin >= 0) {
      pinMode(sensorPin, INPUT_PULLUP);
    }
  }
  
  // Returns true if sensor state changed
  bool readSensor() {
    bool newstate = !digitalRead(sensorPin);
    bool changed = (newstate != this->state);
    this->state = newstate;
    return changed;
  }

  bool getState() {
    return this->state;
  }

protected:
  int8_t sensorPin;

  bool state;
};

#endif
