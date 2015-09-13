#ifndef SENSOR_H_
#define SENSOR_H_

#include "anyware_global.h"
#include "SharpSensor.h"
#include "AnywareEasing.h"

class Sensor {
public:
  Sensor(uint8_t strip, uint8_t panel, int8_t pin)
    : strip(strip), panel(panel), ir(pin) {}

  virtual void setup() { ir.setup(); }
  virtual bool hasSensor() { return ir.hasSensor(); }
  virtual bool readSensor() { return ir.readSensor(); }
  virtual bool getState() { return ir.getState(); }

  uint8_t strip;
  uint8_t panel;
  SharpSensor ir;
};

#endif
