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
byte customChar[] = {
  B00000,
  B00100,
  B00010,
  B11111,
  B00010,
  B00100,
  B00000,
  B00000
};
byte ambos[] = {
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};

byte arriba[] = {
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte abajo[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};

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

//Varibales pulsadores
boolean est_up , est_down , est_left , est_right ;
boolean seg_up = 0;
boolean seg_down = 0;
boolean seg_left = 0;
boolean seg_right = 0;

//Variables menu
boolean menu_conf = true;
boolean menu_auto = false;
boolean menu_manu = false;
byte pag_menu = 0;
byte g_menu = 0;
byte m_cursor = 0;
boolean cursor_fila = 0;
byte paso = 1;
byte pag_anterior = 0;

//Variables de salida
byte tiempo = 1;
byte P_min = 1;
byte P_max = 1;
byte P_pas = 1;
byte P_modo = 1;
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

void setup(){
  pinMode(4, OUTPUT);
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  Serial2.begin(9600);
  Serial.begin(9600);
  lcd.init();                  
  lcd.backlight();
  lcd.createChar(0, customChar);
  lcd.createChar(1, ambos);
  lcd.createChar(2, arriba);
  lcd.createChar(3, abajo);
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
  Pulsadores();
  if(menu_conf == true){
    Ajuste(); 
  }
  if(menu_auto == true){
    Automatico();
  }
  if(menu_manu == true){
    Manual();
  }
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void Ajuste(){
  Lcd_Menu();
  Lcd_Cursor();
}

void Lcd_Cursor(){
  //--------- EST_DOWN ---------
  if(est_down == 1){
    if(g_menu == 5){
      pag_menu = 5;
      paso--;
      if(paso<1){paso = 1;}  
    }
    if(g_menu == 6){
      pag_menu = 6;
      paso--;
      if(paso<1){paso = 1;}  
    }
    if(cursor_fila==1){
      switch(g_menu){
        case 1: pag_menu = 2; cursor_fila = 0; break;
        case 2: pag_menu = 3; cursor_fila = 0; break;
        case 4: pag_menu = 4; cursor_fila = 0; paso = paso+2; 
                if((paso>16)&&((pag_anterior == 1)||(pag_anterior == 2))){paso = 15; cursor_fila = 1;}
                if((paso >4)&&(pag_anterior == 3)){paso = 3; cursor_fila = 1;}
        break;
      }
    }
    else{ cursor_fila=1; }
    //--------- EST_UP ---------
  }else if(est_up == 1){
    if(g_menu == 5){
      pag_menu = 5;
      paso++;
      if(paso>16){paso = 16;}
    }
    if(g_menu == 6){
      pag_menu = 6;
      paso++;
      if(paso>3){paso = 3;}
    }
    if(cursor_fila == 0){
      switch(g_menu){
        case 2: pag_menu = 1; cursor_fila = 1; break;
        case 3: pag_menu = 2; cursor_fila = 1; break;
        case 4: pag_menu = 4; cursor_fila = 1; paso = paso-2; 
              if((paso<1)||(paso > 250)){paso = 1; cursor_fila = 0;}
        break;
      }
    }
    else{cursor_fila=0;}
  }
  //--------- EST_RIGHT ---------
  if(est_right == 1){
    switch(g_menu){
      case 0:
            switch(cursor_fila){
              case 0: pag_menu = 1; break; 
              case 1: /*FALTA PONER PARA AUTOMATICO*/ break;
            }
      break;
      case 1:
            switch(cursor_fila){
              case 0: pag_menu = 4; pag_anterior = 1;if(P_min % 2 == 0){paso = P_min -1; cursor_fila = 1;}else{paso = P_min; cursor_fila = 0;} break;  /*INSTRUCCION POTENCIA MIN*/
              case 1: pag_menu = 4; cursor_fila = 0; pag_anterior = 2; if(P_max % 2 == 0){paso = P_max -1; cursor_fila = 1;}else{paso = P_max; cursor_fila = 0;} break; /*INSTRUCCION POTENCIA MAX*/
            }
      break;
      case 2:
            switch(cursor_fila){
              case 0: pag_menu = 4; pag_anterior = 3; if(P_pas % 2 == 0){paso = P_pas -1; cursor_fila = 1;}else{paso = P_pas; cursor_fila = 0;} break; /*INSTRUCCION POTENCIA PASO*/
              case 1: pag_menu = 5; if(tiempo<5){paso = tiempo;}else if(tiempo >= 5 ){paso = (tiempo*0.2)+4;} break; /*INSTRUCCION TIEMPO PASO*/
            }
      break;
      case 3:
            switch(cursor_fila){
              case 0: pag_menu = 6; paso = P_modo; break; /*INSTRUCCION MODO ARRANQUE*/
              case 1: pag_menu = 7; break; /*INSTRUCCION INICIAR*/
            }
      break;
      case 4:
          switch(pag_anterior){
            case 1: if(cursor_fila == 0){P_min = paso;}else{P_min = paso+1;} pag_menu = 1; cursor_fila = 0;
            break;
            case 2: if(cursor_fila == 0){P_max = paso;}else{P_max = paso+1;} pag_menu = 1; cursor_fila = 1;
            break;
            case 3: if(cursor_fila == 0){P_pas = paso;}else{P_pas = paso+1;} pag_menu = 2; cursor_fila = 0;
            break;
          }
      break;
      case 5: pag_menu = 2; cursor_fila = 1;
      break;
      case 6: P_modo = paso; pag_menu = 3; cursor_fila = 0;
      break;
    }
    //--------- EST_LEFT ---------
  }else if(est_left == 1){
    switch(g_menu){
      case 1: pag_menu = 0; cursor_fila = 0; break;
      case 2: pag_menu = 0; cursor_fila = 0; break;
      case 3: pag_menu = 0; cursor_fila = 0; break;
      case 4: switch(pag_anterior){
                case 1: pag_menu = 1; cursor_fila = 0;break;
                case 2: pag_menu = 1; cursor_fila = 1;break;
                case 3: pag_menu = 2; cursor_fila = 0;break;
              }
      break;
      case 5: pag_menu = 2; cursor_fila = 1; break;
      case 6: pag_menu = 3; cursor_fila = 0; break;
      case 7: pag_menu = 3; cursor_fila = 1; break;
    }
  }
  //--------- CURSOR FILA ---------
  if(g_menu < 5){ 
    lcd.setCursor(0,!cursor_fila);
    lcd.print(" ");
    lcd.setCursor(0,cursor_fila);
    lcd.write(0);
  } else if(g_menu == 5){
    lcd.setCursor(0,1);
    if(paso == 1){lcd.write(2);}
    else if(paso == 16){lcd.write(3);}
    else {lcd.write(1);}
  }
  
  /*
  if(paso>=10){
    lcd.setCursor(14,0);
    lcd.print(paso);
  }else if(paso<10){
    lcd.setCursor(14,0);
    lcd.print(paso);lcd.print(" ");
  }
  */
  
}


void Lcd_Menu(){
  if(pag_menu == 0){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("AUTOMATICO");
      lcd.setCursor(1,1);
      lcd.print("MANUAL");
  }else if(pag_menu == 1){
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("POTENCIA MIN");
      lcd.setCursor(1,1);
      lcd.print("POTENCIA MAX");
      lcd.setCursor(15,1);
      lcd.write(3);
  }else if(pag_menu == 2){
      lcd.clear();
      lcd.setCursor(15,0);
      lcd.write(2);
      lcd.setCursor(1,0);
      lcd.print("POTENCIA PASO");
      lcd.setCursor(1,1);
      lcd.print("TIEMPO PASO");
      lcd.setCursor(15,1);
      lcd.write(3);
  }else if(pag_menu == 3){
      lcd.clear();
      lcd.setCursor(15,0);
      lcd.write(2);
      lcd.setCursor(1,0);
      lcd.print("MODO ARRANQUE");
      lcd.setCursor(1,1);
      lcd.print("INICIAR");
  }else if(pag_menu == 4){
      int valor1 = 75*paso;
      int valor2 = 75*(paso+1);
      byte ent_valor1 = valor1*0.1;
      byte ent_valor2 = valor2*0.1;
      byte un_valor1 = valor1 - (ent_valor1 * 10);
      byte un_valor2 = valor2 - (ent_valor2 * 10);
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print(ent_valor1);lcd.print(".");lcd.print(un_valor1);
      lcd.setCursor(1,1);
      lcd.print(ent_valor2);lcd.print(".");lcd.print(un_valor2);
  }else if(pag_menu == 5){
      if(paso<5){
        tiempo = paso;
      }else if(paso >= 5 ){
        tiempo = 5*(paso-4);
      }
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("TIEMPO POR PASO");
      lcd.setCursor(1,1);
      lcd.print(tiempo);lcd.print(" min");
  }else if(pag_menu == 6){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MODO DE ARRANQUE");
      lcd.setCursor(0,1);
      switch(paso){
        case 1: lcd.write(2);lcd.print("MIN-MAX");
        break;
        case 2: lcd.write(1);lcd.print("MAX-MIN");
        break;
        case 3: lcd.write(3);lcd.print("MIN-MAX-MIN");
        break;  
      }
  }else if(pag_menu == 7){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(P_min);
    lcd.setCursor(3,0);
    lcd.print(P_max);
    lcd.setCursor(6,0);
    lcd.print(P_pas);
    lcd.setCursor(0,1);
    lcd.print(tiempo);
    lcd.setCursor(3,1);
    lcd.print(P_modo);
  }

  if(pag_menu != 254){
    g_menu = pag_menu;
  }
  pag_menu = 254;
}

void Automatico(){
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

void Manual(){
  
}


void Pulsadores(){
  est_up = digitalRead(5);
  est_down = digitalRead(6);
  est_left = digitalRead(7);
  est_right = digitalRead(8);
  //-------Configuracion de PULSADOR UP-------
  if((seg_up == 1)&&(est_up == 0)){   
    seg_up = 0;
  }
  if((seg_up == 1)&&(est_up == 1)){
    est_up = 0;  
  }
  if((seg_up == 0)&&(est_up == 1)){
    est_up = 1;
    seg_up = 1;
  }
  //-------Configuracion de PULSADOR DOWN-------
  if((seg_down == 1)&&(est_down == 0)){   
    seg_down = 0;
  }
  if((seg_down == 1)&&(est_down == 1)){
    est_down = 0;  
  }
  if((seg_down == 0)&&(est_down == 1)){
    est_down = 1;
    seg_down = 1;
  }
  //-------Configuracion de PULSADOR LEFT-------
  if((seg_left == 1)&&(est_left == 0)){   
    seg_left = 0;
  }
  if((seg_left == 1)&&(est_left == 1)){
    est_left = 0;  
  }
  if((seg_left == 0)&&(est_left == 1)){
    est_left = 1;
    seg_left = 1;
  }
  //-------Configuracion de PULSADOR RIGHT-------
  if((seg_right == 1)&&(est_right == 0)){   
    seg_right = 0;
  }
  if((seg_right == 1)&&(est_right == 1)){
    est_right = 0;  
  }
  if((seg_right == 0)&&(est_right == 1)){
    est_right = 1;
    seg_right = 1;
  }
}


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
  for(byte z=0 ; z<3 ; z++){
    myFile.print(Tension[z]);
    myFile.print(",");
  }
  for(byte z=0 ; z<3 ; z++){
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
  for(byte f=1 ; f<3 ; f++){
    digitalWrite(4, HIGH);
    if(f==1){
      Serial2.write(InfoTension, 8);
    }else if(f==2){
      Serial2.write(InfoCorriente, 8);
    }
    Serial2.flush();
    digitalWrite(4,LOW);
    delay(100);
    byte g=0;
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
    byte index = 0;
    char c = cadena[index++];
    pausa = 0;
    
    while (c >= '0' && c <= '9')
    {
      pausa = 10 * pausa + (c - '0');
      c = cadena[index++];
    }
    
    for (byte i=0; i<9; i++){
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
