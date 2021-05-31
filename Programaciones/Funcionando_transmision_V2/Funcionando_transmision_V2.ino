 #include <SoftwareSerial.h>

 #define rxPin 2
 #define txPin 3
 SoftwareSerial mySerial(rxPin,txPin);
byte direccion = 2;
byte funcion = 3;
byte registro_bajo = 00;
byte registro_alto = 55;
byte numero_bajo = 0;
byte numero_alto = 6;
byte crc_bajo = 116;
byte crc_alto = 53;

byte sendData [7];
int datos;
char msg[20];
byte i = 1;

void setup() {
  pinMode(rxPin, INPUT);
   pinMode(txPin, OUTPUT);
   mySerial.begin(9600);
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  sendData[0] = direccion;
  sendData[1] = funcion;
  sendData[2] = registro_bajo;
  sendData[3] = registro_alto;
  sendData[4] = numero_bajo;
  sendData[5] = numero_alto;
  sendData[6] = crc_bajo;
  sendData[7] = crc_alto;
  digitalWrite(4, HIGH);
  mySerial.write(sendData, 8);
  mySerial.flush();
  digitalWrite(4,LOW);
}

void loop() {
  while(mySerial.available()>0){
     datos = mySerial.read();
     sprintf(msg, "%d", datos);
     switch (i){
        case 1:
          Serial.print("Direccion: ");
        break;
        case 2:
          Serial.print("FUNCION: ");
        break;
        case 3:
          Serial.print("TAMAÑO: ");
        break;
        default:
          Serial.print("Registro " + String(i) + " :");
        break;
     }
     Serial.println(msg);
     i++;
  }
}
