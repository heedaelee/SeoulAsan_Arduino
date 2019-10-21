/**************************************************************************
 * Update Note
 *  FootViewer ver1.0 (2019.9.09)
 *    - Read data through serial comm (ble or serial)
 **************************************************************************/

import processing.serial.*;

// Set-up overall Serial comm
Serial  myPort;   // Define serial comm
int lf = 0xEAEA;      // ASCII linefeed (only Single byte works)
byte[] inBytes = new byte[68];
int[][] data = new int[4][8];

/*
// Set-up text file recording.
PrintWriter output;
String path = "/Users/David/Desktop/footMonitoring.txt";
*/


void setup() { 
  size(420, 820);
  frameRate(100);

/*
  //Create text file.
  output = createWriter(path); 
*/
  println(Serial.list());              // Show up all possible serial ports.
  String portName = Serial.list()[0];  // Set specific serial port to comm.

  myPort = new Serial(this, portName, 115200);
  myPort.clear();
  myPort.bufferUntil(lf); 
}

void draw(){

  
  background(100);
  
  //Draw rectangular for sensor indication
  for(int i=0; i<8; i++){
    for(int j=0; j<4; j++){
      fill((data[j][i]-300)/8,0,0);
      rect(20+j*100, 20+i*100, 80, 80, 7);
    }
  }
}

// Receive data from Arduino and analyze it according to data form.
void serialEvent(Serial myPort) {

  // Trim single data packet
  if(myPort.available()>10){
    if(myPort.read() == 0xEA){
      inBytes = myPort.readBytes();
    }else{
      myPort.clear();
    }
  }

  try {
    for(int i=0; i<8; i++){
      for(int j=0; j<4; j++){
        data[j][i] = byte2int(inBytes[(i*4+j)*2+3],inBytes[(i*4+j)*2+2]);
      }
    }
  }

  catch (Exception e) {
    println("Caught Exception");
  }

}

void keyPressed() {
  if (key == 27){ // 27 means ESC key
//    output.flush(); // Writes the remaining data to the file
//    output.close(); // Finishes the file
    exit(); // Stops the program
  }
}

int byte2int(byte msb, byte lsb){ 
    int i = 0;
    i = msb << 8;
    i |= lsb & 0xFF;
  return i;    
}
