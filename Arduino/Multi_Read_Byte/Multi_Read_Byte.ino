#define SERIAL_BUFFER_SIZE 128 //mean : buffer 128byte

uint8_t outMux[8] = {26,25,33,32,17,5,18,19}; // 센서 가로 8줄
uint8_t inMux[4] = {13,12,14,27};//센서 세로 4줄
int header = 0xA8A8;//아스키코드 -> 43176
int footer = 0xEAEA; //60138
int16_t* pD;

int16_t data[34];

void setup() {

  Serial.begin(115200);

  for(int i=0; i++; i<8){
    pinMode(outMux[i],INPUT);//
  }
  pD = &data[0];//9행의 data[0] 주소값 pointer D에 할당
  *pD = header;//그리고 43176 넣기

  pD = &data[33];
  *pD = footer;
}

void loop() {
  // put your main code here, to run repeatedly:
for(int i = 0; i<8; i++){
  pinMode(outMux[i],OUTPUT); //outMux[i]에 출력모드. 즉 전압 출력! <->입력은 데이터 mcu로입력
  digitalWrite(outMux[i], HIGH);
  pD = &data[i*4+1]; *pD = (int16_t)analogRead(inMux[0]);
  //data[i*4+1]의 메모리 주소를 포인터 변수 pD에 저장, 역참조연산자 *로 포인터pD의 메모리 주소에 analogRead() 값 저장
  // 즉 data[i*4+1] = analogRead(inMux[0])와 같은 의미임.
  pD = &data[i*4+2]; *pD = (int16_t)analogRead(inMux[1]);
  pD = &data[i*4+3]; *pD = (int16_t)analogRead(inMux[2]);
  pD = &data[i*4+4]; *pD = (int16_t)analogRead(inMux[3]);
  pinMode(outMux[i],INPUT);
  Serial.print(
      data[i*4+1]+(String)" "
      +data[i*4+2]+(String)" "
      +data[i*4+3]+(String)" "
      +data[i*4+4]+(String)" "
      ); 
}
  Serial.println();
  
//  Serial.write((byte*)data, sizeof(data));
  delay(50);
}
