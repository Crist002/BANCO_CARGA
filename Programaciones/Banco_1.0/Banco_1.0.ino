#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
//variable LCD
const byte columna = 0;
byte fila;
const byte columna2 = 1;
byte fila2;
String contenido;
String contenido2;
//variables Rele
const byte rele_1 = 0;
const byte rele_2 = 1;
const byte rele_3 = 2;
const byte rele_4 = 3;
const byte rele_5 = 4;
const byte rele_6 = 5;
const byte rele_7 = 6;
const byte rele_8 = 7;
const byte rele_9 = 8;
boolean est_r1 = 1;
boolean est_r2 = 1;
boolean est_r3 = 1;
boolean est_r4 = 1;
boolean est_r5 = 1;
boolean est_r6 = 1;
boolean est_r7 = 1;
boolean est_r8 = 1;
boolean est_r9 = 0;
//Variables pulsadores
const byte in_up = 9;
const byte in_down = 10;
const byte in_hab = 11;
boolean est_up;
boolean est_down;
boolean est_hab;
boolean seg_up = 0;
boolean seg_down = 0;
boolean seg_hab = 0;
//Variables de control
int pos_paso = 1;
float potencia = 0;


void setup(){
  //Configuracion LCD
  lcd.init();                  
  lcd.backlight();
  //Pines Rele
  pinMode(rele_1,OUTPUT);
  pinMode(rele_2,OUTPUT);
  pinMode(rele_3,OUTPUT);
  pinMode(rele_4,OUTPUT);
  pinMode(rele_5,OUTPUT);
  pinMode(rele_6,OUTPUT);
  pinMode(rele_7,OUTPUT);
  pinMode(rele_8,OUTPUT);
  pinMode(rele_9,OUTPUT);
  //Pines Pulsadores
  pinMode(in_up, INPUT);
  pinMode(in_down, INPUT);
  pinMode(in_hab, INPUT);
  //Configuracion Inicial
  fila = 3;
  contenido = "INICIANDO";
  Visual();
  delay(1000);
  lcd.clear();
  fila = 0;
  contenido = "PASOS: "+ String(pos_paso);
  fila2 = 0;
  contenido2 = "POT: " + String(potencia) + "KW";
  Visual();
}


void loop(){
  Lectura_pulsador();
  Logica();
  Salida();
}

void Lectura_pulsador(){      //Lee el estado de los pulsadores
  est_up = digitalRead(in_up);
  est_down = digitalRead(in_down);
  est_hab = digitalRead(in_hab);
  if((seg_up == 1)&&(est_up == 0)){   //Configuracion del pulsador UP
    seg_up = 0;
  }
  if((seg_up == 1)&&(est_up == 1)){
    est_up = 0;  
  }
  if((seg_up == 0)&&(est_up == 1)){
    est_up = 1;
    seg_up = 1;
  }
  
  if((seg_down == 1)&&(est_down == 0)){   //Configuracion del pulsador DOWN
    seg_down = 0;
  }
  if((seg_down == 1)&&(est_down == 1)){
    est_down = 0;  
  }
  if((seg_down == 0)&&(est_down == 1)){
    est_down = 1;
    seg_down = 1;
  }
  
  if((seg_hab == 1)&&(est_hab == 0)){   //COnfiguracion del pulsador HAB
    seg_hab = 0;
  }
  if((seg_hab == 1)&&(est_hab == 1)){
    est_hab = 0;  
  }
  if((seg_hab == 0)&&(est_hab == 1)){
    est_hab = 1;
    seg_hab = 1;
  }
  
}


void Logica(){    //Proceso de comparacion y decisiones
  if((est_down == 1)&&(pos_paso < 17)){
    cuenta_cero();
    Salida();
    delay(200);
    pos_paso = pos_paso +1;
    potencia = potencia + 7.5;
    Tabla();
    Menu_pasos();
  }
  if((est_up == 1)&&(pos_paso > 1)){
    cuenta_cero();
    Salida();
    delay(200);
    pos_paso = pos_paso -1;
    potencia = potencia - 7.5;
    Tabla();
    Menu_pasos();  
  }
}

void Menu_pasos(){
    fila = 0;
    contenido = "PASOS: " + String(pos_paso);
    fila2 = 0;
    contenido2 = "POT: " + String(potencia) + "KW";
    Visual();
}


void Visual(){    //Configuro lo que se visualizara en el LCD
  lcd.clear();
  lcd.setCursor(fila,columna);
  lcd.print(contenido);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}

void Tabla(){
  if(pos_paso == 1){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 1;
    est_r4 = 1;
    est_r5 = 1;
    est_r6 = 1;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0; //Inversa 
  }
  if(pos_paso == 2){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 0;
    est_r4 = 1;
    est_r5 = 1;
    est_r6 = 1;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 3){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 1;
    est_r4 = 1;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 4){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 0;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 1;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 5){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 6){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 0;
    est_r4 = 1;
    est_r5 = 1;
    est_r6 = 1;
    est_r7 = 1;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 7){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 8){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 0;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 1;
    est_r7 = 1;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 9){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 10){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 0;
    est_r4 = 1;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 11){
    est_r1 = 0;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 12){
    est_r1 = 1;
    est_r2 = 1;
    est_r3 = 0;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 1;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 13){
    est_r1 = 0;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 1;
    est_r9 = 0;
  }
  if(pos_paso == 14){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 0;
    est_r4 = 0;
    est_r5 = 1;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 15){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 16){
    est_r1 = 1;
    est_r2 = 0;
    est_r3 = 0;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 0;
    est_r9 = 1;
  }
  if(pos_paso == 17){
    est_r1 = 0;
    est_r2 = 0;
    est_r3 = 1;
    est_r4 = 0;
    est_r5 = 0;
    est_r6 = 0;
    est_r7 = 0;
    est_r8 = 0;
    est_r9 = 1;
  }
}



void Salida(){   //Habilita o desabilita las salidas
  digitalWrite(rele_1,est_r1);
  digitalWrite(rele_2,est_r2);
  digitalWrite(rele_3,est_r3);
  digitalWrite(rele_4,est_r4);
  digitalWrite(rele_5,est_r5);
  digitalWrite(rele_6,est_r6);
  digitalWrite(rele_7,est_r7);
  digitalWrite(rele_8,est_r8);
  digitalWrite(rele_9,est_r9);
}

void cuenta_cero(){
  est_r1 = 1;
  est_r2 = 1;
  est_r3 = 1;
  est_r4 = 1;
  est_r5 = 1;
  est_r6 = 1;
  est_r7 = 1;
  est_r8 = 1;
  est_r9 = 0; //Inversa 
}
