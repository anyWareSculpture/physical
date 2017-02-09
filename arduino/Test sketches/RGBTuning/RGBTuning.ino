// Yes, this is actually -*-c++-*-

uint8_t BLACK[] = {0,0,0};
uint8_t WHITE[] = {255,255,255};
uint8_t RED[] = {255,0,0};
uint8_t GREEN[] = {0,255,0};
uint8_t BLUE[] = {0,0,255};


#define RED_PIN 4
#define GRN_PIN 3
#define BLU_PIN 2
#define RED_PIN2 7
#define GRN_PIN2 6
#define BLU_PIN2 5
#define TOUCH_SENSOR_PIN  31
#define VIB_PIN  11
#define LED1_PIN  9
#define LED2_PIN  8
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

  setColor(0, BLACK);
  setColor(1, BLACK);

  setColor(0, RED);
  delay(500);
  setColor(1, RED);
  delay(500);
  setColor(0, GREEN);
  delay(500);
  setColor(1, GREEN);
  delay(500);
  setColor(0, BLUE);
  delay(500);
  setColor(1, BLUE);
  delay(500);
  setColor(0, WHITE);
  delay(500);
  setColor(1, WHITE);
  
  Serial.begin(115200);
  Serial.println("Hello RGBTuning");
  Serial.println("Usage:");
  Serial.println("   <strip> <red> <green> <blue>");
  Serial.println("   strip            L or R");
  Serial.println("   red,green,blue   0-255");
  Serial.println("Example:");
  Serial.println("L 23 0 254");
}

void setColor(uint8_t strip, uint8_t *color) {
  if (strip == 0) {
    analogWrite(RED_PIN, 255-color[0]);
    analogWrite(GRN_PIN, 255-color[1]);      
    analogWrite(BLU_PIN, 255-color[2]);
  }
  else {
    analogWrite(RED_PIN2, 255-color[0]);
    analogWrite(GRN_PIN2, 255-color[1]);      
    analogWrite(BLU_PIN2, 255-color[2]);
  }
}

void loop()
{
  if (Serial.available() > 0) {
    char c = Serial.read();
    int strip = tolower(c) == 'l' ? 0 : tolower(c) == 'r' ? 1 : -1;
    if (strip >= 0) {
      uint8_t color[3]{};
      color[0] = Serial.parseInt();
      color[1] = Serial.parseInt();
      color[2] = Serial.parseInt();
      setColor(strip, color);
    }
  }
}
