#ifndef DISCRETESENSORS_H_
#define DISCRETESENSORS_H_

class DiscreteSensors
{
public:
  DiscreteSensors(uint8_t numsensors, Sensor *sensors) : numsensors(numsensors), sensors(sensors) {

  }

  void setup() {
    for (uint8_t i=0;i<numsensors;i++) sensors[i].setup();
  }

  uint8_t getNumSensors() { return numsensors; }
  Sensor &getSensor(uint8_t n) { return sensors[n]; }
  bool readSensor(uint8_t n) { return sensors[n].readSensor(); }
  bool getState(uint8_t n) { return sensors[n].getState(); }

private:
  uint8_t numsensors;
  Sensor *sensors;
};

#endif
