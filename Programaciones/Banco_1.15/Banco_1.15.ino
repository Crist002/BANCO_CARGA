#include<Wire.h>
#include <SPI.h>
#include "RTClib.h"
#include <SD.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
#include "Arduino.h"
#include "PCF8574.h"
//Configuracion del PCF8574
const byte salidas[8] = {P3, P7, P6, P5, P4, P2, P1, P0};
PCF8574 pcf8574(0x25);  //Direccion de I2C
//CONFIGURACION DEL RTC
RTC_DS3231 rtc;
DateTime HoraFecha;
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

//Variables Modbus
int datT[12];
int datC[12];
float Tension[3];
float Corriente[3];
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void setup(){
  pinMode(4, OUTPUT);
  Serial2.begin(9600);
  Serial.begin(9600);
  lcd.init();                  
  lcd.backlight();
  rtc.begin();    //INICIA EL RTC
  pcf8574.begin();
  for (byte u = 0; u < 8; u++) {
    pcf8574.pinMode(salidas[u], OUTPUT);
  }
  pinMode(9, OUTPUT);
  contenido1= "Iniciando SD ...";
  Visual();
  delay(1000);
  lcd.clear();
  while(!SD.begin(53)) {
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
        myFile.println("FECHA,HORA,TENSION A,TENSION B,TENSION C,CORRIENTE A,CORRIENTE B,CORRIENTE C");
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
  Modbus();
  Calculo_modbus();
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
}

void Escritura_SD(){
  myFile = SD.open("DATOS.csv", FILE_WRITE);
  myFile.print(HoraFecha.day(),DEC);
  myFile.print("/");
  myFile.print(HoraFecha.month(),DEC);
  myFile.print("/");
  myFile.print(HoraFecha.year(),DEC);
  myFile.print(",");
  myFile.print(HoraFecha.hour(),DEC);
  myFile.print(":");
  myFile.print(HoraFecha.minute(),DEC);
  myFile.print(":");
  myFile.print(HoraFecha.second(),DEC);
  myFile.print(",");
  for(int z=0 ; z<3 ; z++){
    myFile.print(Tension[z]);
    myFile.print(",");
  }
  for(int z=0 ; z<3 ; z++){
    if(z==2){
      myFile.println(Corriente[z]);
    }else{
      myFile.print(Corriente[z]);
      myFile.print(",");
    }
  }
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

void Modbus(){
  byte InfoTension[]={2,3,0,9,0,6,21,249};  //Tension Canal A,B,C
  byte InfoCorriente[]={2,3,0,15,0,6,245,248};  //Corriente Canal A,B,C
  for(int f=1 ; f<3 ; f++){
    digitalWrite(4, HIGH);
    if(f==1){
      Serial2.write(InfoTension, 8);
    }else if(f==2){
      Serial2.write(InfoCorriente, 8);
    }
    Serial2.flush();
    digitalWrite(4,LOW);
    delay(100);
    int g=0;
    while(Serial2.available()>0){
      int datos = Serial2.read();
      g++;
      if((g>3)&&(g<16)){
        switch(f){
          case 1: datT[g-4]=datos; break;
          case 2: datC[g-4]=datos; break;
        }
      }
    }
  }
}

void Calculo_modbus(){
  int a,b,c,d;
  int i=0;
  int salto=0;
  int mod = 0;   // 0--> Tension  ; 1--> Corriente  ;
  for(int z=0 ; z<6 ; z++){
    if(mod == 0){
      a=datT[salto];
      b=datT[1+salto];
      c=datT[2+salto];
      d=datT[3+salto];
    }else if(mod == 1){
      for(int j=0 ; j<1 ; j++){
      a=datC[salto];
      b=datC[1+salto];
      c=datC[2+salto];
      d=datC[3+salto];
      }
    }
    unsigned long m;
    int e = ((a*0x100)+b)>>7;
    int ex = e-126;
    float d2 = b;
    float c1 =d2*65536;
    float d3 = c;
    float c2 = d3*256;
    float c3 = d;
    if(((b>>7)&(0x1))== 1){
      m= c1+c2+c3;
    }else{
      m= 8388608+c1+c2+c3;
    }
    float VALOR = pow(2,ex)*m/(256*65536);
    if(mod == 0){
      Tension[i]= VALOR;
    }else if(mod == 1){
      Corriente[i]=VALOR;  
    }
    i++;
    salto = salto + 4;
    if(z==3){
      mod = 1;
      salto = 0;
      i=0;
    }
  }
  
}
