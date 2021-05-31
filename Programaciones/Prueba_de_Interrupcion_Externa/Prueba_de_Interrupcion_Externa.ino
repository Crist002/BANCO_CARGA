volatile int conteo = 100;
byte contador = 0;
void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2),Alto,FALLING);
}

void loop() {
  while(1){
    Serial.println(conteo);
    digitalWrite(13, HIGH);
    delay(conteo);
    digitalWrite(13, LOW);
    delay(conteo);
    if(conteo >= 2000){
     detachInterrupt(digitalPinToInterrupt(2));
     contador++;
    }
    if(contador> 3){
      contador = 0;
      attachInterrupt(digitalPinToInterrupt(2),Alto,FALLING);
      conteo = 200;
    }
  }
}

void Alto(){
  conteo = conteo + 500;
}
