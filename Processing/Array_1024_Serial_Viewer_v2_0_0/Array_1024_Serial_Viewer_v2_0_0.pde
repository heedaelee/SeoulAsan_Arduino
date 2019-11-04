/**************************************************************************
 * Update Note
 *  Array_1024_Serial_Viewer ver1.0.1 (2019.10.06)
 *    - Stabilize serial data transfer packet
 *    - Program pair with Arduino\1024_Array_Serial\1024_Array_Serial.ino
 **************************************************************************/

import processing.serial.*;
import java.util.Date;
import controlP5.*;
import static javax.swing.JOptionPane.*;


ControlP5 cp5;
PFont font;
String progVer = "Rev 2.0.0";

// Matirx array constant.
int NUM_COLUMN = 32;
int NUM_ROW = 32;
int NUM_SENSOR = NUM_COLUMN * NUM_ROW;

// Set-up overall Serial comm
Serial  myPort;   // Define serial comm
int lf = 0xEAEA;      // ASCII linefeed (only Single byte works)
byte[] inBytes = new byte[NUM_SENSOR*2 + 4]; //32*32*2 +4 , why *2? reason : In my thought, before arduino, unit16_t (2byte) => (byte*) 
int[][] data = new int[NUM_COLUMN][NUM_ROW];

// Set-up text file recording.
//PrintWriter output;
Table table;

//Set-up interval saving time
int startedTime;
int savedTime;
int totalTime = 500; //1s = 1000
int passedTime;


void settings() {
  // Set size of window : size(width, Height)
  size(40 + 25 * NUM_COLUMN, 100 + 25 * NUM_ROW);
}

// Variables for current date & time.
int d;    // Values from 1 - 31
int m;  // Values from 1 - 12
int y;   // 2003, 2004, 2005, etc.
int s;  // Values from 0 - 59
int mn;  // Values from 0 - 59
int h;    // Values from 0 - 23

//button
int sBtnX = 340;
int sBtnY = 25;
int sBtnWidth = 60;
int sBtnHeight = 30;

String portName;

void setup() { 

  // Set frame rate.
  frameRate(100);

  font = createFont("Arial Bold", 48);

  table = new Table();
  table.addColumn("TimeStamp");
  for (int i=1; i<=NUM_SENSOR; i++ ) {
    table.addColumn("Col"+i);
  }

  //save current time
  savedTime = millis();

  cp5 = new ControlP5(this);

  // create a new button 
  Button saveBtn= cp5.addButton("SAVE")
    .setPosition(sBtnX, sBtnY)
    .setSize(sBtnWidth, sBtnHeight)
    ;
  //saveBtn.setColorBackground(color(#ffffff));
  //saveBtn.setColorActive(); when mouse-over
  saveBtn.getCaptionLabel().setFont(font).setSize(14);


  Button resetBtn= cp5.addButton("RESET")
    .setPosition(sBtnX+sBtnWidth+10, sBtnY)
    .setSize(sBtnWidth, sBtnHeight);
  resetBtn.getCaptionLabel().setFont(font).setSize(14);

  // Select serial port.
  println(Serial.list());  // Show up all possible serial ports.
  delay(2000);
  portName = Serial.list()[1];  // Set specific serial port to comm.
  println("portName : "+portName);
  myPort = new Serial(this, portName, 115200);
  myPort.clear();
  myPort.bufferUntil(lf); //lf = footer (dec ->60138), read untill footer set in arduino before..
  // Sets a specific byte to buffer until before calling serialEvent().
}


void draw() {

  // Set background color as gray.
  background(100);

  // Set font size and color.
  textFont(font, 10);
  fill(255);
  text(progVer, width-70, height-15);
  text("FPS :"+int(frameRate), 20, 60);
  text("Connected port: " + portName, 20, 40);


  // Set font size and color.
  textFont(font, 18);
  fill(255);
  getDate();
  text(str(y)+"."+str(m)+"."+str(d)+". "+str(h)+":"+str(mn)+":"+str(s), width-200, 40);


  //Draw rectangular for sensor indication.
  for (int i=0; i<NUM_ROW; i++) {
    for (int j=0; j<NUM_COLUMN; j++) {
      fill(data[j][i]*14, 0, 0);
      rect(20+i*25, 70+j*25, 21, 21, 7);
    }
  }
}

int minusConst = 38;
int loadingTime = 3400;

// Receive data from Arduino and analyze it according to data form.
void serialEvent(Serial myPort) {
  // Trim single data packet
  if (myPort.available() > ((NUM_SENSOR+1) * 2)) { //available :   Returns the number of bytes available.
    inBytes = myPort.readBytes(); //Reads a group of bytes from the buffer or null if there are none available. received Byte inBytes[2052]
    myPort.clear();
  }

  try {
    passedTime = millis() - savedTime;
    //making new row data after loadingTime, and per totalTimes. 
    if (millis()>loadingTime && passedTime > totalTime) {

      TableRow newRow = table.addRow();
      newRow.setString("TimeStamp", timeStamp(millis()));    
      for (int i=0; i<NUM_ROW; i++) {    
        for (int j=0; j<NUM_COLUMN; j++) {
          data[i][j] = remap(byte2int(inBytes[(i*NUM_COLUMN+j)*2+4], inBytes[(i*NUM_COLUMN+j)*2+3]))-minusConst;

          //converting minus datas to 0
          if (data[i][j]<0) { 
            data[i][j] = 0;
          }
          //insert data
          newRow.setInt("Col"+Integer.toString(i*NUM_COLUMN+j+1), data[i][j]);
        }
      }    
      //renewal
      savedTime = millis();
    } else {
      //println("passedTime : " + passedTime);
      // Data re-arrangement to matrix.
      for (int i=0; i<NUM_ROW; i++) {    
        for (int j=0; j<NUM_COLUMN; j++) {        
          data[i][j] = remap(byte2int(inBytes[(i*NUM_COLUMN+j)*2+4], inBytes[(i*NUM_COLUMN+j)*2+3]))-minusConst; 
          //byte to int // why +4, +3 ?? cause inBytes=readBytes(); inBytes includes header (=2byte)
          //output.print(data[i][j]+" "+"\t");//output.print(" "+"\t"); //int test
        }
      }
    }
  }
  catch (Exception e) {
    e.printStackTrace();
  }
}

//SAVE button click event
public void SAVE() {
  println("data saved");
  getDate();
  saveTable(table, str(y)+"_"+str(m)+"_"+str(d)+"_"+str(h)+"_"+str(mn)+"_"+str(s)+".csv");
  showMessageDialog(null, "저장되었습니다", "메시지", INFORMATION_MESSAGE);
  table.clearRows();
}

//RESET button click event
public void RESET() {
  println("data reset");
  showMessageDialog(null, "데이터 로우를 초기화합니다", "메시지", INFORMATION_MESSAGE);
  table.clearRows();
}

// Set escape event for terminate program.
void keyPressed() {
  if (key == 27) { // 27 means ESC key
    //output.flush(); // Writes the remaining data to the file
    //output.close(); // Finishes the file
    getDate();
    saveTable(table, str(y)+"_"+str(m)+"_"+str(d)+"_"+str(h)+"_"+str(mn)+"_"+str(s)+".csv");
    //myPort.dispose();
    exit(); // Stops the program
  }
}

// Read transfered 2-byte based 12bit ADC data.
int byte2int(byte msb, byte lsb) { 
  //print("msb : "+msb+" lsb : "+lsb); //msb : Most significant Bit, lsb : Least Significant Bit
  int i = 0;
  i = msb << 8; // msb << 8 == xxxxxxxx 00000000, means plus 8bit on right side
  // int i = msb << 8 means 00000000 00000000 xxxxxxxx 00000000 
  i |= lsb & 0xFF; //bit or calculating, then assignment 0xFF = 255 = 11111111, |= means if 0 or 1, then should be 1 
  // lsb & 0xFF means masking. xxxxxxxx & 11111111 = xxxxxxxx 
  //if short type, or types having at least more than 2 bytes, & 0xFF is effective. so that's useless code  
  return i;
}

// Get current date and hours.
void getDate() {
  d = day();    // Values from 1 - 31
  m = month();  // Values from 1 - 12
  y = year();   // 2003, 2004, 2005, etc.
  s = second();  // Values from 0 - 59
  mn = minute();  // Values from 0 - 59
  h = hour();    // Values from 0 - 23
}

//first column in csv file.
String timeStamp(int MS) {
  float seconds = float(nfs((MS % 60000)/1000f, 2, 2));
  int minutes = (MS / (1000*60)) % 60;
  int hours = ((MS/(1000*60*60)) % 24);                      

  return hours+": " +minutes+ ": "+ seconds;
}

//converting data's range from 0 to 100.
int remap(int val)
{
  float v = map(val, 0, 2000, 0, 100);
  return (int)v;
}
