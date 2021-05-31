#include<Wire.h>
#include <SPI.h>
#include<RTClib.h>
#include <SD.h>
//CONFIGURACION DEL RTC
RTC_DS1307 rtc;
DateTime HoraFecha;
int segundo,minuto,hora,dia,mes;
int anio;
//CONFIGURACION DEL SD
File myFile;


void setup() {
  Serial.begin(9600);
  rtc.begin();    //INICIA EL RTC
  Serial.print("Iniciando SD ...");
  if (!SD.begin(10)) {      //INICIA EL SD A TRAVEZ DEL PIN 10 (CS), NO CAMBIAR SI SE USA LA SHIELD DATALOGGER
    Serial.println("No se pudo iniciar SD");
    return;
  }
  Serial.println("SD INICIADA");
  if(!SD.exists("Tiempo.csv")){
      myFile = SD.open("Tiempo.csv", FILE_WRITE);
      if (myFile) {
        myFile.println("FECHA,HORA");
        myFile.close();
      } else {
        Serial.println("Error creando el archivo datalog.csv");
      }
  }
}

void loop() {
  Lectura_Tiempo();
  Escritura_SD();
  Serial.println("GUARDADO");
  delay(2000);
}


void Lectura_Tiempo(){
  HoraFecha = rtc.now();
  segundo    = HoraFecha.second();
  minuto     = HoraFecha.minute();
  hora       = HoraFecha.hour();
  dia        = HoraFecha.day();
  mes        = HoraFecha.month();
  anio       = HoraFecha.year();
}

void Escritura_SD(){
  myFile = SD.open("Tiempo.csv", FILE_WRITE);
  myFile.print(String(dia) + "/" + mes + "/" + anio);
  myFile.print(",");
  myFile.println(String(hora) + ":" + minuto + ":" + segundo);
  myFile.close();
}
