int a = 0x42;
int b = 0xDD;
int c = 0XCC;
int d = 0x80;
unsigned long m;
void setup() {
  Serial.begin(9600);
  int e = ((a*0x100)+b)>>7;
  int ex = e-126;
  float d2 = b;
  float c1 =d2*65536;
  float d3 = c;
  float c2 = d3*256;
  float c3 = d;
  if(((b>>7)&(0x1))== 1){
    m= c1+c2+c3;
  }else{
    m= 8388608+c1+c2+c3;
  }
  float VALOR = pow(2,ex)*m/(256*65536);
  Serial.println(VALOR);
}

void loop() {
  // put your main code here, to run repeatedly:

}
