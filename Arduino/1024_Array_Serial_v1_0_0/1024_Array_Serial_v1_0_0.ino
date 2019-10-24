
#define NUM_COLUMN 32
#define NUM_ROW 32
#define NUM_SENSOR NUM_ROW*NUM_COLUMN // 32 * 32 = 1024
#define SERIAL_BUFFER_SIZE 2*NUM_SENSOR //mean : buffer 1024 * 2
#define S1 analogRead(A4) //mcu의 입력은 여기로만 받는듯... analogRead 하나임 분해능 10의 12승 설정. 즉 4096까지 가능

int header = 0xA8A8;//43176
int footer = 0xEAEA;//60138
int16_t* pD;

int16_t data[2 + NUM_SENSOR];// int16_t = 16비트, 2의5승 . 십진법 다섯자리, 센서 32x32+2 = 1026
//양수 65,535까지 unsigned = 양수

// Connect 3.3V
uint8_t mux1[5] = {21, 19, 18, 5, 17}; //   mux : 한개의 전압에 32개 센서에 전압출력, 실제 센서제작 mux x 2 & De-mux x 1, 
uint8_t deMux1[5] = {13, 14, 27, 26, 25}; // deMux : 32개 값 받아 한개의 신호 출력, mux2개가 오른쪽 위쪽일때 윗쪽
uint8_t CS1 = 2;//CS1,WR1,EN1 다 mux,demux 엮여있음. 역할:??
uint8_t WR1 = 16;//WR이 write 무엇무엇일듯...
uint8_t EN1 = 4;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  analogReadResolution(12);// 분해능 값을 12 비트로 설정합니다 아날로그 값을 2 ^ 12(2의 12승) 즉 0 ~ 4095까지 측정 가능

  pinMode(CS1, OUTPUT);
  digitalWrite(CS1, LOW); //(0v) 전압

  pinMode(WR1, OUTPUT);
  digitalWrite(WR1, LOW);

  pinMode(EN1, OUTPUT);
  digitalWrite(EN1, LOW);

  for (uint8_t i = 0; i < 5; i++) {
    pinMode(mux1[i], OUTPUT);
    pinMode(deMux1[i], OUTPUT);
    digitalWrite(mux1[i], LOW);
    digitalWrite(deMux1[i], LOW);
  }

  pD = &data[0]; // pD에 data[0]의 주소값을 참조시킴, 처음이니까 헤더 작성
  *pD = header; // header = 0xA8A8;//43176

  pD = &data[1 + NUM_SENSOR];//헤더 랑 32개 센서 추가하여서 data[33]에 푸터 작성
  *pD = footer;

}

unsigned long tic = 0;
unsigned long toc = 0;

void loop() {
  // put your main code here, to run repeatedly:
  for (uint8_t i = 0; i < NUM_ROW; i++) {
    rowSelect(i);
    for (uint8_t j = 0; j < NUM_COLUMN; j++) {
      colSelect(j);
      delayMicroseconds(100);
      pD = &data[i * NUM_COLUMN + j + 1]; *pD = (int16_t)S1;
    }
  }
  //  tic = micros();
  //  Serial.println(tic-toc);
  //  toc = tic;
Serial.write((byte*)data, sizeof(data)); //Writes binary data to the serial port. Serial.write(buf, len)
                                           //sizeof() => total byte. so, int16_t data [1024+2] ==>1026*2, dif from int=4byte ?print test

/* 만약 data를 int로 전환 한다면 사용.
for (int i = 0; i < sizeof(data)/sizeof(int); i++) {
  Serial.println(data[i]);
}*/

  delay(100); 
}


void colSelect(uint8_t colnum) {
  digitalWrite(WR1, LOW);
  for (uint8_t x = 0; x < 5; x++) {
    digitalWrite(mux1[x], bitRead(colnum, x));//일단 넘어감? 
  }
  digitalWrite(WR1, HIGH);
}

void rowSelect(uint8_t rownum) {
  digitalWrite(WR1, LOW);
  for (uint8_t y = 0; y < 5; y++) {
    digitalWrite(deMux1[y], bitRead(rownum, y));
  }
  //Input data is latched on the rising edge of WR.
  digitalWrite(WR1, HIGH);
}
