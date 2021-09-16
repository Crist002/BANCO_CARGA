#include <EEPROM.h>

void setup() {
  EEPROM.write(0,0b11111111); //PASO 1
  EEPROM.write(1,0b11111111);
  EEPROM.write(2,0b11111011); //PASO 2
  EEPROM.write(3,0b11111111);
  EEPROM.write(4,0b11011111); //PASO 3
  EEPROM.write(5,0b11111111);
  EEPROM.write(6,0b11110011); //PASO 4
  EEPROM.write(7,0b11111111);
  EEPROM.write(8,0b11010111); //PASO 5
  EEPROM.write(9,0b11111111);
  EEPROM.write(10,0b01111011);  //PASO 6
  EEPROM.write(11,0b11111110);
  EEPROM.write(12,0b11010101);  //PASO 7
  EEPROM.write(13,0b11111111);
  EEPROM.write(14,0b01110011);  //PASO 8
  EEPROM.write(15,0b11111110);
  EEPROM.write(16,0b10010101);  //PASO 9
  EEPROM.write(17,0b11111111);
  EEPROM.write(18,0b01011001);  //PASO 10
  EEPROM.write(19,0b11111110);
  EEPROM.write(20,0b11000100);  //PASO 11
  EEPROM.write(21,0b11111111);
  EEPROM.write(22,0b01000011);  //PASO 12
  EEPROM.write(23,0b11111110);
  EEPROM.write(24,0b10000100);  //PASO 13
  EEPROM.write(25,0b11111111);
  EEPROM.write(26,0b00010001);  //PASO 14
  EEPROM.write(27,0b11111110);
  EEPROM.write(28,0b00000101);  //PASO 15
  EEPROM.write(29,0b11111110);
  EEPROM.write(30,0b00000001);  //PASO 16
  EEPROM.write(31,0b11111110);
  EEPROM.write(32,0b00000100);  //PASO 17
  EEPROM.write(33,0b11111110);
}

void loop() {
  // put your main code here, to run repeatedly:

}