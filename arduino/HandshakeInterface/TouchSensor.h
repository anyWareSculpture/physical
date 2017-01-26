#ifndef TOUCHSENSOR_H_
#define TOUCHSENSOR_H_

// Tuned for proximity sensor
#define TOUCH_THRESHOLD   6
#define	RELEASE_THRESHOLD 4

class TouchSensor {
public:
  TouchSensor(int pin, int addr = 0x5A) : irq_pin(pin), address(addr), status(false)  {}

  void setup();
  bool update();
  bool getStatus() {
    return status;
  }
private:
  int address;
  int irq_pin;
  bool status;
};

#endif
