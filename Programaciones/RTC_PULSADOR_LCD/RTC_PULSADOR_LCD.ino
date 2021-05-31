#include<Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include<RTClib.h>
//CONFIGURACION DEL RTC
RTC_DS1307 rtc;
DateTime HoraFecha;
byte segundo,minuto,hora,dia,mes;
int anio;
byte segundero_up = 0;
byte selector=0;

void setup() {
  pinMode(6, INPUT);
  lcd.init();                  
  lcd.backlight();
  rtc.begin();    //INICIA EL RTC
  lcd.setCursor(0,0);
  lcd.print("SELECTOR= ");
}

void loop() {
  Lectura_Tiempo();
  pulsadores();
  lcd.setCursor(10,0);
  lcd.print(selector);
}

void pulsadores(){
  if(digitalRead(6)== HIGH){
    segundero_up = segundo;
    byte f= 1;
    while(f == 1){
      Lectura_Tiempo();
      if((digitalRead(6)== HIGH)&&((segundo-segundero_up)> 1)){
        selector = selector +5;
        break;
      }
      if((digitalRead(6)== LOW)&&((segundo-segundero_up)< 1)){
        selector++;
        break;
      }
      
    }
  }
}

void Lectura_Tiempo(){
  HoraFecha  = rtc.now();
  segundo    = HoraFecha.second();
  minuto     = HoraFecha.minute();
  hora       = HoraFecha.hour();
  dia        = HoraFecha.day();
  mes        = HoraFecha.month();
  anio       = HoraFecha.year();
}
