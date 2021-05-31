#include<avr/wdt.h>
boolean si = 0;
unsigned int factor = 1000;
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
//VARIABLES MENU
boolean m_principal = 1;
boolean m_ensayo = 0;
boolean m_modalidad= 0;
//Variables pulsadores
const byte in_up = 10;         //PULSADOR ARRIBA
const byte in_down = 11;      //PULSADOR ABAJO
const byte in_hab = 2;       //INTERRUPTOR HABILITACION
boolean est_up;               //GUARDA EL ESTADO DE PULSADOR ARRIBA
boolean est_down;             //GUARDA EL ESTADO DE PULSADOR ABAJO
boolean est_hab;              //GUARDA EL ESTADO DE INT HABILITACION
boolean ok = 0;
boolean back = 0;
int selector = 1;
boolean entrada_pulso = 0;
//Variables de control
int pos_paso = 0;     //POSICION DEL PASO ACTUAL
float potencia = 0;
byte v_min = 0;
byte v_max = 0;
byte v_paso = 0;
byte p_salto = 0;
//DEFINICION DE PASOS     
//*LAS PRIMERAS 8 SALIDAS ESTAN INVERTIDAS EN SU LOGICA POR EL MODULO RELE QUE TRABAJA A LA INVERSA *
//*LA ULTIMA SALIDA TIENE SU LOGICA NORMAL*
byte Paso[17][9]={                      //PASO[CANTIDAD DE PASOS][ACTIVAR/DESACTIVAR SALIDAS]
{1 , 1 , 1 , 1 , 1 , 1 , 1 , 1 , 0},    //PASO 1
{1 , 1 , 0 , 1 , 1 , 1 , 1 , 1 , 0},    //PASO 2
{1 , 1 , 1 , 1 , 1 , 0 , 1 , 1 , 0},    //PASO 3
{1 , 1 , 0 , 0 , 1 , 1 , 1 , 1 , 0},    //PASO 4
{1 , 1 , 1 , 0 , 1 , 0 , 1 , 1 , 0},    //PASO 5
{1 , 1 , 0 , 1 , 1 , 1 , 1 , 0 , 1},    //PASO 6
{1 , 0 , 1 , 0 , 1 , 0 , 1 , 1 , 0},    //PASO 7
{1 , 1 , 0 , 0 , 1 , 1 , 1 , 0 , 1},    //PASO 8
{1 , 0 , 1 , 0 , 1 , 0 , 0 , 1 , 0},    //PASO 9
{1 , 0 , 0 , 1 , 1 , 0 , 1 , 0 , 1},    //PASO 10
{0 , 0 , 1 , 0 , 0 , 0 , 1 , 1 , 0},    //PASO 11
{1 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 1},    //PASO 12
{0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0},    //PASO 13
{1 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 1},    //PASO 14
{1 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 15
{1 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 16
{0 , 0 , 1 , 0 , 0 , 0 , 0 , 0 , 1},    //PASO 17
};
byte Tiempo_paso[]={0,1,2,3,4,5,10,15,20,25,30,35,40,45,50,55,60};
byte v_tiempo = 0;
byte t_paso= 1;
byte max_tiempo= 16;

float Valores[]={0 ,0, 7.5 , 15 , 22.5 , 30 , 37.5 , 45 , 52.5 , 60 , 67.5 , 75 , 82.5 , 90 , 97.5 , 105 , 112.5 , 120 };
const byte num_pasos = 17;                                  //NUMERO TOTAL DE PASOS
byte salida_rele[]={0,1,3,4,5,6,7,8,9};                     //PINES DE SALIDA AL RELE
const byte numero_rele =9 ;                                 //CANTIDAD DE SALIDAS(RELE)
//CONTADOR
unsigned long tiempo = 0;
unsigned long referencia = 0;
boolean act = 0;
unsigned long valor;
boolean salto = 0;
boolean salto1 = 0;
unsigned long tiempo1 = 0;
unsigned long referencia1 = 0;
boolean act1 = 0;
unsigned long valor1;
boolean salto2 = 0;
unsigned long tiempo2 = 0;
unsigned long referencia2 = 0;
boolean act2 = 0;
unsigned long valor2;
boolean salto3 = 0;
unsigned long tiempo3 = 0;
unsigned long referencia3 = 0;
boolean act3 = 0;
unsigned long valor3;
const int espera = 900;
const byte intervalo = 5;
float pot_int = intervalo * 7.5 ;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  wdt_disable();    //watchdog desabilitado
  //Configuracion LCD
  lcd.init();                  
  lcd.backlight();
  //Pines Rele
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
      pinMode(salida_rele[numero_pin],OUTPUT);
  }
  cuenta_cero();
  delay(3100);
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
  detachInterrupt(digitalPinToInterrupt(2));
  si = 0;
}

void loop() {
  Lectura_pulsador();
  Logica_Pulsador();
  if(entrada_pulso == 1){
    Designacion();
    LCD();
    Visual();
  }
  estado_pulso = 0;
}


void Designacion(){
  if(ok == 1){
    switch (sector){
        case 0
            sector = selector;
            selector = 1;
        break
        case 1
            
        break
        case 2
            
        break
        case 3
            
        break
    }
  }
}


void LCD(){
  switch(sector){
      case 1
        contenido = "->MANUAL";
        contenido2= "  AUTOMATICO";
      break
      case 2
        contenido = "  MANUAL";
        contenido2= "->AUTOMATICO";
      break
      case 3
        contenido = "  COMUNICACION";
        contenido2= "";
      break
    }
}


void Menu(){
  switch(selector){
      case 1
        contenido = "->MANUAL";
        contenido2= "  AUTOMATICO";
      break
      case 2
        contenido = "  MANUAL";
        contenido2= "->AUTOMATICO";
      break
      case 3
        contenido = "  COMUNICACION";
        contenido2= "";
      break
    }
  
}

void Manual(){
  
}

void Automatico(){
  
}

void Comunicacion(){
  
}
/////////////////////////////////////////////////////////////////////////
void Lectura_pulsador(){      //Lee el estado de los pulsadores
  est_up = digitalRead(in_up);
  est_down = digitalRead(in_down);
  est_hab = digitalRead(in_hab);
}
/////////////////////////////////////////////////////////////////////////
void Logica_Pulsador(){    //Proceso de comparacion y decisiones
  if((est_down == 0)&&(valor <espera)&& (valor > 100)&&(salto== 0)){       //LOGICA DE PULL_DOWN
    entrada_pulso = 1;
    selector++;
    act = 0;
    valor = 0;
  }
  if((est_down == 1)&&(pos_paso < (num_pasos+1))){
    reloj();
    if(valor >= espera){
      entrada_pulso = 1;
      selector = selector + intervalo;
      act = 0;
      if(pos_paso >= num_pasos){
        pos_paso = num_pasos;
        potencia = 120;
      }
      salto = 1;
    }
  }
  if((est_down == 0)&&(salto == 1)){
    salto = 0;
    valor = 0;  
    act = 0;
  }
  if((est_up == 0)&&(valor1 <espera)&& (valor1 > 100)&&(salto1== 0)){         //LOGICA DE PULL_UP
    entrada_pulso = 1;
    selector--;
    act1 = 0;
    valor1 = 0;
  }
  if((est_up == 1)&&(selector > 1)){
    reloj1();
    if(valor1 >= espera){
      entrada_pulso = 1;
      selector = selector - intervalo;
      if(selector<= 1){
        selector = 1;  
      }
      act1 = 0;
      if(pos_paso <= 1){
        pos_paso = 1;
        potencia = 0;
      }
      salto1 = 1;
    }
  }
  if((est_up == 0)&&(salto1 == 1)){
    salto1 = 0;
    valor1 = 0;  
    act1 = 0;
  }

  if((est_hab == 0)&&(valor3 <espera)&& (valor3 > 50)&&(salto3== 0)){         //LOGICA DE PULL_HABILITADO
    entrada_pulso = 1;
    ok = 1;
    act3 = 0;
    valor3 = 0;
  }
  if((est_hab == 1)&&(selector >= 1)){
    reloj3();
    if(valor3 >= espera){
      entrada_pulso = 1;
      back = 1;
      act3 = 0;
      salto3 = 1;
    }
  }
  if((est_hab == 0)&&(salto3 == 1)){
    salto3 = 0;
    valor3 = 0;  
    act3 = 0;
  }
}

/////////////////////////////////////////////////////////////////////////
void reloj(){
  tiempo = millis();
  if(act == 0){
    referencia = tiempo;
    act = 1;  
  }
  valor = tiempo - referencia;
}
/////////////////////////////////////////////////////////////////////////
void reloj1(){
  tiempo1 = millis();
  if(act1 == 0){
    referencia1 = tiempo1;
    act1 = 1;  
  }
  valor1 = tiempo1 - referencia1;
}

/////////////////////////////////////////////////////////////////////////
void reloj2(){
  tiempo2 = millis();
  if(act2 == 0){
    referencia2 = tiempo2;
    act2 = 1;  
  }
  valor2 = tiempo2 - referencia2;
}

/////////////////////////////////////////////////////////////////////////
void reloj3(){
  tiempo3 = millis();
  if(act3 == 0){
    referencia3 = tiempo3;
    act3 = 1;  
  }
  valor3 = tiempo3 - referencia3;
}
/////////////////////////////////////////////////////////////////////////
void cuenta_cero(){             //Desactiva sus salidas
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
    digitalWrite(salida_rele[numero_pin],Paso[0][numero_pin]);
  }
}
/////////////////////////////////////////////////////////////////////////

void Salida(){   //Habilita o desabilita las salidas
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
    fila2=numero_pin;
    lcd.setCursor(0,0);
    lcd.print(pos_paso);
    lcd.setCursor(fila2, 1);
    lcd.print(Paso[pos_paso-1][numero_pin]);
    digitalWrite(salida_rele[numero_pin],Paso[pos_paso-1][numero_pin]);
  }
}
/////////////////////////////////////////////////////////////////////////
void Visual(){    //Configuro lo que se visualizara en el LCD
  lcd.clear();
  lcd.setCursor(fila,columna);
  lcd.print(contenido);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}
