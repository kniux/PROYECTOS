#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 1;

char hexaKeys[ROWS][COLS] = {
  {'2'},
  {'1'},
  {'4'},
  {'3'}
};

byte rowPins[ROWS] = {12, 11, 10, 9};
byte colPins[COLS] = {8};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);  

void setup(){
  lcd.backlight();
  lcd.init(); 
  Serial.begin(9600);
}

void loop(){
  char customKey = customKeypad.getKey();
  if (customKey){
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print(customKey);
    Serial.println(customKey);
  }
}
