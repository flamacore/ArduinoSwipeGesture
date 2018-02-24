
#include <neotimer.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
 
#define SONAR_NUM     2 // Number or sensors.
#define MAX_DISTANCE 90// Max distance in cm.
#define PING_INTERVAL 33 // Milliseconds between pings.
#define ledUp 12
#define ledDown 7
 
unsigned long pingTimer[SONAR_NUM]; // When each pings.
unsigned int cm[SONAR_NUM]; // Store ping distances.
uint8_t currentSensor = 0; // Which sensor is active.
uint8_t lastSensor = 0; // Which sensor is active.
int value1;
int value2;

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified module
Neotimer thetimer = Neotimer(1000);
 
NewPing sonar[SONAR_NUM] = { // Sensor object array.
  NewPing(9, 10, MAX_DISTANCE),
  NewPing(11, 8, MAX_DISTANCE),
};
 
void setup() {

pinMode(ledUp, OUTPUT);
pinMode(ledDown, OUTPUT);
    digitalWrite(ledUp, HIGH);
    digitalWrite(ledDown, HIGH);
  Serial.begin(115200);
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(LOW); 
  lcd.begin(16, 2);
  lcd.clear();
  pingTimer[0] = millis() + 75; // First ping start in ms.
  thetimer.start();
  for (uint8_t i = 1; i < SONAR_NUM; i++)
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
}

void loop() {
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if (i == 0 && currentSensor == SONAR_NUM - 1)
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = 0;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }
  
  if(thetimer.repeat(300)){
    lcd.setCursor(0,1);
    lcd.print("Reset");
    value1 = 0;
    value2 = 0;
  }
  
    lcd.setCursor(0,0);
    lcd.print("Working");
    lcd.setCursor(0,1);
    lcd.print("No Action");
    
  if(cm[0] > 0 || cm[1] > 0)
  {
    Serial.print("0");
    Serial.print("=");
    Serial.print(cm[0]);
    Serial.print("cm ");
    Serial.print("1");
    Serial.print("=");
    Serial.print(cm[1]);
    Serial.print("cm ");
    Serial.println();
  }
  if (cm[0] > 10){
    value1 = 1;
    thetimer.reset();
    delay(50);
  }
    
  if (cm[1] > 10 && value1 == 1){
    value1 = 0;
    goUp();
    digitalWrite(ledUp, LOW);
    delay(400);
    digitalWrite(ledUp, HIGH);
    lcd.clear();
    value1 = 0;
    value2 = 0;
  }

  //set value 2 true
  if (cm[1] > 10){
    value2 = 1;
    thetimer.reset();
    delay(50);
  }
  
  if (cm[0] > 10 && value2 == 1){
    value2 = 0;
    goDown();
    digitalWrite(ledDown, LOW);
    delay(400);
    digitalWrite(ledDown, HIGH);
    lcd.clear();
    value1 = 0;
    value2 = 0;
  }
}
 void goUp()
 {
   Serial.println("MovingUpStarted");
    lcd.setCursor(0,1);
    lcd.print("Go Up Trigger");
    value1 = 0;
    value2 = 0;
 }
 void goDown()
 {
   Serial.println("MovingDownStarted");
    lcd.setCursor(0,1);
    lcd.print("Go Down Trigger");
    value1 = 0;
    value2 = 0;
 }
void echoCheck() { // If ping echo, set distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}
