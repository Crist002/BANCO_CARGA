const byte data[] = {144959883589039157};
const size_t dataLength = sizeof(data) / sizeof(data[0]);
int entrada;
void setup()
{
  Serial.begin(9600);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.write(data, dataLength);
  digitalWrite(4,LOW);
}
void loop() 
{
  if(Serial.available()>0){
    entrada = Serial.read();
    Serial.print(entrada);
  }
}
