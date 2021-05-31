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
boolean m_principal = 1;
boolean m_ensayo = 0;
boolean m_modalidad= 0;
boolean indicado = 0;
byte Opcion;
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
boolean visual = 0;
boolean activado = 1;
boolean est_inicio = 1;
boolean manual = 0;
boolean automatico = 0;
boolean fin2 = 0;
boolean fin = 0;
int pos_paso = 0;     //POSICION DEL PASO ACTUAL
float potencia = 0;
byte v_min = 0;
byte v_max = 0;
byte v_paso = 0;
byte modo= 0;
byte hab_salida = 0;
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
void setup(){
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
  Visual_Inicio();
  detachInterrupt(digitalPinToInterrupt(2));
  si = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void loop(){
  
  Lectura_pulsador();
  Logica_Pulsador();
  Atras();
  if(est_inicio == 1){
    Inicio();
  }
  if(manual == 1){
    automatico = 0;
    Menu_Manual();
  }
  if(automatico == 1){
    manual = 0;
    Selector_Menu();
    Habilitaciones();
    if(fin2 == 1){
      detachInterrupt(digitalPinToInterrupt(2));
      si = 0;
      Menu_Principal();
      fin2 = 0;
    }
  }
  entrada_pulso = 0;
  
}


/////////////////////////////////////////////////////////////////////////
void Inicio(){
  if(entrada_pulso == 1){
    Visual_Inicio();
  }
  if((ok == 1)&&(selector == 1)){
    est_inicio = 0;
    manual = 1;
    automatico = 0;
    selector = 1;
    contenido = "";
    contenido2 =""; 
  }
  if((ok == 1)&&(selector == 2)){
    est_inicio = 0;
    manual = 0;
    automatico = 1;
    selector = 1;
    contenido = "";
    contenido2 ="";
    ok = 0;
  }
}
/////////////////////////////////////////////////////////////////////////
void Atras(){
  if(back == 1){
    if(manual == 1){
      manual = 0;
      automatico = 0;
      est_inicio = 1;
      ok = 0;
      selector = 1;
      back = 0;
      cuenta_cero();
      activado = 1;
    }
    if(automatico == 1){
      if((m_principal == 1)&&(visual == 0)){
        v_max = 0;
        v_min = 0;
        v_paso = 0;
        m_principal = 1;
        automatico = 0;
        manual = 0;
        est_inicio = 1;
        ok = 0;
        back = 0;
        selector = 2;
      }
      if(m_ensayo == 1){
        m_ensayo = 0;
        m_modalidad = 0;
        v_max = 0;
        v_min = 0;
        v_paso = 0;
        m_principal = 1;
        ok = 0;
        back = 0;
        selector = 1;
        indicado = 1;
        Menu_Principal();
      }
      if(m_modalidad == 1){
        m_ensayo = 1;
        ok=0;
        back = 0;
        selector = 1;
        indicado = 1;
        m_principal = 1;
        m_modalidad = 0;
      }
      if(visual == 1){
        indicado = 1;
        visual = 0;
        ok =0;
        back = 0;
      }
    }

  }
}
/////////////////////////////////////////////////////////////////////////

void Visual_Inicio(){
  if(selector <= 1){
    selector = 1;
    fila = 0;
    contenido = "-> MANUAL";
    fila2 = 3;
    contenido2= "AUTOMATICO";
    Visual();
  }else if(selector >= 2){
    selector = 2;
    fila = 3;
    contenido = "MANUAL";
    fila2= 0;
    contenido2= "-> AUTOMATICO";
    Visual();
  }  
}

/////////////////////////////////////////////////////////////////////////
void Menu_Manual(){
  if(entrada_pulso == 1){
    if(selector<= 1){
      selector = 1;
    }else if(selector>= 17){
      selector = 17;
    }
    pos_paso = selector;
    fila = 0;
    contenido = "PASO: " + String(pos_paso);
    fila2 = 0;
    contenido2= "P: " + String(Valores[pos_paso]) + " KW";
    if(ok == 1){
        ok = 0;
        activado =! activado;
        if(activado == 0){
            cuenta_cero();
        }else {
            for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
                digitalWrite(salida_rele[numero_pin],Paso[pos_paso-1][numero_pin]);
            }
        } 
    }
    Visual();
  }
}

/////////////////////////////////////////////////////////////////////////
void Lectura_pulsador(){      //Lee el estado de los pulsadores
  est_up = digitalRead(in_up);
  est_down = digitalRead(in_down);
  est_hab = digitalRead(in_hab);
}
/////////////////////////////////////////////////////////////////////////
void Selector_Menu(){
  if((entrada_pulso == 1)&&(m_principal == 1)){
    Menu_Principal();
    if(ok == 1){
      ok=0;
      Valores_Visual();
    }
  }
  if((v_max>0)&&(v_min>0)&&(v_paso>0)&&(m_principal == 1)){
    m_principal = 0;
    m_ensayo = 1;
    contenido = "";
    contenido2= "";
    Menu_Ensayo();
    m_ensayo = 0;
    contenido = "";
    contenido2= "";
    if(m_modalidad == 1){
      Menu_Modalidad();
    }
  }
}
/////////////////////////////////////////////////////////////////////////

void Menu_Principal(){
  if(selector <= 1){
      selector = 1;
      fila= 0;
      if(v_min > 0){
          contenido = "->P. MIN: " + String(Valores[v_min]);
      }else{
          contenido = "->P. MIN       ";
      }
      fila2= 2;
      contenido2= "P. MAX";
      Visual();
  }else if(selector == 2){
      fila = 2;
      contenido = "P. MIN";
      fila2 = 0;
      if(v_max>0){
          contenido2= "->P. MAX: " + String(Valores[v_max]);
      }else{
          contenido2= "->P. MAX       ";
      }
      Visual();
  }else if(selector >= 3){
    selector = 3;  
    fila =0;
    if(v_paso> 0){
        contenido = "->P. PASO: " + String(Valores[v_paso]);
    }else{
        contenido = "->P. PASO      ";
    }
    fila2 = 0;
    contenido2= "";
    Visual();
  }
}

/////////////////////////////////////////////////////////////////////////
void Valores_Visual(){
    Opcion = selector;
    selector = 1;
    pos_paso = 0;
    visual= 1;
    if(Opcion == 3){
      selector = 2;
    }
    while (indicado < 1){
      Lectura_pulsador();
      Logica_Pulsador();
      if(selector > pos_paso){
          if(selector>=num_pasos){
              selector=num_pasos;  
          }
          pos_paso = selector;
          if((Opcion == 3)&&(pos_paso >= 4)){
            selector = 4;
            pos_paso = selector;
            fila2 =0;
            contenido2="";
          }

          fila = 0;
          contenido= "-> " + String(Valores[pos_paso]) + " KW";
          
          if((Opcion != 3)&&(pos_paso < 17)){
          fila2 =0;
          contenido2="   " + String(Valores[pos_paso + 1]) + " KW";
          }
          if ((Opcion != 3)&&(pos_paso >= 17)){
            fila2 = 0;
            contenido2= "";  
          }
          Visual();
      }
      else if(selector < pos_paso){
        pos_paso = selector;
        if((selector <= 2)&&(Opcion == 3)){
          selector = 2;
          pos_paso = selector;
        }
        fila = 0;
        contenido= "-> " + String(Valores[pos_paso]) + " KW";
        if(Opcion != 3){
        fila2 =0;
        contenido2="   " + String(Valores[pos_paso + 1]) + " KW";
        }
        Visual();
      }
      Atras();
      if(ok == 1){
          ok = 0;
          designaciones();
          fila = 0;
          contenido = "SELECCIONADO";
          fila2 = 0;
          contenido2 = String(Valores[pos_paso]) + "KW";
          Visual();
          delay(2000);
          indicado = 1;
          visual = 0;
      }
      entrada_pulso = 0;
    }


    
    indicado = 0;
    selector = Opcion;
    Menu_Principal();
    entrada_pulso = 0;
}
/////////////////////////////////////////////////////////////////////////
void designaciones(){
  if((m_principal == 1)&&(Opcion == 1)){
    v_min = pos_paso;
  }else if((m_principal == 1)&&(Opcion == 2)){
    v_max = pos_paso;
  }else if((m_principal == 1)&&(Opcion == 3)){
    v_paso = pos_paso;
  }
  if(m_ensayo == 1){
    v_tiempo = Tiempo_paso[selector];
    m_ensayo = 0;
  }
  if(m_modalidad == 1){
    fin = 0;
    if(selector == 1){
      modo=1;
      pos_paso = v_min;
    }else if(selector == 2){
      modo=2;
      pos_paso = v_max;
    }else if(selector == 3){
      pos_paso = v_min;
      modo=3;
    }  
  }
}
/////////////////////////////////////////////////////////////////////////

void Menu_Ensayo(){
  fila = 0;
  contenido= String(Valores[v_min]) + "KW-" + String(Valores[v_max])+ "KW";
  selector=1;
  t_paso=0;
  while(indicado<1){
    Lectura_pulsador();
    Logica_Pulsador();
    if(selector>t_paso){
      if(selector>= max_tiempo){
        selector = max_tiempo;
      }
      t_paso=selector;
      fila2= 0;
      contenido2= "T. PASO: " + String(Tiempo_paso[selector]) + " min" ;
      Visual();
    }
    if(selector<t_paso){
      if(selector<=0){
        selector = 1;
      }
      t_paso=selector;
      fila2= 0;
      contenido2= "T. PASO: " + String(Tiempo_paso[selector]) + " min" ;
      Visual();
    }
    Atras();
    if(ok == 1){
          ok = 0;
          designaciones();
          fila = 0;
          contenido = "SELECCIONADO";
          fila2 = 0;
          contenido2 = String(Tiempo_paso[selector]) + "min";
          Visual();
          delay(2000);
          m_modalidad = 1;
          indicado = 1;
    }
    
  }
  selector = 1;
  entrada_pulso = 0;
}

/////////////////////////////////////////////////////////////////////////
void Menu_Modalidad(){
  selector = 1;
  indicado = 0;
  entrada_pulso = 1;
  while(indicado<1){
    Lectura_pulsador();
    Logica_Pulsador();
    if((selector <= 1)&&(entrada_pulso == 1)){
      selector = 1;
      fila = 0;
      contenido = "->MIN-MAX";
      fila2= 2;
      contenido2= "MAX-MIN";  
      Visual();
    }
    if((selector == 2)&&(entrada_pulso == 1)){
      fila = 2;
      contenido = "MIN-MAX";
      fila2= 0;
      contenido2= "->MAX-MIN";  
      Visual();
    }
    if((selector >= 3)&&(entrada_pulso == 1)){
      selector = 3;
      fila = 0;
      contenido = "->MIN-MAX-MIN";
      fila2= 0;
      contenido2= "";  
      Visual();
    }
    Atras();
    if(ok == 1){
          ok = 0;
          designaciones();
          fila = 3;
          contenido = "COMENZANDO";
          attachInterrupt(digitalPinToInterrupt(2),Alto,FALLING);
          delay(100);
          si = 1;
          fila2 = 0;
          contenido2 = "";
          Visual();
          delay(2000);
          hab_salida = 1;
          
          fila = 0;
          fila2= 0;
          contenido = "";
          contenido2= "";
          indicado = 1;
          Visual();
    }
    entrada_pulso = 0;
  }
  indicado = 0;
}
/////////////////////////////////////////////////////////////////////////

void Menu_pasos(){
    fila = 0;
    contenido = "PASOS: " + String(pos_paso);
    fila2 = 0;
    contenido2 = "POT: " + String(potencia) + "KW";
    cuenta_cero();
    Visual();
}

/////////////////////////////////////////////////////////////////////////
void Visual(){    //Configuro lo que se visualizara en el LCD
  lcd.clear();
  lcd.setCursor(fila,columna);
  lcd.print(contenido);
  lcd.setCursor(fila2,columna2);
  lcd.print(contenido2);
}

/////////////////////////////////////////////////////////////////////////
void Habilitaciones(){
  if(hab_salida == 1){
    
    reloj2();
    p_salto=0;
    if(modo==1){     /////////////MODO 1
      if(valor2 >= (v_tiempo*factor)){
        if(Valores[v_paso] == 7.5){
          p_salto = 1;
        }
        if(Valores[v_paso] == 15){
          p_salto = 2;
        }
        if(Valores[v_paso] == 22.5){
          p_salto = 3;
        }
        pos_paso = pos_paso + p_salto;
        if(pos_paso <= v_min){
          pos_paso = v_min;
        }
        if(fin == 1){
          fin2 = 1;
          hab_salida = 0;
          m_principal = 1;
          selector = 1;
          m_modalidad = 0;
          m_ensayo = 0;
          v_paso = 0;
          v_max = 0;
          v_min = 0;
          cuenta_cero();
          contenido = "";
          contenido2= "";
        }
        if(pos_paso >= v_max){
          pos_paso = v_max;
          fin = 1;
        }
        if(fin2 == 1){
          pos_paso = 1;
        }
        act2 = 0;
        Salida();
          
      }
      if(pos_paso== v_min){
        Salida();
      }
    }         /////////////FIN MODO 1

    if(modo==2){     /////////////MODO 2
      if(valor2 >= (v_tiempo*factor)){
        if(Valores[v_paso] == 7.5){
          p_salto = 1;
        }
        if(Valores[v_paso] == 15){
          p_salto = 2;
        }
        if(Valores[v_paso] == 22.5){
          p_salto = 3;
        }
        pos_paso = pos_paso - p_salto;
        if(fin == 1){
          hab_salida = 0;
          fin2 = 1;
          m_principal = 1;
          selector = 1;
          m_modalidad = 0;
          m_ensayo = 0;
          v_paso = 0;
          v_max = 0;
          v_min = 0;
          cuenta_cero();
          contenido = "";
          contenido2= "";
        }
        if(pos_paso <= v_min){
          pos_paso = v_min;
          fin = 1;
        }
        if(pos_paso > v_max){
          pos_paso = v_max;
        }
        if(fin2 == 1){
          pos_paso = 1;
        }
        act2 = 0;
        Salida();
      }
      if(pos_paso== v_max){
        Salida();
      }
    }         /////////////FIN MODO 2

    if(modo==3){     /////////////MODO 3
      if(valor2 >= (v_tiempo*factor)){
        if(Valores[v_paso] == 7.5){
          p_salto = 1;
        }
        if(Valores[v_paso] == 15){
          p_salto = 2;
        }
        if(Valores[v_paso] == 22.5){
          p_salto = 3;
        }
        pos_paso = pos_paso + p_salto;
        if(pos_paso <= v_min){
          pos_paso = v_min;
        }
        if(pos_paso >= v_max){
          pos_paso = v_max;
          fin = 1;
        }
        if(fin == 1){
          modo = 2; 
          fin = 0;
        }
        act2 = 0;
        Salida();
      }
      if(pos_paso== v_min){
        Salida();
      }
    }         /////////////FIN MODO 3
  
  }else{
    cuenta_cero();  
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
void cuenta_cero(){             //Desactiva sus salidas
  for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){
    digitalWrite(salida_rele[numero_pin],Paso[0][numero_pin]);
  }
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
void Alto(){        //SUBRUTINA DE LA INTERRUPCION POR HARDWARE
  if(si == 1){
    wdt_enable(WDTO_1S);    //HABILITACION DEL WATCHDOG EN UN TIEMPO PROGRAMADO DE 1SEGUNDO
    while(1){
        for (byte numero_pin = 0;numero_pin < numero_rele; numero_pin++){   //DESACTIVACION DE SALIDAS DE LOS RELES
            digitalWrite(salida_rele[numero_pin],Paso[0][numero_pin]);
        }
    }
  }
}
