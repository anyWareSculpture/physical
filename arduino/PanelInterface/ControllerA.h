#define STRIPID 0
#define STRIPREGEXP " 0"

#ifdef ANYWARE_MAIN

// Pins
const int IRPin1 = 2;  // IR pins
const int IRPin2 = 3;
const int IRPin3 = 4;
const int IRPin4 = 5;
const int IRPin5 = 6;
const int IRPin6 = 7;
const int IRPin7 = 8;
const int IRPin8 = 9;
const int IRPin9 = 14;
const int IRPin10 = 15;

#define STRIP1_LEDS 10
CRGB strip1_leds[STRIP1_LEDS];
Pixel strip1_pixels[STRIP1_LEDS] = {
  Pixel(STRIPID, 0),
  Pixel(STRIPID, 1),
  Pixel(STRIPID, 2),
  Pixel(STRIPID, 3),
  Pixel(STRIPID, 4),
  Pixel(STRIPID, 5),
  Pixel(STRIPID, 6),
  Pixel(STRIPID, 7),
  Pixel(STRIPID, 8),
  Pixel(STRIPID, 9)
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip1(STRIP1_LEDS, strip1_leds, strip1_pixels);

#define STRIP3_LEDS 6
CRGB strip3_leds[STRIP3_LEDS];
Pixel strip3_pixels[STRIP3_LEDS] = {
  Pixel(3, 0),
  Pixel(3, 1),
  Pixel(3, 2),
  Pixel(3, 3),
  Pixel(3, 4),
  Pixel(3, 5),
};
LEDStrip<16, 17> strip3(STRIP3_LEDS, strip3_leds, strip3_pixels);

#define STRIP1_SENSORS 10
Sensor strip1_sensors[STRIP1_SENSORS] = {
  Sensor(STRIPID, 0, IRPin1),
  Sensor(STRIPID, 1, IRPin2),
  Sensor(STRIPID, 2, IRPin3),
  Sensor(STRIPID, 3, IRPin4),
  Sensor(STRIPID, 4, IRPin5),
  Sensor(STRIPID, 5, IRPin6),
  Sensor(STRIPID, 6, IRPin7),
  Sensor(STRIPID, 7, IRPin8),
  Sensor(STRIPID, 8, IRPin9),
  Sensor(STRIPID, 9, IRPin10)
};
DiscreteSensors sensors(STRIP1_SENSORS, strip1_sensors);

#endif
