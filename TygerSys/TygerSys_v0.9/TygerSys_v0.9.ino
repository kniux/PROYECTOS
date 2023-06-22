/*--------------------------------------------------------------------------------/* 
 *  INTEGRACION:
 *  • Button Arcade
 *  
*/
#include <LiquidCrystal_I2C.h>
#include <TimerOne.h>
#include <Wire.h>
#include <Keypad.h>
#include <Servo.h>
#include <TM1637Display.h>    
#include <Adafruit_NeoPixel.h>   

const byte ROWS = 4;
const byte COLS = 1;
char hexaKeys[ROWS][COLS] = {
  {'1'}, {'2'}, {'3'}, {'4'}
};
      // Keys  -->    | 1 | 2 | 3 | 4 |    en Osoyoo : 33 35 37 39 41 43 45 47

byte rowPins[ROWS] = { 47 , 45, 51 , 49};
byte colPins[COLS] = {53};

boolean button1_Press  = false;
boolean button2_Press  = false;
boolean button3_Press  = false;
boolean button4_Press  = false;
boolean button1_Hold = false;
boolean button2_Hold = false;
boolean button3_Hold = false;
boolean button4_Hold = false;

boolean flag_SC = false;
boolean flag_SA = false;

boolean blink = false;
boolean ledPin_state;

int pos = 0;    // variable to store the servo position
const int LED_COUNT     =  5;

const int ledPin       =  52;   
const int CLK          =  23;     
const int DIO          =  22;     
const int buzzerPin    = 13;   
const int ServoPin     =  50;   
const int buttonArcade =  3;

bool alarm1_status, alarm2_status;
//Buffers
char Time[]     = "  :  :   ",
     calendar[] = "      /  /  ",
     alarm1[]   = "A1:   :  :00 ",
     alarm2[]   = "A2:   :  :00 ",
  temperature[] = "   .  C";
byte  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;

volatile bool Food = false;  

/* Gráfico Campana*/
uint8_t bell[8]={
  B00100,
  B01110,
  B01110,
  B01110,
  B01110,
  B11111,
  B00000,
  B00100
};

uint8_t grade[8]={
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};      
Keypad keypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS );
LiquidCrystal_I2C lcd(0x27, 16, 2); 
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT,ledPin, NEO_GRB + NEO_KHZ800);  
Servo myservo;  
TM1637Display display(CLK, DIO);  

void Alarm() {
  if (alarm1_status || alarm2_status || Food){ 
    if( (second == 0 && hour == alarm1_hour && minute == alarm1_minute) || (Food == true) ){
        beep(4);
        lcd.clear();          
        lcd.setCursor(0, 0);
        lcd.print("COMIDA GATO!!");  
        colorWipe(0 , 255, 0, 50); // Green         
  
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }    
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        Food = false; 
        lcd.clear(); 
        colorWipe(0 , 0, 255, 50); // Blue  
    }  
    if( (second == 0 && hour == alarm2_hour && minute == alarm2_minute )|| (Food == true) ){
        beep(4);
        lcd.clear();          
        lcd.setCursor(0, 0);
        lcd.print("COMIDA GATO!!");  
        colorWipe(0 , 255, 0, 50); // Green         
  
        for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }    
        for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
          // in steps of 1 degree
          myservo.write(pos);              // tell servo to go to position in variable 'pos'
          delay(15);                       // waits 15ms for the servo to reach the position
        }
        Food = false; 
        lcd.clear(); 
    }
     colorWipe(0 , 0, 255, 50); // Blue   
  }
}

void setup() { 
  Wire.begin();
  Serial.begin(9600);  
  pinMode(ledPin, OUTPUT);
  pinMode(buttonArcade, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);  
  digitalWrite(buzzerPin, LOW);      
  lcd.init();
  lcd.backlight();                  
  lcd.createChar(0, bell);  
  lcd.createChar(1, grade);
  lcd.write(1);
  lcd.clear();     
  strip.begin();       // inicializacion de la tira
  strip.setBrightness(255); // Brillo de los LEDs
  strip.show();
  myservo.attach(ServoPin);  
  display.setBrightness(7);   // establece nivel de brillo     
/* Añadiendo Interrupción por cambio de estado al PIN 2 */
  attachInterrupt(digitalPinToInterrupt(buttonArcade), buttonISR, FALLING);
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad  
/* Prueba de calibración*/
  lcd.setCursor(0,0);
  lcd.print(" TygerSys 2021");
  lcd.setCursor(1,1);
  lcd.print(" Version 0.8!");  
  rainbowFade2White(5, 3, 1);
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }    
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  lcd.clear();     
/* Fin calibración*/
  colorWipe(0 , 0, 255, 50); // Blue  
} 

void buttonISR(){
  Serial.println("Button will be used through interrupts");
  Food = true;  
}

void loop() { 
    display.showNumberDecEx(hour, 0b01000000, true, 2, 0);  // muestra hora y enciende    
    display.showNumberDec(minute, true, 2, 2);    // muestra minutos
    char key = keypad.getKey();
    if (key) Serial.println(key);        
  /* Set Time */  
  if(button1_Hold){                    // If B1 button is holding     
      beep(2);      
      flag_SC = true;
      i = 0;     
      hour   = edit(0, 0, hour);      
      minute = edit(3, 0, minute);  
       
      while(true){
        char key = keypad.getKey();
        if (key) Serial.println(key);   
           
        if(button3_Press){                    // If button B3 is pressed 
          day++;                                // Increment day of the week
          if(day > 7) day = 1;          
          calendar_display();                   // Call display_calendar function
          lcd.setCursor(0, 1);
          lcd.print(calendar);                  // Display calendar                  
        }
        if(button2_Press){                    // If button B3 is pressed 
          day--;                                // Increment day of the week
          if(day < 1) day = 7;
          calendar_display();                   // Call display_calendar function
          lcd.setCursor(0, 1);
          lcd.print(calendar);                  // Display calendar                    
        }        
        lcd.setCursor(0, 1);
        lcd.print("   ");                   // Print 3 spaces 
        delay(40);                         // Wait 200ms             
        lcd.setCursor(0, 1);          
        lcd.print(calendar);                // Print calendar

        if(button4_Press){               // If button B1 is pressed
          delay(200);                    // OK
          button4_Press = false;                        // asegurando que se desactivo flag
          break;
        }   
      }      
      date = edit(4, 1, date);                  // Edit date
      month = edit(7, 1, month);                // Edit month
      year = edit(10, 1, year);                 // Edit year      
      /*
      lcd.clear();
      lcd.setCursor(2, 1);          
      lcd.print("Set Clock OK!");     
      */      
      beep(3);       
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
      flag_SC = false;
    }
    
    if(button2_Hold){                  // If B3 button is pressed         
        beep(2);
        flag_SA = true;
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
      /*
      lcd.clear();
      lcd.setCursor(1, 1);          
      lcd.print("Set Alarms OK!");     
      */
      beep(3);            
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
      flag_SA = false;
    }
    /* Set Alarms */  
    if((button2_Press) && digitalRead(buzzerPin)){         // When button B2 pressed with alarm (Reset and turn OFF the alarm)
      
      strip.setPixelColor(0, 0, 0, 0);   
      strip.show();// off  
      digitalWrite(buzzerPin, LOW);              // Turn OFF the alarm indicator      
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
    temperature[0] = '-';
  }
  else
  temperature[0] = ' ';
  temperature_lsb >>= 6;
  temperature[2] = temperature_msb % 10  + 48;
  temperature[1] = temperature_msb / 10  + 48;
  if (temperature_lsb == 0 || temperature_lsb == 2) {   // aprox to .00 or .50
   // temperature[5] = '0';
    if (temperature_lsb == 0) temperature[4] = '0';
    else                     temperature[4] = '5';
  }
  if (temperature_lsb == 1 || temperature_lsb == 3) {  // aprox to .25 or .75
  //  temperature[5] = '5';
    if (temperature_lsb == 1) temperature[4] = '2';
    else                      temperature[4] = '7';
  }
  //temperature[5]  = 167;                        // Put the degree symbol    
  lcd.setCursor(9, 0);
  lcd.print(temperature);                       // Display temperature    
  
  /* Draw C Grade */
  lcd.setCursor(14,0);
  lcd.write(1); 
  
    /* Draw Bell */
    if (alarm1_status){
        lcd.setCursor(13,1);
        lcd.write(0);                            // Display Bell alarm1        
    }else{
      lcd.setCursor(13,1);
      lcd.print(" ");                            
    }
    if (alarm2_status) {
      lcd.setCursor(14,1);
      lcd.write(0);                            // Display Bell alarm2        
    }else{
      lcd.setCursor(14,1);
      lcd.print("  ");                         //  Dos espacios para borrar OFF      
    }
  
  if(button3_Press){                            // wait press button 3, display alarms     
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
    delay(2000);     
    button3_Press = false;                        // asegurando que se desactivo flag
 }
}
void calendar_display() {                       // Function to display calendar
  switch (day) {
    case 1:  strcpy(calendar, "Dom   /  /  "); break;
    case 2:  strcpy(calendar, "Lun   /  /  "); break;
    case 3:  strcpy(calendar, "Mar   /  /  "); break;
    case 4:  strcpy(calendar, "Mie   /  /  "); break;
    case 5:  strcpy(calendar, "Jue   /  /  "); break;
    case 6:  strcpy(calendar, "Vie   /  /  "); break;
    case 7:  strcpy(calendar, "Sab   /  /  "); break;
    default: strcpy(calendar, "Sab   /  /  ");
  }
  calendar[11] = year  % 10 + 48;
  calendar[10] = year  / 10 + 48;
  calendar[8]  = month % 10 + 48;
  calendar[7]  = month / 10 + 48;
  calendar[5]  = date  % 10 + 48;
  calendar[4]  = date  / 10 + 48;
  calendar[3]  = ' ';
 // calendar[2]  = ' ';
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
  while (j < 5 && (button1_Press || i >= 5)
          && (button1_Press && button3_Press || i < 5)) {
    j++;
    delay(25);
  }
}
// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    
    switch (keypad.getState()){      
    case PRESSED:   
    beep(1);
        if (key == '1') {               
            button1_Press = true;  
            Serial.print("button1_Press: ");   Serial.println(button1_Press);        
        }
        if (key == '2') {                
            button2_Press = true;     
            Serial.print("button2_Press: ");   Serial.println(button2_Press);     
        }
        if (key == '3') {                
            button3_Press = true;          
            Serial.print("button3_Press: ");   Serial.println(button3_Press);
        }
        if (key == '4') {                
            button4_Press = true; 
            Serial.print("button4_Press: ");   Serial.println(button4_Press);         
        }       
    break;         
 // Ajuste de hora          
    case HOLD:
        if (key == '1') {      
            button1_Hold = true;  
            Serial.print(" | button1_Hold: "); Serial.println(button1_Hold);        
        }
        if (key == '2') {                
            button2_Hold = true;  
            Serial.print(" | button2_Hold: "); Serial.println(button2_Hold);        
        }
        if (key == '3') {                
            button3_Hold = true;  
            Serial.print(" | button3_Hold: "); Serial.println(button3_Hold);        
        }
        if (key == '4') {                
            button4_Hold = true;    
            Serial.print(" | button4_Hold: "); Serial.println(button4_Hold);      
        }
    break;    
    case RELEASED:    
        if (key == '1') {         
            button1_Press = false;        
            button1_Hold = false;    
            Serial.println("button1 RELEASED");
        }
        if (key == '2') {             
            button2_Press = false;           
            button2_Hold = false;          
            Serial.println("button2 RELEASED");
        }
        if (key == '3') {                
            button3_Press = false;        
            button3_Hold = false;          
            Serial.println("button3 RELEASED");
        }
        if (key == '4') {                
            button4_Press = false;        
            button4_Hold = false;          
            Serial.println("button4 RELEASED");
        }
    break;
    }    
}

byte edit(byte x, byte y, byte parameter){
  char text[3];    
  delay(200);                  // waiting to finish released event 
  while(true){          
      char key = keypad.getKey();
      if (key) Serial.println(key);    
      if(button3_Press){                                     // If button B3 is pressed      
        parameter++;
        if(((i == 0) || (i == 5)) && parameter > 23)          // If hours > 23 ==> hours = 0 (Time|alarms)
          parameter = 0;                                      
        if(((i == 1) || (i == 6)) && parameter > 59)          // If minutes > 59 ==> minutes = 0 (Time|alarms)
          parameter = 0;
        if(i == 2 && parameter > 31)                          // If date > 31 ==> date = 1
          parameter = 1;
        if(i == 3 && parameter > 12)                          // If month > 12 ==> month = 1
          parameter = 1;
        if(i == 4 && parameter > 50)                          // If year > 50 ==> year = 0
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
    if(button2_Press){                                     // If button B3 is pressed              
      parameter--;
        if(((i == 0) || (i == 5)) && parameter == 255)      // If hours < 0 ==> hours = 23 (Time|alarms)
          parameter = 23;                                     
        if(((i == 1) || (i == 6)) && parameter == 255)      // If minutes > 0 ==> minutes = 59 (Time|alarms)
          parameter = 59;
        if(i == 2 && parameter < 1)                         // If date < 1 ==> date = 31
          parameter = 31;
        if(i == 3 && parameter < 1)                          // If month < 1 ==> month = 12
          parameter = 12;
        if(i == 4 && parameter == 255)                       // If year < 0 ==> year = 50
          parameter = 50;
        if(i == 7 && parameter == 255)                       // For alarms ON or OFF (1: alarm ON, 0: alarm OFF)
          parameter = 1;         
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
   
    delay(25);
    lcd.setCursor(x, y);      
      if(i == 7){                               // For alarms ON & OFF
        if(parameter == 1)  lcd.print("ON ");
        else                lcd.print("OFF");
      }else{
          sprintf(text,"%02u", parameter);
          lcd.print(text);
        }     
 
    if(button4_Press) {
        Serial.println("Entra OK");
        i++;                                      // Increment 'i' for the next parameter
        delay(200);                               // Evitando rebote
        return parameter;                         // Return parameter value and exit        
    }
  }
}

void beep(byte type){
  if(type == 1){      //key sound
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);  
  }
  if(type == 2){      //set configuration sound
    digitalWrite(buzzerPin, HIGH);
    delay(50);      
    digitalWrite(buzzerPin, LOW);
    delay(50);      
    digitalWrite(buzzerPin, HIGH);
    delay(50);
    digitalWrite(buzzerPin, LOW);    
  }
  if(type == 3){      //end configuration sound
    delay(150);
    digitalWrite(buzzerPin, LOW);  
    digitalWrite(buzzerPin, HIGH);
    delay(200);      
    digitalWrite(buzzerPin, LOW);
    delay(50);      
    digitalWrite(buzzerPin, HIGH);
    delay(200);
    digitalWrite(buzzerPin, LOW);    
  }
  if(type == 4){      //Free food sound
    digitalWrite(buzzerPin, HIGH);
    delay(500);    
    digitalWrite(buzzerPin, LOW); 
    delay(500);  
    digitalWrite(buzzerPin, HIGH);
    delay(500);    
    digitalWrite(buzzerPin, LOW);
  }  
}

/* Rutinas para tira de leds*/
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
