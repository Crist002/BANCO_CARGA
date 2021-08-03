unsigned int crc = 0xFFFF;
byte crc1,crc2;
byte mensaje[]={2,3,0,39,0,6};  //DIRECCION,FUNCION,2BYTES PARA "DIREECION" DE REGSTRO A LEER, 2BYTE PARA "CANTIDAD" DE REGISTROS A LEER

void setup() {
  Serial.begin(9600);
  crc = 0xFFFF;
  for(byte u=0; u<6;u++){   //u<6 debido a que son 6 los datos a realizar XOR, cambiar el valor en caso de ser distinto la cantidad de los datos
    crc ^= mensaje[u];
    for (int i = 8; i != 0; i--) { 
        if ((crc & 0x0001) != 0) {    
          crc >>= 1;                
          crc ^= 0xA001;
        }else{
          crc >>= 1;  
        }                
    }
  }
  crc2= crc>>8;
  crc1= crc&0xFF;
  Serial.println(crc,HEX);
  Serial.println(crc1,HEX); //PRIMER CRC
  Serial.println(crc2,HEX); //SEGUNDO CRC
}

void loop() {
  // put your main code here, to run repeatedly:

}
