#include<avr/wdt.h>
int conteo = 500;
byte contador = 0;
boolean si = 0;

void setup() {
  wdt_disable();
  delay(3200);
  Serial.begin(9600);
  Serial.println("");
  Serial.println("INICIANDO PROGRAMA");
  Serial.println("");
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  
}

void loop() {
  detachInterrupt(digitalPinToInterrupt(2));
  si = 0;
  while(1){
    Serial.println(contador);
    digitalWrite(13, HIGH);
    delay(conteo);
    digitalWrite(13, LOW);
    delay(conteo);
    contador++;
    if(contador> 8){
      Serial.println("ACTIVADA INTERRUPCION");
      attachInterrupt(digitalPinToInterrupt(2),Alto,FALLING);
      si = 1;
    }
  }
}

void Alto(){
  if(si == 1){
    wdt_enable(WDTO_2S);

    while(1){
      digitalWrite(13, HIGH);
    }
  }
}
