#define STRIPID 2
#define STRIPREGEXP " 2"

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

#define STRIP_LEDS 10
CRGB strip_leds[STRIP_LEDS];
Pixel strip_pixels[STRIP_LEDS] = {
  Pixel(STRIPID, 9),
  Pixel(STRIPID, 8),
  Pixel(STRIPID, 7),
  Pixel(STRIPID, 6),
  Pixel(STRIPID, 5),
  Pixel(STRIPID, 4),
  Pixel(STRIPID, 3),
  Pixel(STRIPID, 2),
  Pixel(STRIPID, 1),
  Pixel(STRIPID, 0)
};
LEDStrip<SPI_DATA, SPI_CLOCK> strip(STRIP_LEDS, strip_leds, strip_pixels);

#define STRIP_SENSORS 10
Sensor strip_sensors[STRIP_SENSORS] = {
  Sensor(STRIPID, 9, IRPin1),
  Sensor(STRIPID, 8, IRPin2),
  Sensor(STRIPID, 7, IRPin3),
  Sensor(STRIPID, 6, IRPin4),
  Sensor(STRIPID, 5, IRPin5),
  Sensor(STRIPID, 4, IRPin6),
  Sensor(STRIPID, 3, IRPin7),
  Sensor(STRIPID, 2, IRPin8),
  Sensor(STRIPID, 1, IRPin9),
  Sensor(STRIPID, 0, IRPin10)
};
DiscreteSensors sensors(STRIP_SENSORS, strip_sensors);

#endif
