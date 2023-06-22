#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <Wire.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>   
#include <TM1637Display.h>    // incluye libreria TM1637
#define LED_PIN     2
#define LED_COUNT   5
#define BRIGHTNESS  120 // Set BRIGHTNESS to about 1/5 (max = 255)
#define Servo_pin   8
#define CLK         5     
#define DIO         4     
#define Button_Int  3     

LiquidCrystal_I2C lcd(0x27, 16, 2); 
Servo myservo;  
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ800);
TM1637Display display(CLK, DIO);  // crea objeto 
const int button1   = 12;                   // button1 pin number
const int button2   = 10;                   // button1 pin number
const int button3   = 11;                   // button1 pin number
const int buzzer_pin = 13;                   // Alarms pin number
int pos = 0;    // variable to store the servo position
bool alarm1_status, alarm2_status;
volatile bool Food = false;  
//Buffers
char Time[]     = "  :  :   ",
     calendar[] = "        /  /20  ",
     alarm1[]   = "A1:   :  :00 ",
     alarm2[]   = "A2:   :  :00 ",
  temperature[] = "T:    C";
byte  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;

void setup() { 
  Wire.begin(); 
  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(button3, INPUT_PULLUP);
  pinMode(buzzer_pin, OUTPUT);
  digitalWrite(buzzer_pin, LOW);   
  myservo.attach(Servo_pin);     
  Serial.begin(9600);
  display.setBrightness(7);   // establece nivel de brillo  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(BRIGHTNESS);   
  lcd.init();  
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print(" TygerSys 2021");
  lcd.setCursor(1,1);
  lcd.print(" By Edgar-Cano!");
  delay(2000);       
  attachInterrupt(digitalPinToInterrupt(Button_Int), buttonISR, FALLING);
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }    
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  rainbowFade2White(5, 3, 1); colorWipe(0 , 0, 255, 50); // Blue
  lcd.clear();     
} 
void buttonISR(){
  Serial.println("Button will be used through interrupts");
  Food = true;  
}
void loop() {      
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);  // muestra hora y enciende    
    display.showNumberDec(minute, true, 2, 2);    // muestra minutos
    delay(100);       // demora de 100 mseg.   
  if(!digitalRead(button1)){                    // If B1 button is pressed
      i = 0;
      hour   = edit(0, 0, hour);
      minute = edit(3, 0, minute);
      while(!digitalRead(button1));             // Wait until button B1 released
      while(true){
        while(!digitalRead(button2)){           // If button B2 button is pressed
          day++;                                // Increment day of the week
          if(day > 7) day = 1;
          calendar_display();                   // Call display_calendar function
          lcd.setCursor(0, 1);
          lcd.print(calendar);                  // Display calendar
          delay(200);
        }
        lcd.setCursor(0, 1);
        lcd.print("      ");                       // Print 3 spaces
        Blink();
        lcd.setCursor(0, 1);
        lcd.print(calendar);                    // Print calendar
        Blink();                                // Call Blink function
        if(!digitalRead(button1))               // If button B1 is pressed
          break;
      }
      date = edit(6, 1, date);                  // Edit date
      month = edit(9, 1, month);                // Edit month
      year = edit(14, 1, year);                 // Edit year
      // Convert decimal to BCD
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
      // End conversion
      // Write time & calendar data to DS3231 RTC
      Wire.beginTransmission(0x68);             // Start I2C protocol with DS3231 address
      Wire.write(0);                            // Send register address
      Wire.write(0);                            // Reset sesonds and start oscillator
      Wire.write(minute);                       // Write minute
      Wire.write(hour);                         // Write hour
      Wire.write(day);                          // Write day
      Wire.write(date);                         // Write date
      Wire.write(month);                        // Write month
      Wire.write(year);                         // Write year
      Wire.endTransmission();                   // Stop transmission and release the I2C bus
      delay(200);
    }
    if(!digitalRead(button3)){                  // If B3 button is pressed
      while(!digitalRead(button3));             // Wait until button B3 released
        /* Se visualizan las alarmas en lo que se editan*/
        lcd.setCursor(0, 0);
        lcd.print(alarm1);                            // Display alarm1
        lcd.setCursor(13, 0);
        if (alarm1_status)  lcd.print("ON ");        // If A1IE = 1 print 'ON'
        else                lcd.print("OFF");        // If A1IE = 0 print 'OFF'
        lcd.setCursor(0, 1);
        lcd.print(alarm2);                            // Display alarm2
        lcd.setCursor(13, 1);
        if (alarm2_status)  lcd.print("ON ");        // If A2IE = 1 print 'ON'
        else                lcd.print("OFF");        // If A2IE = 0 print 'OFF'          
      i = 5;
      alarm1_hour   = edit(4,  0, alarm1_hour);
      alarm1_minute = edit(7,  0, alarm1_minute);
      alarm1_status = edit(13, 0, alarm1_status);
      i = 5;
      alarm2_hour   = edit(4,  1, alarm2_hour);
      alarm2_minute = edit(7,  1, alarm2_minute);
      alarm2_status = edit(13, 1, alarm2_status);
      alarm1_minute = ((alarm1_minute / 10) << 4) + (alarm1_minute % 10);
      alarm1_hour   = ((alarm1_hour   / 10) << 4) + (alarm1_hour % 10);
      alarm2_minute = ((alarm2_minute / 10) << 4) + (alarm2_minute % 10);
      alarm2_hour   = ((alarm2_hour   / 10) << 4) + (alarm2_hour % 10);
      // Write alarms data to DS3231
      Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
      Wire.write(7);                              // Send register address (alarm1 seconds)
      Wire.write(0);                              // Write 0 to alarm1 seconds
      Wire.write(alarm1_minute);                  // Write alarm1 minutes value to DS3231
      Wire.write(alarm1_hour);                    // Write alarm1 hours value to DS3231
      Wire.write(0x80);                           // Alarm1 when hours, minutes, and seconds match
      Wire.write(alarm2_minute);                  // Write alarm2 minutes value to DS3231
      Wire.write(alarm2_hour);                    // Write alarm2 hours value to DS3231
      Wire.write(0x80);                           // Alarm2 when hours and minutes match
      Wire.write(4 | alarm1_status | (alarm2_status << 1));      // Write data to DS3231 control register (enable interrupt when alarm)
      Wire.write(0);                              // Clear alarm flag bits
      Wire.endTransmission();                     // Stop transmission and release the I2C bus
      delay(200);                                 // Wait 200ms
    }
    if(!digitalRead(button2) && digitalRead(buzzer_pin)){         // When button B2 pressed with alarm (Reset and turn OFF the alarm)
      strip.setPixelColor(0, 0, 0, 0);   
      strip.show();// off  
      digitalWrite(buzzer_pin, LOW);              // Turn OFF the alarm indicator      
      Wire.beginTransmission(0x68);               // Start I2C protocol with DS3231 address
      Wire.write(0x0E);                           // Send register address (control register)
      // Write data to control register (Turn OFF the occurred alarm and keep the other as it is)
      Wire.write(4 | (!bitRead(status_reg, 0) & alarm1_status) | ((!bitRead(status_reg, 1) & alarm2_status) << 1));
      Wire.write(0);                              // Clear alarm flag bits
      Wire.endTransmission();                     // Stop transmission and release the I2C bus
    }
    DS3231_read();                                // Read time and calendar parameters from DS3231 RTC
    DS3231_display();                             // Display time & calendar
    alarms_read_display();                        // Read and display alarms parameters       
    delay(50);                                    // Wait 50ms
    Alarm();
}
// End of LOOP 

void Alarm() {
  if (alarm1_status || alarm2_status || Food){ 
    if( (second == 0 && hour == alarm1_hour && minute == alarm1_minute) || (Food == true) ){
        lcd.clear();  
        digitalWrite(buzzer_pin, HIGH);
        delay(500);    
        digitalWrite(buzzer_pin, LOW); 
        delay(500);  
        digitalWrite(buzzer_pin, HIGH);
        delay(500);    
        digitalWrite(buzzer_pin, LOW);         
        lcd.setCursor(0, 0);
        lcd.print("COMIDA GATO!!");  
        colorWipe(0 , 255, 0, 50); // Green 
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees          
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position     
         }
        pulseWhite(5);   
        colorWipe(255, 0, 0, 50); // Green
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position                        
        }          
        pulseWhite(5);
        Food = false; 
        lcd.clear();       
        colorWipe(0 , 0, 255, 50); // Blue 
    }  
    if( (second == 0 && hour == alarm2_hour && minute == alarm2_minute )|| (Food == true) ){
        lcd.clear();  
        digitalWrite(buzzer_pin, HIGH);
        delay(500);    
        digitalWrite(buzzer_pin, LOW); 
        delay(500);  
        digitalWrite(buzzer_pin, HIGH);
        delay(500);    
        digitalWrite(buzzer_pin, LOW);         
        lcd.setCursor(0, 0);
        lcd.print("COMIDA GATO!!");  
        colorWipe(0 , 255, 0, 50); // Green 
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees          
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position     
         }
        pulseWhite(5);   
        colorWipe(255, 0, 0, 50); // Green
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position                        
        }          
        pulseWhite(5);
        Food = false; 
        lcd.clear();       
        colorWipe(0 , 0, 255, 50); // Blue 
    } 
  }
}
void DS3231_read() {                            // Function to read time & calendar data
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0);                                // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 7);                    // Request 7 bytes from DS3231 and release I2C bus at end of reading
  second = Wire.read();                         // Read seconds from register 0
  minute = Wire.read();                         // Read minuts from register 1
  hour   = Wire.read();                         // Read hour from register 2
  day    = Wire.read();                         // Read day from register 3
  date   = Wire.read();                         // Read date from register 4
  month  = Wire.read();                         // Read month from register 5
  year   = Wire.read();                         // Read year from register 6
}
void alarms_read_display() {                    // Function to read and display alarm1, alarm2 and temperature data
  byte control_reg, temperature_lsb;
  char temperature_msb;
  Wire.beginTransmission(0x68);                 // Start I2C protocol with DS3231 address
  Wire.write(0x08);                             // Send register address
  Wire.endTransmission(false);                  // I2C restart
  Wire.requestFrom(0x68, 11);                   // Request 11 bytes from DS3231 and release I2C bus at end of reading
  alarm1_minute = Wire.read();                  // Read alarm1 minutes
  alarm1_hour   = Wire.read();                  // Read alarm1 hours
  Wire.read();                                  // Skip alarm1 day/date register
  alarm2_minute = Wire.read();                  // Read alarm2 minutes
  alarm2_hour   = Wire.read();                  // Read alarm2 hours
  Wire.read();                                  // Skip alarm2 day/date register
  control_reg = Wire.read();                    // Read the DS3231 control register
  status_reg  = Wire.read();                    // Read the DS3231 status register
  Wire.read();                                  // Skip aging offset register
  temperature_msb = Wire.read();                // Read temperature MSB
  temperature_lsb = Wire.read();                // Read temperature LSB
  // Convert BCD to decimal
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
 // Convert decimal to ASCII
  alarm1[8]     = alarm1_minute % 10  + 48;
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bitRead(control_reg, 0);      // Read alarm1 interrupt enable bit (A1IE) from DS3231 control register
  alarm2_status = bitRead(control_reg, 1);      // Read alarm2 interrupt enable bit (A2IE) from DS3231 control register
  /*Control de la temperatura*/
  if (temperature_msb < 0) {
    temperature_msb = abs(temperature_msb);
    temperature[2] = '-';
  } else temperature[2] = ' ';
  temperature_lsb >>= 6;
  temperature[4] = temperature_msb % 10  + 48;  // unidades  
  temperature[3] = temperature_msb / 10  + 48;  // decenas
  /* Por cuestiones de espacio no tratar LSB de la temperatura*/
  //temperature[5]  = 223;                        // Put the degree symbol
  lcd.setCursor(9, 0);
  lcd.print(temperature);                       // Display temperature  
  while(!digitalRead(button2)){                 // wait press button 2, display alarms
   while(true){
    lcd.setCursor(0, 0);
    lcd.print(alarm1);                            // Display alarm1
    lcd.setCursor(13, 0);
     if (alarm1_status)  lcd.print("ON ");        // If A1IE = 1 print 'ON'
     else                lcd.print("OFF");        // If A1IE = 0 print 'OFF'
    lcd.setCursor(0, 1);
    lcd.print(alarm2);                            // Display alarm2
    lcd.setCursor(13, 1);
     if (alarm2_status)  lcd.print("ON ");        // If A2IE = 1 print 'ON'
     else                lcd.print("OFF");        // If A2IE = 0 print 'OFF'    
    if(digitalRead(button2)) break;               // If button B2 is free, return display time/calendar        
   }
 }
}
void calendar_display() {                       // Function to display calendar
  switch (day) {
    case 1:  strcpy(calendar, "Dom     /  /20  "); break;
    case 2:  strcpy(calendar, "Lun     /  /20  "); break;
    case 3:  strcpy(calendar, "Mar     /  /20  "); break;
    case 4:  strcpy(calendar, "Mie     /  /20  "); break;
    case 5:  strcpy(calendar, "Jue     /  /20  "); break;
    case 6:  strcpy(calendar, "Vie     /  /20  "); break;
    case 7:  strcpy(calendar, "Sab     /  /20  "); break;
    default: strcpy(calendar, "Sab     /  /20  ");
  }
  calendar[15] = year  % 10 + 48;
  calendar[14] = year  / 10 + 48;
  calendar[10]  = month % 10 + 48;
  calendar[9]  = month / 10 + 48;
  calendar[7]  = date  % 10 + 48;
  calendar[6]  = date  / 10 + 48;
  calendar[5]  = ' ';
  calendar[4]  = ' ';
  lcd.setCursor(0, 1);
  lcd.print(calendar);                          // Display calendar
}
void DS3231_display() {
  // Convert BCD to decimal
  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);  
  Time[7]     = second % 10  + 48;
  Time[6]     = second / 10  + 48;
  Time[4]     = minute % 10  + 48;
  Time[3]     = minute / 10  + 48;
  Time[1]     = hour   % 10  + 48;
  Time[0]     = hour   / 10  + 48;
  calendar_display();                           // Call calendar display function
  lcd.setCursor(0, 0);
  lcd.print(Time);                              // Display time
}

void Blink() {
  byte j = 0;
  while (j < 10 && (digitalRead(button1) || i >= 5)
          && digitalRead(button2) && (digitalRead(button3) || i < 5)) {
    j++;
    delay(25);
  }
}

byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(button1) || !digitalRead(button3));    // Wait until button B1 is released
  while(true){
    while(!digitalRead(button2)){                           // If button B2 is pressed
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)          // If hours > 23 ==> hours = 0 (Time|alarms)
        parameter = 0;                                      
      if(((i == 1) || (i == 6)) && parameter > 59)          // If minutes > 59 ==> minutes = 0 (Time|alarms)
        parameter = 0;
      if(i == 2 && parameter > 31)                          // If date > 31 ==> date = 1
        parameter = 1;
      if(i == 3 && parameter > 12)                          // If month > 12 ==> month = 1
        parameter = 1;
      if(i == 4 && parameter > 99)                          // If year > 99 ==> year = 0
        parameter = 0;
      if(i == 7 && parameter > 1)                           // For alarms ON or OFF (1: alarm ON, 0: alarm OFF)
        parameter = 0;
      lcd.setCursor(x, y);
      if(i == 7){                                           // For alarms print ON  or OFF
        if(parameter == 1)  lcd.print("ON ");
        else                lcd.print("OFF");
      }
      else{
        sprintf(text,"%02u", parameter);
        lcd.print(text);
      }
      delay(200);                               // Wait 200ms
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                            // Print two spaces
      if(i == 7) lcd.print(" ");                // Print space (just for alarms ON & OFF)
    Blink();                                    // Call Blink function
    lcd.setCursor(x, y);
      if(i == 7){                               // For alarms ON & OFF
        if(parameter == 1)  lcd.print("ON ");
        else                lcd.print("OFF");
      }else{
          sprintf(text,"%02u", parameter);
          lcd.print(text);
        }
    Blink();
    if((!digitalRead(button1) && i < 5) || (!digitalRead(button3) && i >= 5)){
      i++;                                      // Increment 'i' for the next parameter
      return parameter;                         // Return parameter value and exit
    }
  }
}
void colorWipe(uint8_t R, uint8_t G, uint8_t B, int wait) {
  for(int i=0; i<LED_COUNT; i++) strip.setPixelColor(i, R,G,B);         
  strip.show();                          //  Update strip to match
  delay(wait); 
}

void pulseWhite(uint8_t wait) {
  for(int j=0; j<256; j++) { // Ramp up from 0 to 255
    // Fill entire strip with white at gamma-corrected brightness level 'j':
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
  for(int j=255; j>=0; j--) { // Ramp down from 255 to 0
    strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
    strip.show();
    delay(wait);
  }
}
void rainbowFade2White(int wait, int rainbowLoops, int whiteLoops) {
  int fadeVal=0, fadeMax=100;
  // Hue of first pixel runs 'rainbowLoops' complete loops through the color
  // wheel. Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to rainbowLoops*65536, using steps of 256 so we
  // advance around the wheel at a decent clip.
  for(uint32_t firstPixelHue = 0; firstPixelHue < rainbowLoops*65536;
    firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...

      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      uint32_t pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());

      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the three-argument variant, though the
      // second value (saturation) is a constant 255.
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue, 255,
        255 * fadeVal / fadeMax)));
    }
    strip.show();
    delay(wait);
    if(firstPixelHue < 65536) {                              // First loop,
      if(fadeVal < fadeMax) fadeVal++;                       // fade in
    } else if(firstPixelHue >= ((rainbowLoops-1) * 65536)) { // Last loop,
      if(fadeVal > 0) fadeVal--;                             // fade out
    } else {
      fadeVal = fadeMax; // Interim loop, make sure fade is at max
    }
  }
  for(int k=0; k<whiteLoops; k++) {
    for(int j=0; j<256; j++) { // Ramp up 0 to 255
      // Fill entire strip with white at gamma-corrected brightness level 'j':
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
    delay(1000); // Pause 1 second
    for(int j=255; j>=0; j--) { // Ramp down 255 to 0
      strip.fill(strip.Color(0, 0, 0, strip.gamma8(j)));
      strip.show();
    }
  }
  delay(500); // Pause 1/2 second
}
