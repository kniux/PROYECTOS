#include "RTClib.h"
#include <TimeLib.h>      // incluye libreria TimeLib   
#include <TM1637Display.h>    // incluye libreria TM1637

#define CLK 5       // CLK a pin 2
#define DIO 4       // DIO a pin 3

TM1637Display display(CLK, DIO);  // crea objeto
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday",
"Wednesday", "Thursday", "Friday", "Saturday"};

void setup () {
  Serial.begin(9600);
  
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    abort();
  }

  if (rtc.lostPower()) {
   // Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }  
  display.setBrightness(7);   // establece nivel de brillo
  setTime(0,0,0,21,8,2021);    // establece fecha y horario inicial  
}

void loop() {
    DateTime now = rtc.now();
    display.showNumberDecEx(now.hour(), 0b01000000, false, 2, 0);  // muestra hora y enciende    
    display.showNumberDec(now.minute(), true, 2, 2);    // muestra minutos
    delay(100);       // demora de 100 mseg.   
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

    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    delay(3000);
}
