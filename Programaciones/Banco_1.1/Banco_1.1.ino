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

//Variables pulsadores
const byte in_up = 9;         //PULSADOR ARRIBA
const byte in_down = 10;      //PULSADOR ABAJO
const byte in_hab = 11;       //INTERRUPTOR HABILITACION
boolean est_up;               //GUARDA EL ESTADO DE PULSADOR ARRIBA
boolean est_down;             //GUARDA EL ESTADO DE PULSADOR ABAJO
boolean est_hab;              //GUARDA EL ESTADO DE INT HABILITACION
boolean seg_up = 0;
boolean seg_down = 0;
boolean seg_hab = 0;
//Variables de control
int pos_paso = 1;     //POSICION DEL PASO ACTUAL
float potencia = 0;
//DEFINICION DE PASOS     
//*LAS PRIMERAS 8 SALIDAS ESTAN INVERTIDAS EN SU LOGICA POR EL MODULO RELE QUE TRABAJA A LA INVERSA *
//*LA ULTIMA SALIDA TIENE SU LOGICA NORMAL*
byte Paso_1[]=  {1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0};
byte Paso_2[]=  {1 , 1 , 0 , 1 , 1 , 1 , 1 , 1 , 0};
byte Paso_3[]=  {1 , 1 , 1 , 1 , 1 , 0 , 1 , 1 , 0};
byte Paso_4[]=  {1 , 1 , 0 , 0 , 1 , 1 , 1 , 1 , 0};
byte Paso_5[]=  {1 , 1 , 1 , 0 , 1 , 0 , 1 , 1 , 0};
byte Paso_6[]=  {1 , 1 , 0 , 1 , 1 , 1 , 1 , 0 , 1};
byte Paso_7[]=  {1 , 0 , 1 , 0 , 1 , 0 , 1 , 1 , 0};
byte Paso_8[]=  {1 , 1 , 0 , 0 , 1 , 1 , 1 , 0 , 1};
byte Paso_9[]=  {1 , 0 , 1 , 0 , 1 , 0 , 0 , 1 , 0};
byte Paso_10[]= {1 , 0 , 0 , 1 , 1 , 0 , 1 , 0 , 1};
byte Paso_11[]= {0 , 0 , 1 , 0 , 0 , 0 , 1 , 1 , 0};
byte Paso_12[]= {1 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 1};
byte Paso_13[]= {0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0};
byte Paso_14[]= {1 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 1};
byte Paso_15[]= {1 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1};
byte Paso_16[]= {1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1};
byte Paso_17[]= {0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1};
byte salida_rele[]={0,1,2,3,4,5,6,7,8};                     //PINES DE SALIDA AL RELE
const byte numero_rele =9 ;                                 //CANTIDAD DE SALIDAS(RELE)


void setup(){
  //Configuracion LCD
  lcd.init();                  
  lcd.backlight();
  //Pines Rele
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      pinMode(salida_rele[numero_pin],OUTPUT);
  }
  Salida();
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
    pos_paso = pos_paso +1;
    potencia = potencia + 7.5;
    Menu_pasos();
  }
  if((est_up == 1)&&(pos_paso > 1)){
    pos_paso = pos_paso -1;
    potencia = potencia - 7.5;
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



void Salida(){   //Habilita o desabilita las salidas
  if(pos_paso == 1){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_1[numero_pin]);
    }
  }
  if(pos_paso == 2){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_2[numero_pin]);
    }
  }
  if(pos_paso == 3){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_3[numero_pin]);
    }
  }
  if(pos_paso == 4){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_4[numero_pin]);
    }
  }
  if(pos_paso == 5){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_5[numero_pin]);
    }
  }
  if(pos_paso == 6){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_6[numero_pin]);
    }
  }
  if(pos_paso == 7){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_7[numero_pin]);
    }
  }
  if(pos_paso == 8){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_8[numero_pin]);
    }
  }
  if(pos_paso == 9){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_9[numero_pin]);
    }
  }
  if(pos_paso == 10){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_10[numero_pin]);
    }
  }
  if(pos_paso == 11){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_11[numero_pin]);
    }
  }
  if(pos_paso == 12){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_12[numero_pin]);
    }
  }
  if(pos_paso == 13){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_13[numero_pin]);
    }
  }
  if(pos_paso == 14){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_14[numero_pin]);
    }
  }
  if(pos_paso == 15){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_15[numero_pin]);
    }
  }
  if(pos_paso == 16){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_16[numero_pin]);
    }
  }
  if(pos_paso == 17){
    for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      digitalWrite(salida_rele[numero_pin],Paso_17[numero_pin]);
    }
  }
  
}

void cuenta_cero(){

}
