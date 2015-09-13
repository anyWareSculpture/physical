
#define STRIPREGEXP "[13]"

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
  Pixel(1, 0),
  Pixel(1, 1),
  Pixel(1, 2),
  Pixel(1, 3),
  Pixel(1, 4),
  Pixel(1, 5),
  Pixel(1, 6),
  Pixel(1, 7),
  Pixel(1, 8),
  Pixel(1, 9)
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
  Sensor(1, 0, IRPin1),
  Sensor(1, 1, IRPin2),
  Sensor(1, 2, IRPin3),
  Sensor(1, 3, IRPin4),
  Sensor(1, 4, IRPin5),
  Sensor(1, 5, IRPin6),
  Sensor(1, 6, IRPin7),
  Sensor(1, 7, IRPin8),
  Sensor(1, 8, IRPin9),
  Sensor(1, 9, IRPin10)
};
DiscreteSensors sensors(STRIP1_SENSORS, strip1_sensors);

#endif
