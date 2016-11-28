uint8_t colorA[] = {255,0,0};
uint8_t colorB[] = {0,0,255};


#define RED_PIN 4
#define GRN_PIN 3
#define BLU_PIN 2
#define RED_PIN2 7
#define GRN_PIN2 6
#define BLU_PIN2 5
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
  digitalWrite(LED1_PIN, 1);
  pinMode(LED2_PIN, OUTPUT);
  digitalWrite(LED2_PIN, 1);
  pinMode(LED3_PIN, OUTPUT);
  digitalWrite(LED3_PIN, 1);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GRN_PIN, OUTPUT);   
  pinMode(BLU_PIN, OUTPUT); 
  pinMode(RED_PIN2, OUTPUT);
  pinMode(GRN_PIN2, OUTPUT);   
  pinMode(BLU_PIN2, OUTPUT); 

  analogWrite(RED_PIN, 255-colorA[0]);
  analogWrite(GRN_PIN, 255-colorA[1]);      
  analogWrite(BLU_PIN, 255-colorA[2]); 
  analogWrite(RED_PIN2, 255-colorB[0]);
  analogWrite(GRN_PIN2, 255-colorB[1]);      
  analogWrite(BLU_PIN2, 255-colorB[2]); 

  Serial.begin(115200);
  Serial.println("Hello RGBStrips");
}

void loop()
{
}
