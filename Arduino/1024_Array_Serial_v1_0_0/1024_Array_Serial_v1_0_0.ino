
#define NUM_COLUMN 32
#define NUM_ROW 32
#define NUM_SENSOR NUM_ROW*NUM_COLUMN
#define SERIAL_BUFFER_SIZE 2*NUM_SENSOR
#define S1 analogRead(A4)

int header = 0xA8A8;//43176
int footer = 0xEAEA;//60138
int16_t* pD;

int16_t data[2 + NUM_SENSOR];// int16_t = 16비트, 2의5승 . 십진법 다섯자리, 센서 32x32+헤더+푸터
//양수 65,535까지 unsigned = 양수

// Connect 3.3V
uint8_t mux1[5] = {21, 19, 18, 5, 17}; 
uint8_t deMux1[5] = {13, 14, 27, 26, 25};
uint8_t CS1 = 2;
uint8_t WR1 = 16;
uint8_t EN1 = 4;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  analogReadResolution(12);// 분해능 값을 12 비트로 설정합니다

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
  Serial.write((byte*)data, sizeof(data));
  delay(100);
}

void colSelect(uint8_t colnum) {
  digitalWrite(WR1, LOW);
  for (uint8_t x = 0; x < 5; x++) {
    digitalWrite(mux1[x], bitRead(colnum, x));
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
