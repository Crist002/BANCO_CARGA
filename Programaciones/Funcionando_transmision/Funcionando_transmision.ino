byte direccion = 2;
byte funcion = 3;
byte registro_bajo = 00;
byte registro_alto = 7;
byte numero_bajo = 0;
byte numero_alto = 6;
byte crc_bajo = 116;
byte crc_alto = 58;

byte sendData [7];
int datos;

void setup() {
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

void loop() {
  sendData[0] = direccion;
  sendData[1] = funcion;
  sendData[2] = registro_bajo;
  sendData[3] = registro_alto;
  sendData[4] = numero_bajo;
  sendData[5] = numero_alto;
  sendData[6] = crc_bajo;
  sendData[7] = crc_alto;
  
  Serial.write(sendData, 8);
  Serial.flush();
  digitalWrite(4,LOW);
  while(Serial.available()>0){
      datos = Serial.read();
      Serial.println(datos, HEX);
  }
  digitalWrite(4, HIGH);
  delay(2000);
}
