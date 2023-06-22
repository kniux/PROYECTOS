#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>    // importa libreria, debe indtalarla previamente

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
Servo myservo;  // create servo object to control a servo
Adafruit_NeoPixel LED = Adafruit_NeoPixel(1,2, NEO_GRB + NEO_KHZ800);  // 1 LED y Pin 2 del Arduino es salida de datos

int pos = 0;    // variable to store the servo position
int retardo = 500;
void setup() {
  myservo.attach(12);  // attaches the servo on pin 9 to the servo object
  lcd.init();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Hello, world!");
  lcd.setCursor(2,1);
  lcd.print("Ywrobot Arduino!");
  lcd.setCursor(0,2);
  lcd.print("Arduino LCM IIC 2004");
  lcd.setCursor(2,3);
  lcd.print("Power By Ec-yuan!");
  LED.begin();       // inicializacion de la tira
  LED.setBrightness(255); // Brillo de los LEDs
  LED.show(); 
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  LED.setPixelColor(0, 255, 255, 255); delay(retardo);LED.show();// BLANCO  
  LED.setPixelColor(0, 255, 0, 0);     delay(retardo);LED.show();// ROJO
  LED.setPixelColor(0, 0, 255, 0);     delay(retardo);LED.show();// VERDE
  LED.setPixelColor(0, 0, 0, 255);     delay(retardo);LED.show(); // AZUL
  LED.setPixelColor(0, 255, 100, 31);  delay(retardo);LED.show();// AMARILLO
  LED.setPixelColor(0, 0, 255, 60);    delay(retardo);LED.show();// CYAN
  LED.setPixelColor(0, 180, 0, 180);   delay(retardo);LED.show();// VIOLETA
  LED.setPixelColor(0, 0, 0, 0);   delay(retardo);LED.show();// off
}
