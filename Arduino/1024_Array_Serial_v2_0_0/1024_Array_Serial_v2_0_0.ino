#define NUM_COLUMN 32
#define NUM_ROW 32
#define NUM_SENSOR NUM_ROW*NUM_COLUMN // 32 * 32 = 1024
#define SERIAL_BUFFER_SIZE 2*NUM_SENSOR //mean : buffer 1024 * 2
#define S1 analogRead(A4) //mcu의 입력은 여기로만 받는듯... analogRead 하나임 분해능 2^12 설정. 즉 4096까지 가능, 원래 아두이노는 10bit 아날로그 값 가짐. 즉 2^10= 1024값 표현 가능
//핀번호 32

int header = 0xA8A8;//43176
int footer = 0xEAEA;//60138
int16_t* pD;

int16_t data[2 + NUM_SENSOR];// int16_t = 16비트, 2의16승 표현가능=65563이 2의16승. 센서 32x32+2 = 1026
//양수 65,535까지 unsigned = 양수

// Connect 3.3V
uint8_t mux1[5] = {21, 19, 18, 5, 17}; //   mux : 한개의 전압에 32개 센서에 전압출력, 실제 센서제작 mux x 2 & De-mux x 1, 설계랑 달리 위쪽
uint8_t deMux1[5] = {13, 14, 27, 26, 25}; // deMux : 32개 값 받아 한개의 신호 출력, 설계랑 달리 왼쪽
uint8_t CS1 = 2;//CS1,WR1,EN1 다 mux,demux 엮여있음. Chip Selection
uint8_t CS2 = 22;
uint8_t WR1 = 16;//WR이 write 무엇무엇일듯... 16핀 : UART 2 RX.
uint8_t EN1 = 4;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  analogReadResolution(12);// 분해능 값을 12 비트로 설정합니다 아날로그 값을 2 ^ 12(2의 12승) 즉 0 ~ 4095까지 측정 가능

  //  pinMode(CS1, OUTPUT);//chip select
  //  digitalWrite(CS1, LOW); //Low 됨, HIGH 안됨
  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, HIGH);
//
//  pinMode(CS2, OUTPUT);
//  digitalWrite(CS2, HIGH);

  pinMode(WR1, OUTPUT);
  digitalWrite(WR1, LOW);

  pinMode(EN1, OUTPUT);
  digitalWrite(EN1, LOW);//

  for (uint8_t i = 0; i < 5; i++) {
    pinMode(mux1[i], OUTPUT); //출력하겠다!! OUTPUT mode : 전력을 보내니깐 OUTPUT. 값을 읽어올땐 INPUT 으로 전환.
    pinMode(deMux1[i], OUTPUT); //현재 소스에서 값은 5행에 analogRead(A4)에 A4가 alias로 mcu 32번 핀에서 값 갖고 옴
    digitalWrite(mux1[i], LOW); //아직 대기
    digitalWrite(deMux1[i], LOW);
  }

  pD = &data[0]; // pD에 data[0]의 주소값을 참조시킴, 처음이니까 헤더 작성
  *pD = header; // header = 0xA8A8;//43176

  pD = &data[1 + NUM_SENSOR];//헤더 랑 32개 센서 추가하여서 data[33]에 푸터 작성
  *pD = footer;
}

void loop() {
  // put your main code here, to run repeatedly:
  for (uint8_t i = 0; i < NUM_ROW; i++) {
    rowSelect(i);
    //delay(100);
    for (uint8_t j = 0; j < NUM_COLUMN; j++) {
      colSelect(j);
      delayMicroseconds(100);

      data[i * NUM_COLUMN + j + 1] = (int16_t)S1; //maximum value : 4096 즉 2의 12승까지
      
//      Serial.print(data[i * NUM_COLUMN + j + 1]);
//      Serial.print(" ");
    }
//    Serial.println();
  }
//  Serial.println();
//  Serial.println();
//  Serial.println();
//  delay(4000);
  Serial.write((byte*)data, sizeof(data));


}

void colSelect(uint8_t colnum) {
  digitalWrite(WR1, LOW);
  for (uint8_t x = 0; x < 5; x++) {
    digitalWrite(deMux1[x], bitRead(colnum, x));//High 또는 low값을 디지털 핀에 출력. 전압출력 1=HIGH, 0=LOW
  
  }
  
  digitalWrite(WR1, HIGH); //RX 핀에 전류보내기
}//Input data is latched on the rising edge of WR.

void rowSelect(uint8_t rownum) {
  //  chip change. maybe..
//  if (rownum == 32) {
//    digitalWrite(CS2, LOW);
//    digitalWrite(CS1, HIGH);
//  }
  if (rownum == 0) {
    digitalWrite(CS1, LOW);
    digitalWrite(CS2, HIGH);
  }

  digitalWrite(WR1, LOW);
  for (uint8_t y = 0; y < 5; y++) {
    digitalWrite(mux1[y], bitRead(rownum, y));
  }
  //Input data is latched on the rising edge of WR.
  digitalWrite(WR1, HIGH);
}
