#define TOUCH_SENSOR_PIN  31
#define VIB_PIN  11
#define LED1_PIN  8
#define LED2_PIN  9
#define LED3_PIN  10

#if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__)
  #error Oops!  Make sure you have 'Arduino/Genuino Mega or Mega 2560' selected from the 'Tools -> Board' menu.
#endif

void setup()
{
  pinMode(TOUCH_SENSOR_PIN, INPUT);
  pinMode(VIB_PIN, OUTPUT);
  digitalWrite(VIB_PIN, 0);
  pinMode(LED1_PIN, OUTPUT);
  digitalWrite(LED1_PIN, 0);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED2_PIN, 0);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED3_PIN, 0);

  Serial.begin(115200);
  Serial.println("Hello HighPowerOn");
}

void loop()
{
}
