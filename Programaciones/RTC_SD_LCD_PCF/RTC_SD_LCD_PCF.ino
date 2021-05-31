#include<Wire.h>
#include <SPI.h>
#include<RTClib.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include "Arduino.h"
#include "PCF8574.h"
//Configuracion del PCF8574
byte salidas[8]={P3,P7,P6,P5,P4,P2,P1,P0};
byte opcion[17][9]={                      //PASO[CANTIDAD DE PASOS][ACTIVAR/DESACTIVAR SALIDAS]
{0 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0},    //PASO 1
{0 , 1 , 0 , 1 , 1 , 1 , 1 , 1 , 0},    //PASO 2
{0 , 1 , 1 , 1 , 1 , 0 , 1 , 1 , 0},    //PASO 3
{0 , 1 , 0 , 0 , 1 , 1 , 1 , 1 , 0},    //PASO 4
{0 , 1 , 1 , 0 , 1 , 0 , 1 , 1 , 0},    //PASO 5
{0 , 1 , 0 , 1 , 1 , 1 , 1 , 0 , 1},    //PASO 6
{0 , 0 , 1 , 0 , 1 , 0 , 1 , 1 , 0},    //PASO 7
{0 , 1 , 0 , 0 , 1 , 1 , 1 , 0 , 1},    //PASO 8
{0 , 0 , 1 , 0 , 1 , 0 , 0 , 1 , 0},    //PASO 9
{0 , 0 , 0 , 1 , 1 , 0 , 1 , 0 , 1},    //PASO 10
{1 , 0 , 1 , 0 , 0 , 0 , 1 , 1 , 0},    //PASO 11
{0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 1},    //PASO 12
{1 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0},    //PASO 13
{0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 1},    //PASO 14
{0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 15
{0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 16
{1 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 17
};
int paso = 0;
PCF8574 pcf8574(0x25);  //Direccion de I2C
//variable LCD
const byte columna = 0;
byte fila;
const byte columna2 = 1;
byte fila2;
String contenido;
String contenido2;
//CONFIGURACION DEL RTC
RTC_DS1307 rtc;
DateTime HoraFecha;
byte segundo,minuto,hora,dia,mes;
int anio;
//CONFIGURACION DEL SD
File myFile;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  lcd.init();                  
  lcd.backlight();
  rtc.begin();    //INICIA EL RTC
  pcf8574.begin();
  for(byte u=0; u<8; u++){
    pcf8574.pinMode(salidas[u], OUTPUT);
  }
  pinMode(9, OUTPUT);
  contenido = "Iniciando SD ...";
  Visual();
  delay(1000);
  while(!SD.begin(10)) {      //INICIA EL SD A TRAVEZ DEL PIN 10 (CS), NO CAMBIAR SI SE USA LA SHIELD DATALOGGER
    contenido = "No se pudo iniciar SD";
    Visual();
    delay(1000);
  }
  contenido = "SD INICIADA";
  Visual();
  delay(1000);
  if(!SD.exists("DATOS.csv")){
      myFile = SD.open("DATOS.csv", FILE_WRITE);
      if (myFile) {
        myFile.println("FECHA,HORA,PISO");
        myFile.close();
      } else {
        contenido="Error creando el archivo datalog.csv";
        Visual();
        delay(1000);
      }
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() {
  Lectura_Tiempo();
  PCF_salidas_rele();
  Escritura_SD();
  delay(2000);
  paso++;
  if(paso>16){
    paso = 0;
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Lectura_Tiempo(){
  HoraFecha  = rtc.now();
  segundo    = HoraFecha.second();
  minuto     = HoraFecha.minute();
  hora       = HoraFecha.hour();
  dia        = HoraFecha.day();
  mes        = HoraFecha.month();
  anio       = HoraFecha.year();
}

void Escritura_SD(){
  myFile = SD.open("DATOS.csv", FILE_WRITE);
  myFile.print(String(dia) + "/" + mes + "/" + anio);
  myFile.print(",");
  myFile.print(String(hora) + ":" + minuto + ":" + segundo);
  myFile.print(",");
  myFile.println(paso + 1);
  myFile.close();
  contenido = "GUARDADO ";
  contenido2= "PASO= " + String(paso+1);
  Visual();
}

void Visual(){    //Configuro lo que se visualizara en el LCD
  lcd.clear();
  lcd.setCursor(fila,columna);
  lcd.print(contenido);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}

void PCF_salidas_rele(){
  for(byte u =0; u<8; u++){
    pcf8574.digitalWrite(salidas[u],opcion[paso][u]);
    digitalWrite(9,opcion[paso][8]);
  }
}
