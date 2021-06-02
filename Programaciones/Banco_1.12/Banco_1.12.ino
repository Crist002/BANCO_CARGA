#include<Wire.h>
#include <SPI.h>
#include<RTClib.h>
#include <SD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include "Arduino.h"
#include "PCF8574.h"
//Configuracion del PCF8574
const byte salidas[8] = {P3, P7, P6, P5, P4, P2, P1, P0};
PCF8574 pcf8574(0x25);  //Direccion de I2C
//CONFIGURACION DEL RTC
RTC_DS1307 rtc;
DateTime HoraFecha;
byte segundo, minuto, hora, dia, mes;
int anio;
//variable LCD
const byte columna1 = 0;
byte fila1;
const byte columna2 = 1;
byte fila2;
String contenido1;
String contenido2;

File myFile;
int UltimaPocicion = 0;
int pausa = 1000;

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void setup(){
  lcd.init();                  
  lcd.backlight();
  rtc.begin();    //INICIA EL RTC
  pcf8574.begin();
  for (byte u = 0; u < 8; u++) {
    pcf8574.pinMode(salidas[u], OUTPUT);
  }
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  contenido1= "Iniciando SD ...";
  Visual();
  delay(1000);
  lcd.clear();
  while(!SD.begin(10)) {
    fila1 = 0;
    contenido1= "Inserte SD";
    Visual2();
  }
  contenido1="SD Iniciado";
  Visual();
  delay(1000);
  lcd.clear();
  while(!SD.exists("Salidas.txt")){
    fila1 = 0;
    contenido1 = "Falta archivo:";
    fila2 = 0;
    contenido2 = "Salidas.txt";
    Visual2();
  }
  if(!SD.exists("DATOS.csv")){
      myFile = SD.open("DATOS.csv", FILE_WRITE);
      if (myFile) {
        myFile.println("FECHA,HORA");
        myFile.close();
      } else {
        contenido1="Error creando:";
        contenido2="DATOS.csv";
        Visual();
        delay(1000);
      }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void loop(){
  Lectura_Tiempo();
  Escritura_SD();
  myFile = SD.open("Salidas.txt", FILE_READ);//abrimos  el archivo
  int totalBytes = myFile.size();
  String cadena = "";

  if (myFile) {
    if (UltimaPocicion >= totalBytes)   UltimaPocicion = 0;
    myFile.seek(UltimaPocicion);

    //--Leemos una línea de la hoja de texto--------------
    while (myFile.available()) {
      char caracter = myFile.read();
      cadena = cadena + caracter;
      UltimaPocicion = myFile.position();
      if (caracter == 10) //ASCII de nueva de línea
      {
        break;
      }
    }
    //---------------------------------------------------
    myFile.close(); //cerramos el archivo
    //Serial.print("Cadena Leida:");
    //Serial.print(cadena);
    //-----------procesamos la cadena------------
    int index = 0;
    char c = cadena[index++];
    pausa = 0;
    while (c >= '0' && c <= '9')
    {
      pausa = 10 * pausa + (c - '0');
      c = cadena[index++];
    }
    fila1=0;
    fila2=0;
    contenido1 = "PAUSA=" + String(pausa);
    contenido2 = "SALIDA=";
    Visual();
    for (byte i=0; i<9; i++)
    {
      fila2= 7+i;
      if (i == 8) {
        if (cadena[index + i * 2] == '1')
        {
          digitalWrite(9, HIGH);
          contenido2 ="1";
        }
        else
        {
          digitalWrite(9, LOW);
          contenido2 ="0";
        }
      }
      if (i < 8) {
        if (cadena[index + i * 2] == '1')
        {
          pcf8574.digitalWrite(salidas[i], HIGH);
          contenido2 ="1";
        }
        else
        {
          pcf8574.digitalWrite(salidas[i], LOW);
          contenido2 ="0";
        }
      }
      Visual2();
    }

  } else {    //Si no se lee el archivo Salidas.txt  :
    fila1=0;
    fila2=0;
    contenido1= "Error al abrir:";
    contenido2= "Salidas.txt";
    Visual2();
  }
  delay(pausa);
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

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
  myFile.close();
}

void Visual(){    //Configuro lo que se visualizara en el LCD
  lcd.clear();
  lcd.setCursor(fila1,columna1);
  lcd.print(contenido1);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}

void Visual2(){    //Configuro lo que se visualizara en el LCD
  lcd.setCursor(fila1,columna1);
  lcd.print(contenido1);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}
