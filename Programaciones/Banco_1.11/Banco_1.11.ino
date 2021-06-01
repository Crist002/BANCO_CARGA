#include<Wire.h>
#include <SPI.h>
#include<RTClib.h>
#include <SD.h>
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


File myFile;
int UltimaPocicion = 0;
int pausa = 1000;

//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void setup()
{
  rtc.begin();    //INICIA EL RTC
  pcf8574.begin();
  for (byte u = 0; u < 8; u++) {
    pcf8574.pinMode(salidas[u], OUTPUT);
  }
  pinMode(9, OUTPUT);
  Serial.begin(9600);
  Serial.print("Iniciando SD ...");
  if (!SD.begin(10)) {
    Serial.println("No se pudo inicializar");
    return;
  }
    Serial.println("inicializacion exitosa");
    if(!SD.exists("Salidas.txt")){
    Serial.println("EXISTE Salidas.txt");
  }
}
  
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
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
    Serial.print("Cadena Leida:");
    Serial.print(cadena);
    //-----------procesamos la cadena------------
    int index = 0;
    char c = cadena[index++];
    pausa = 0;
    while (c >= '0' && c <= '9')
    {
      pausa = 10 * pausa + (c - '0');
      c = cadena[index++];
    }
    Serial.print("PAUSA=");
    Serial.print(pausa);
    Serial.print("   SALIDA|");
    for (byte i=0; i<9; i++)
    {
      if (i == 8) {
        if (cadena[index + i * 2] == '1')
        {
          digitalWrite(9, HIGH);
          Serial.print(" 1 |");
        }
        else
        {
          digitalWrite(9, LOW);
          Serial.print(" 0 |");
        }
      }
      if (i < 8) {
        if (cadena[index + i * 2] == '1')
        {
          pcf8574.digitalWrite(salidas[i], HIGH);
          Serial.print(" 1 |");
        }
        else
        {
          pcf8574.digitalWrite(salidas[i], LOW);
          Serial.print(" 0 |");
        }
      }
    }
    Serial.println();
    Serial.println();

  } else {
    Serial.println("Error al abrir el archivo");
  }
  delay(pausa);
}
