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
const byte num_pasos = 17;                                  //NUMERO TOTAL DE PASOS
byte salida_rele[]={0,1,2,3,4,5,6,7,8};                     //PINES DE SALIDA AL RELE
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
const int espera = 900;
const byte intervalo = 5;
float pot_int = intervalo * 7.5 ;

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
}


void Logica(){    //Proceso de comparacion y decisiones
  if((est_down == 0)&&(valor <espera)&& (valor > 100)&&(salto== 0)){
    pos_paso = pos_paso +1;
    potencia = potencia + 7.5;
    act = 0;
    Menu_pasos();
    valor = 0;
  }
  if((est_down == 1)&&(pos_paso < num_pasos)){
    reloj();
    if(valor >= espera){
      pos_paso = pos_paso + intervalo;
      potencia = potencia + pot_int;
      act = 0;
      if(pos_paso >= num_pasos){
        pos_paso = num_pasos;
        potencia = 120;
      }
      Menu_pasos();
      salto = 1;
    }
  }
  if((est_down == 0)&&(salto == 1)){
    salto = 0;
    valor = 0;  
    act = 0;
  }



  if((est_up == 0)&&(valor1 <espera)&& (valor1 > 100)&&(salto1== 0)){
    pos_paso = pos_paso -1;
    potencia = potencia - 7.5;
    act1 = 0;
    Menu_pasos();
    valor1 = 0;
  }
  if((est_up == 1)&&(pos_paso > 1)){
    reloj1();
    if(valor1 >= espera){
      pos_paso = pos_paso - intervalo;
      potencia = potencia - pot_int;
      act1 = 0;
      if(pos_paso <= 1){
        pos_paso = 1;
        potencia = 0;
      }
      Menu_pasos();
      salto1 = 1;
    }
  }
  if((est_up == 0)&&(salto1 == 1)){
    salto1 = 0;
    valor1 = 0;  
    act1 = 0;
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
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
    digitalWrite(salida_rele[numero_pin],Paso[pos_paso -1][numero_pin]);
  }
}

void cuenta_cero(){             //Cuando Salta de un paso a otro desactiva sus salidas por 200mSeg
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
    digitalWrite(salida_rele[numero_pin],Paso[0][numero_pin]);
  }
  delay(200);
}

void reloj(){
  tiempo = millis();
  if(act == 0){
    referencia = tiempo;
    act = 1;  
  }
  valor = tiempo - referencia;
}
void reloj1(){
  tiempo1 = millis();
  if(act1 == 0){
    referencia1 = tiempo1;
    act1 = 1;  
  }
  valor1 = tiempo1 - referencia1;
}
