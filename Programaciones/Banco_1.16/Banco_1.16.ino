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
uint8_t UltPos = 0;
int pausa = 0;

//Variables Modbus
byte datT[12];
byte datC[12];
int Tension[3];
int Corriente[3];

boolean leer = true;
byte set_segundo;
byte set_minuto;
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
  Visual_Modbus();
  Escritura_SD();
  if(leer == true){
    LecturaSD();
    uint8_t segs = HoraFecha.second();
    uint8_t mins = HoraFecha.minute();
    set_segundo = segs + pausa;
    set_minuto = mins;
    if(set_segundo >= 60){
      set_segundo = set_segundo - 60;
      set_minuto = set_minuto + 1;
    }
    leer = false;
  }
  if((HoraFecha.second()== set_segundo)&&(HoraFecha.minute()== set_minuto)){
    leer = true;
  }
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
  byte a,b,c,d;
  byte i=0;
  byte salto=0;
  boolean mod = 0;   // 0--> Tension  ; 1--> Corriente  ;
  unsigned long m;
  unsigned long VALOR;
  unsigned long c1;
  byte e;
  int ex;
  byte d2;
  byte d3;
  int c2;
  byte c3;
  for(byte z=0 ; z<6 ; z++){
    lcd.clear();
    if(mod == 0){
      a=datT[salto];
      b=datT[1+salto];
      c=datT[2+salto];
      d=datT[3+salto];
    }else if(mod == 1){
      a=datC[salto];
      b=datC[1+salto];
      c=datC[2+salto];
      d=datC[3+salto];
    }
    e = ((a*0x100)+b)>>7;
    ex = e-126;
    d2 = b;
    c1 =d2*65536;
    d3 = c;
    c2 = d3*256;
    c3 = d;
    if(((b>>7)&(0x1))== 1){
      m= c1+c2+c3;
    }else{
      m= 8388608+c1+c2+c3;
    }
    VALOR = pow(2,ex)*m;
    VALOR = VALOR*0.0000059604;
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

void Visual_Modbus(){
  lcd.clear();
  for(byte v1=0; v1<3 ; v1++){
    lcd.setCursor(5*v1,0);
    lcd.print(Tension[v1]);
  }
  for(byte v1=0; v1<3 ; v1++){
    lcd.setCursor(5*v1,1);
    lcd.print(Corriente[v1]);
  }
}

void LecturaSD(){
  myFile = SD.open("Salidas.txt", FILE_READ);//abrimos  el archivo
  uint16_t totalBytes = myFile.size();
  String cadena = "";

  if (myFile) {
    if (UltPos >= totalBytes){UltPos = 0;}
    myFile.seek(UltPos);
    //--Leemos una línea de la hoja de texto--------------
    while (myFile.available()) {
      char caracter = myFile.read();
      cadena = cadena + caracter;
      UltPos = myFile.position();
      if (caracter == 10) //ASCII de nueva de línea
      {
        break;
      }
    }
    //---------------------------------------------------
    myFile.close(); //cerramos el archivo
    //Serial.print("Cadena Leida:");
    //-----------procesamos la cadena------------
    int index = 0;
    char c = cadena[index++];
    pausa = 0;
    
    while (c >= '0' && c <= '9')
    {
      pausa = 10 * pausa + (c - '0');
      c = cadena[index++];
    }
    
    for (int i=0; i<9; i++){
      if (i < 8) {
        if (cadena[index + i * 2] == '1') { pcf8574.digitalWrite(salidas[i], HIGH); }
        else{ pcf8574.digitalWrite(salidas[i], LOW); }
      }
    }

  } else {    //Si no se lee el archivo Salidas.txt  :
    /*
    lcd.setCursor(0,0);
    lcd.print("ERROR EN LEER: ");
    lcd.setCursor(0,1);
    lcd.print("Salidas.txt    ");
    */
  }
  pausa = pausa/1000;
}
