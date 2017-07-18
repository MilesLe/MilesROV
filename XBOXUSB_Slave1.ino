//ID 001 - ID matches with other code counter part.
/////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 Example sketch for the Xbox 360 USB library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */
 /////////////////////////////////////////////////////////////////////////////////////////////////////
 /////////////////////////////////////////////////////////////////////////////////////////////////////
 //THIS CODE IS A DERIVATIVE OF THE CODE FROM "ROV2Slave2XBox" but modified for the Xbox controller.
 /////////////////////////////////////////////////////////////////////////////////////////////////////

#include <XBOXUSB.h>
#include <Wire.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

USB Usb;
XBOXUSB Xbox(&Usb);

int leftJoyX;
int leftJoyY;
int rightJoyX;
int rightJoyY;
int button1 = 0; 
int button2 = 0; 
int button3 = 0; 
int button4 = 0; 

int id = 5;
int messageLength = 16; 
byte messageArray[16];

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nXBOX USB Library Started"));

  Wire.begin(id);
  Wire.onRequest(requestEvent);
}

void loop() {
  Usb.Task();
  if (Xbox.Xbox360Connected) {

    if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500 || Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500 || Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500 || Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
      if (Xbox.getAnalogHat(LeftHatX) > 7500 || Xbox.getAnalogHat(LeftHatX) < -7500) {
        Serial.print(F("LeftHatX: "));
        Serial.print(Xbox.getAnalogHat(LeftHatX));
        Serial.print("\t");
        leftJoyX = Xbox.getAnalogHat(LeftHatX);
      }
      if (Xbox.getAnalogHat(LeftHatY) > 7500 || Xbox.getAnalogHat(LeftHatY) < -7500) {
        Serial.print(F("LeftHatY: "));
        Serial.print(Xbox.getAnalogHat(LeftHatY));
        Serial.print("\t");
        leftJoyY = Xbox.getAnalogHat(LeftHatY);
      }
      if (Xbox.getAnalogHat(RightHatX) > 7500 || Xbox.getAnalogHat(RightHatX) < -7500) {
        Serial.print(F("RightHatX: "));
        Serial.print(Xbox.getAnalogHat(RightHatX));
        Serial.print("\t");
        rightJoyX = Xbox.getAnalogHat(RightHatX);
      }
      if (Xbox.getAnalogHat(RightHatY) > 7500 || Xbox.getAnalogHat(RightHatY) < -7500) {
        Serial.print(F("RightHatY: "));
        Serial.print(Xbox.getAnalogHat(RightHatY));
        rightJoyY = Xbox.getAnalogHat(RightHatY);
      }
      Serial.println();

      //Vibrating Intensity for horizontal thrusters:
      int numf = abs(map(rightJoyY, -32780, 32770, -200, 200));
      int numk = abs(map(rightJoyX, -32780, 32770, -200, 200));
      int numt = abs(map(leftJoyY, -32780, 32770, -225, 225));
      Serial.print(numf); Serial.print("  "); Serial.print(numk); Serial.print("  "); Serial.print(numt); Serial.print("  ");
      if(numf < 255 && numf > 70){
        Xbox.setRumbleOn(0, numf); //(Left: 0-255 , Right: 0-255)
        if(numt < 255 && numt > 70){
          Xbox.setRumbleOn(numt, numf);
        }
      } 
      else if(numk < 255 && numk > 70){
        Xbox.setRumbleOn(0, numk);
        if(numt < 255 && numt > 70){
          Xbox.setRumbleOn(numt, numf);
        }
      }
      else if(numt < 255 && numt > 80){
          Xbox.setRumbleOn(numt, 0);
      }
      else{ Xbox.setRumbleOn(0,0); }      
      
    }

    if (Xbox.getButtonClick(A)){       /////////// Button Click ///////////
      Serial.println(F("A"));
      if (button1 == 0){
        button1 = 1;
      }
      else if (button1 == 1){
        button1 = 0;
      }
      Serial.println(button1); 
    } //else{button1 = 0; Serial.println(button1);}

    if (Xbox.getButtonClick(X)){
      Serial.println(F("X"));
      if (button3 == 0){
        button3 = 1;
      }
      else if (button3 == 1){
        button3 = 0;
      }
      Serial.println(button3);
    } //else{button3 = 0; Serial.println(button3);}

    
    if (Xbox.getButtonPress(B)){       /////////// Button Press ///////////
      Serial.println(F("B"));          // The print statment will not work unless you comment out
      button2 = 1;                     // the other button print statments.
      Serial.println(button2);     
    } else{button2 = 0; Serial.println(button2);}    

    if (Xbox.getButtonPress(Y)){
      Serial.println(F("Y"));
      button4 = 1;
      Serial.println(button4);
    } else{button4 = 0; Serial.println(button4);}

  }
  delay(1);
}

//TRY THIS: Change button1 to int, then test if the "if" statement changes variable value, then 
//          send the button1 variable as a byte by doing the highByte/lowByte thing.

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
  //Button Array Assignment:
  messageArray[8]  = highByte(button1);
  messageArray[9]  = lowByte(button1);
  messageArray[10] = highByte(button2);
  messageArray[11] = lowByte(button2);
  messageArray[12]  = highByte(button3);
  messageArray[13]  = lowByte(button3);
  messageArray[14] = highByte(button4);
  messageArray[15] = lowByte(button4);
  
  //Message Send:
  Wire.write(messageArray, messageLength); //change later to only send Data: Roll/Pitch/Yaw/Elevation
}
