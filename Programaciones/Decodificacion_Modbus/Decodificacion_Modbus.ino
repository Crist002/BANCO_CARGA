int a = 0x43;
int b = 0x5E;
int c = 0x38;
int d = 0x52;
unsigned long m;
void setup() {
  Serial.begin(9600);
  int e = ((a*0x100)+b)>>7;
  int ex = e-126;
  if(((b>>7)&0x1)== 1){
    m= (b*0x10000)+(c*0x100)+d;
  }else{
    m= 0x800000+(b*0x10000)+(c*0x100)+d;
  }
  float VALOR = pow(2,ex)*m/(256*65536);
  Serial.println(e);
  Serial.println(m);
  Serial.println(VALOR);
}

void loop() {
  // put your main code here, to run repeatedly:

}
