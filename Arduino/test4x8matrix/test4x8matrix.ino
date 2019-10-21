#define SERIAL_BUFFER_SIZE 128

int outMux[8] = {26,25,33,32,17,5,18,19};
int inMux[4] = {13,12,14,27};
int header = 0xA8A8;//아스키코드 -> 43176
int footer = 0xEAEA; //60138

int data[34];
int* pD;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  for(int i=0; i<8;i++){
    pinMode(outMux[i], INPUT);
  }
//  pD = &data[0];
//  *pD = header;
//
//  pD = &data[33];
//  *pD = footer;
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i=0; i<8; i++){
    pinMode(outMux[i], OUTPUT);
    digitalWrite(outMux[i], HIGH);
    pD = &data[i*4+1]; *pD = analogRead(inMux[0]);
    pD = &data[i*4+2]; *pD = analogRead(inMux[1]);
    pD = &data[i*4+3]; *pD = analogRead(inMux[2]);
    pD = &data[i*4+4]; *pD = analogRead(inMux[3]);
    pinMode(outMux[i], INPUT);
    Serial.print(data[i*4+1]+(String)" "
      +data[i*4+2]+(String)" "
      +data[i*4+3]+(String)" "
      +data[i*4+4]+(String)" "
      ); 
  }
  Serial.println();
    
//  pinMode(26, OUTPUT);
//  digitalWrite(26, HIGH);
//  data = analogRead(13);
//
//  pinMode(26, INPUT);
//  Serial.println(data);
//Serial.println((int16_t)analogRead(12));
//Serial.write((int16_t)analogRead(12));
  delay(50);
}
