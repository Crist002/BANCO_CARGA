/*
 Blink led on PIN0
 by Mischianti Renzo <http://www.mischianti.org>

 https://www.mischianti.org/2019/01/02/pcf8574-i2c-digital-i-o-expander-fast-easy-usage/
*/

#include "Arduino.h"
#include "PCF8574.h"
int salidas[8]={P0,P1,P2,P3,P4,P5,P6,P7};
byte opcion1[8]= {0,0,0,1,0,0,0,0};
byte opcion2[8]= {1,1,1,0,1,1,1,1};
// Set i2c address
PCF8574 pcf8574(0x25);

void setup()
{
  pcf8574.begin();
  for(byte u=0; u<8; u++){
    pcf8574.pinMode(salidas[u], OUTPUT);
  }
}

void loop()
{
  for(byte u =0; u<8; u++){
    pcf8574.digitalWrite(salidas[u],opcion1[u]);
  }
	delay(1000);
  for(byte u =0; u<8; u++){
    pcf8574.digitalWrite(salidas[u], opcion2[u]);
  }
	delay(1000);
}
