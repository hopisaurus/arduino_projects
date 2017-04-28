// Date and time functions using a DS3231 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"
// PIN definitions
#define RELAY_PIN 2
// FSM states
#define STATE_OFF  0
#define STATE_ON   1
// Timer settings
#define START_TIME  0134
#define END_TIME    0136
// Variables
RTC_DS3231 rtc;
int fsm_state;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(9600);
  delay(3000); // wait for console opening

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  fsm_state = STATE_OFF;
}

void loop () {
    DateTime now = rtc.now();
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    delay(3000);
    
  int nowHourMinute = now.hour() * 100 + now.minute();
  int nowHour = now.hour();
  int nowMin = now.minute();
  int nowSec = now.second();
  
  // FSM states
  switch(fsm_state) {
    
    case STATE_OFF:
      Serial.print(nowHourMinute);
      Serial.println();
      Serial.print(nowHour);
      Serial.print(':');
      Serial.print(nowMin);
      Serial.print(':');
      Serial.print(nowSec);
      Serial.println();

      if(nowHourMinute > START_TIME && nowHourMinute < END_TIME) {
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.println(", it's time to wake up!");
        digitalWrite(RELAY_PIN, HIGH);
        fsm_state = STATE_ON;
      }
      break;
    
    case STATE_ON:
      if(nowHourMinute > END_TIME) {
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);        
        Serial.println(", it's time to go to sleep!");
        digitalWrite(RELAY_PIN, LOW);
        fsm_state = STATE_OFF;
      }    
      break;
  }


    
}
