//////////////////////////////////////////////////////////////////////////////////////////////
//Code from: https://www.youtube.com/watch?v=FGjyUSrfx-k
//This Class sends data to the Master class.
//Note: This slave class also has to recieve and print battery life data from the Master class.
//
//Arduino: in Controller.
//Connected to: XBox Controller, i2c wires.
//
//i2c pins: 
//All pins and their corresponding pins are listed in "ROVMaster1" file.
//
//**************************************************************************************************************************
//**THIS VERSION IS DIFFERENT THAN "ROV2Slave1" BECAUSE IT TAKES JOY INPUT FROM X_BOX CONTROLLER INSTEAD OF STANDARD JOYS.**
//**************************************************************************************************************************
///////////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>

int leftJoyX;
int leftJoyY;
int rightJoyX;
int rightJoyY;
byte button1; int val1 = 0;
byte button2; int val2 = 0;
byte button3; int val3 = 0;
byte button4; int val4 = 0;

int id = 5;
int messageLength = 12; 
byte messageArray[12]; //[!] Can't put "messageLength" in for "num". [!]  

void setup() {
  Wire.begin(id);
  Wire.onRequest(requestEvent);

  Serial.begin(9600);//regular: 9600.
}

void loop() {
  //Sensor Read:
  leftJoyY = analogRead(0);    //pin A0
  leftJoyX = analogRead(1);    //pin A1
  rightJoyY = analogRead(2);    //pin A2
  rightJoyX = analogRead(3);    //pin A3
  button1    = val1;  
  button2    = val2; 
  button3    = val3;
  button4    = val4; 
}

void requestEvent() {
  //JoyL Array Assignment:
  messageArray[0]  = highByte(leftJoyY);
  messageArray[1]  = lowByte(leftJoyY);
  messageArray[2]  = highByte(leftJoyX);
  messageArray[3]  = lowByte(leftJoyX);
  //JoyR Array Assignment:
  messageArray[4]  = highByte(rightJoyY);
  messageArray[5]  = lowByte(rightJoyY);
  messageArray[6]  = highByte(rightJoyX);
  messageArray[7]  = lowByte(rightJoyX);
  //ElevBut Array Assignment:
  messageArray[8]  = button1;
  messageArray[9]  = button2;
  messageArray[10] = button3;
  messageArray[11] = button4;
  
  //Message Send:
  Wire.write(messageArray, messageLength); //change later to only send Data: Roll/Pitch/Yaw/Elevation
}
