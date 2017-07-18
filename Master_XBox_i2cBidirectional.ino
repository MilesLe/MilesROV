//ID 002 - ID matches with other code counter part..
////////////////////////////////////////////////////////////////////////////////////
//README:
//12c Code from: https://www.youtube.com/watch?v=FGjyUSrfx-k
//ESC Code from: https://www.youtube.com/watch?v=DHDOAocEpqU
//
//This program recieves data from the Controller Arduino through i2c communication. Then it processes
//the data from joystick and button values into values such as thrust, vertical, and yaw. Then it has 
//algorithms that tell the ESCs their correct values based off the new values(thrust, vertical, yaw).
//The algorithms may not work at first and will need to be tweeked for perfection!!!
//
//********************************************************************************************************************
//**THIS VERSION IS DIFFERENT THAN "ROV2MASTER1" BECAUSE IT HAS DIFFERENT OUTCOMES FOR "FULL RIGHT" AND "FULL LEFT".**
//********************************************************************************************************************
//
//PROGRAM A SENSITIVITY VARIABLE!!!
//
//Arduino: in ROV.
//Connected to: ESCs, i2c wires.
//
//PAY ATTENTION TO ANYTHING WITH ASTRICS:******...
//
//PINS: ("<->" means the two pins on the two arduinos are connected)("|" means that the two different pins on the two arduinos DO NOT connect)
//  Type       |  Master-Micro  |  Type      |  Slave-Uno   
//  i2c SCL       D3           <-> i2c SCL      A5
//  i2c SDA       D2           <-> i2c SDA      A4
//  escR          D5            |  RJoyX        A3
//  escL          D6            |  RJoyY        A2
//  escV          D9            |  LJoyX        A1
//                              |  LJoyY        A0
//
//NOTE: Could work on Right Forward & Right Reverse and Left Forward & Left Reverse body codes. There are notes thier that describe an objective.
////////////////////////////////////////////////////////////////////////////////////
/*
  This Program is for a ROV with a Gopro Camera and a 3 thruster mobility system. (not for a submerine with a ballast.)
*/

#include <Wire.h>
#include <Servo.h>

//Input Data Variables:
  int yInputJoyL, xInputJoyL; //Input data
  int yInputJoyR, xInputJoyR; //Input data (on/off data)(not sure of the Output yet)(Should be 1 & 0)
  int button1, button2, button3, button4 = 0; //Input data.
//Main Data Variables: 
  int yaw              = 0; //A range: -180 -- 0 -- 180  //Ex. if range=90, then escL+power and escR-power, as long as min < escL&escR < max.     
  int thrust           = 0; //A range: -180 -- 0 -- 180
  int vertical         = 0; //A range: -180 -- 0 -- 180
//Parts Pin Variables: (need to assign real PWM pins)
  int escLPin          = 9; Servo escL; //PWM 
  int escRPin          = 10; Servo escR; //PWM
  int escVPin          = 5; Servo escV; //PWM
  int escValR;
  int escValL;
//Other Variables:
  const int id               = 5;
  const int messageLength    = 16; //if problems, revert to "10"
  const int minDeadZone      = 1475; //should be 1450
  const int maxDeadZone      = 1574; //should be 1549
  const int s                = 7; //*****Must be between: 0 and 8 inclusive. "s" changes the Range between "minESC" and "maxESC" to be Shorter: (increase s)The amount of movement of the joy affects the ESC signal LESS OR Longer:(decrease s) The amount of movement of the joy affects the ESC signal MORE.*****
  int minESC                 = 1000 + s*50;
  const int medESC           = 1500; 
  int maxESC                 = 2000 - s*50;

//Transmition to Slave:
  int messageLengthSlaveTransmit = 2;
  byte message[2];
  int temp;

  bool isTransmiter = false;

void setup() {
  Wire.begin();
  Serial.begin(115200);//regular: 9600. But use 115200.
  
  escL.attach(escLPin);
  escR.attach(escRPin);
  escV.attach(escVPin);
  //Arming ESCs: //So Important!
  escL.writeMicroseconds(1520);
  escR.writeMicroseconds(1520);
  escV.writeMicroseconds(1520);

  pinMode(LED_BUILTIN, OUTPUT);

  //Sensor setup (for transmition):
  pinMode(A0, INPUT);
}

void loop() {
  //Wire.beginTransmission(id);
  int available = Wire.requestFrom(id, messageLength);

  //if(!isTransmiter){
  //Read Slave Data: (Needs to correlate with the Data placement in the Array!) /////////////////////////////////////////////////////
          // -What this does is it scans through the array and after reading a piece of data it deletes it to read the next one.
          yInputJoyL  = (Wire.read() << 8) | (Wire.read()); //Index 0,1 
          xInputJoyL  = (Wire.read() << 8) | (Wire.read()); //Index 2,3
          yInputJoyR  = (Wire.read() << 8) | (Wire.read()); //Index 4,5
          xInputJoyR  = (Wire.read() << 8) | (Wire.read()); //Index 6,7
          button1     = (Wire.read() << 8) | (Wire.read()); //Index 8,9
          button2     = (Wire.read() << 8) | (Wire.read()); //Index 10,11
          button3     = (Wire.read() << 8) | (Wire.read()); //Index 12,13
          button4     = (Wire.read() << 8) | (Wire.read()); //Index 14,15

          //if(button2 == 1 && button1 == 0){
          //  isTransmiter = true;
          //  Serial.print("test");
          //}
  //}

  if(available == messageLength && button2 == 0 && button1 == 0) {
    //Blink LED to show it is connected:
          //digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
          Serial.print("Connected");
          
    //
      
    //Processing Data / Conversion of Input to Main Data / Conversion of Main to Output Data: /////////////////////////////////////////
          //Input to Servo/Print Data:
          //original: 0<->1023 not -32770<->32770
          yInputJoyL  = map(yInputJoyL, -32770, 32770, minESC, maxESC);          //--------PROGRAM SENSITIVITY---------// the movement of the   
          yInputJoyR  = map(yInputJoyR, -32770, 32770, minESC, maxESC);          //joy stick is more or less impacful on the ESC signals.
          xInputJoyR  = map(xInputJoyR, -32770, 32770, minESC, maxESC);          //Right now the Sensitivity = 1. (ratio, joy:ESC is 1:1)
                                                                           //******Do this by making the Range less than 1000-2000.**
                                                                           //EX. Make "minESC": 1250 and "maxESC": 1750. This cuts the
          //Input to Main Data:                                            //Range in half (1000-2000 = 1000 to 1250-1750 = 500).
          yaw             = xInputJoyR;  //Value: 1000-2000  //(L->R Increase)Left is 1000 and Right is 2000.
          vertical        = yInputJoyL;  //Value: 1000-2000  //^^^    //****Write Now, I am assuming 1000 correlates to Right and 2000 correlates to Left. May need to reverse based on tests of actual Joys.****
          thrust          = yInputJoyR;  //Value: 1000-2000  //^^^    
          
  //DO: ALGORITHMS/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      //ESC (vertical):
          escValR = thrust; 
          escValL = thrust;
          if(vertical < minDeadZone || vertical > maxDeadZone){
            escV.writeMicroseconds(vertical);
            Serial.print("   UP/DOWN"); 
            Serial.print("   escV = "); Serial.print(vertical);
          }
          else{
            escV.writeMicroseconds(medESC); //Stop movement
            Serial.print("   UP/DOWN"); Serial.print("   escV = "); Serial.print(medESC);
          }
      //ESCs (Thrust)(Yaw):
          //This makes the Right and Left ESCs equal to the "thrust" but then sub or add to their val according to the change on "yaw"(n).
          //****To Visualize this, draw a graph and make yaw equal to x-values and thrust equal to y-values. It should surround the origin and create a symmetrical shape. There should be 1 section and 2 half sections per 
          //   Quadrant of the Graph (in whole graph there should be 8 sections). Sections should be (clock wise): Forward, Right Forward, Full Right, Right Reverse, Reverse, Left Reverse, Full Left, Left Forward.*****
          int nL = yaw - minESC; //yaw - 1000. //"nL" is the amount of change in yaw from the midpoint.
          int nR = (maxESC - minESC) - nL; //Inverse direction of nL.
          
          //If in DeadZone, then stop: 
          if((thrust > minDeadZone && thrust < maxDeadZone) && (yaw > minDeadZone && yaw < maxDeadZone)){ 
            escL.writeMicroseconds(medESC);
            escR.writeMicroseconds(medESC); 
            Serial.print("   Off"); 
            Serial.print("   escR = "); Serial.print(medESC);
            Serial.print("   escl = "); Serial.print(medESC);
          }                                                                           //Vars to Use: minDeadZone/maxDeadZone, thrust, n, yaw, minESC/maxESC/medESC
          //Right:
          else if(yaw < minDeadZone){ //yaw > 1550.    *******IF the joy Left and Right data is flipped, then switch this "if statement" with the Left one.********
                //Forward: yaw < 1450 and thrust >= 1550 (Doesn't include full forward and full Right)(Includes area between full forward and full Right) 
                if(thrust >= maxDeadZone){
                  escL.writeMicroseconds(escValL);
                  escR.writeMicroseconds(medESC);
                  //****Change Code Here that changes escR val to make ROV turn forwards while still moving forwards. Now it is just turning and not moving.****
                  Serial.print("   Right Forward");
                  Serial.print("   escR = "); Serial.print(medESC);
                  Serial.print("   escL = "); Serial.print(escValL);
                }
                //Full Left: yaw < 1450 and 1450 < thrust < 1550 
                else if(thrust > minDeadZone && thrust < maxDeadZone){
                  escR.writeMicroseconds(medESC); 
                  escL.writeMicroseconds(medESC + (nR - ((maxESC - minESC) / 2)));
                  Serial.print("   Full Right");
                  Serial.print("   escR = "); Serial.print(medESC);
                  Serial.print("   escL = "); Serial.print(medESC + (nR - ((maxESC - minESC) / 2)));
                }
                //Reverse: yaw < 1450 and thrust <= 1450 (Doesn't include full reverse and full Right)(Includes area between full reverse and full Right) 
                else if(thrust <= minDeadZone){ 
                  escR.writeMicroseconds(escValR);
                  escL.writeMicroseconds(medESC);
                  //****Add Code Here that changes escL val to make ROV turn reverse while still moving reverse. Now it is just turning and not moving.****
                  Serial.print("   Right Reverse");
                  Serial.print("   escR = "); Serial.print(escValR);
                  Serial.print("   escL = "); Serial.print(medESC);
                }
              }
          //Left: 
          else if(yaw > maxDeadZone){ //yaw < 1450.
                //Forward: yaw < 1450 and thrust >= 1550 (Doesn't include full forward and full Left)(Includes area between full forward and full Left) 
                if(thrust >= maxDeadZone){  
                  escL.writeMicroseconds(medESC);
                  escR.writeMicroseconds(escValR);
                  //****Change Code Here that changes escL val to make ROV turn forwards while still moving forwards. Now it is just turning and not moving.****
                  Serial.print("   Left Forward");
                  Serial.print("   escR = "); Serial.print(escValR);
                  Serial.print("   escL = "); Serial.print(medESC);
                }
                //Full Left: yaw < 1450 and 1450 < thrust < 1550 
                else if(thrust > minDeadZone && thrust < maxDeadZone){
                  escR.writeMicroseconds(medESC + (nL - ((maxESC - minESC) / 2))); //(maxESC - minESC) / 2)) == 500.
                  escL.writeMicroseconds(medESC);
                  Serial.print("   Full Left");
                  Serial.print("   escR = "); Serial.print(medESC + (nL - ((maxESC - minESC) / 2)));
                  Serial.print("   escL = "); Serial.print(medESC);
                }
                //Reverse: yaw < 1450 and thrust <= 1450 (Doesn't include full reverse and full Left)(Includes area between full reverse and full Left) 
                else if(thrust <= minDeadZone){ 
                  escR.writeMicroseconds(medESC);
                  escL.writeMicroseconds(escValL);
                  //****Add Code Here that changes escR val to make ROV turn reverse while still moving reverse. Now it is just turning and not moving.****
                  Serial.print("   Left Reverse");
                  Serial.print("   escR = "); Serial.print(medESC);
                  Serial.print("   escL = "); Serial.print(escValL);
                }
          }
          //Med (Straight or Reverse):
          else{ //1450 < yaw < 1550.
            escR.writeMicroseconds(escValR);
            escL.writeMicroseconds(escValL); 
            Serial.print("   Forwards/Reverse");
            Serial.print("   escR = "); Serial.print(escValR);
            Serial.print("   escL = "); Serial.print(escValL);
          }

    //Print: //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
          Serial.print("  Button1 = ");       Serial.print(button1); //CLICK !!! //Maybe for the LIGHTS?
          Serial.print("  Button2 = ");       Serial.print(button2); //PRESS !!!
          Serial.print("  Button3 = ");       Serial.print(button3); //CLICK !!! //Maybe for RESET function?
          Serial.print("  Button4 = ");       Serial.print(button4); //PRESS !!!
          //Serial.print("  |  ");
          
          //Serial.print("  JoyLeftY = ");      Serial.print(yInputJoyL);
          //Serial.print("  Vertical = ");      Serial.print(vertical);
          //Serial.print("  |  ");
          
          //Serial.print("  JoyRightX = ");     Serial.print(xInputJoyR);
          //Serial.print("  JoyRightY = ");     Serial.print(yInputJoyR);
          //Serial.print("  Yaw = ");           Serial.print(yaw);
          //Serial.print("  Thrust = ");        Serial.print(thrust);

          //Serial.print("  nR = ");            Serial.print(nR);
          //Serial.print("  nL = ");            Serial.print(nL); 
          
          //Serial.println();  <-- Got moved to under the print statement for "temp" but has the same purpose: to make a new line.
  }
  else{
    //Default Sub Mode / Failsafe:
    Serial.print("Signal Disconnected");
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW

    //Maybe program for everything to shut down so nothing can break or it can't run into anything:
    escR.writeMicroseconds(medESC);
    escL.writeMicroseconds(medESC);
    escV.writeMicroseconds(medESC);
  }
  
  //Wire.endTransmission();  <--- Moved to end.
  //delay(50);               <---

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////// Above is Receiver ////////// Below is Transmitter ////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //if(isTransmiter){
  //Temp Sensor:
    temp = analogRead(A0);
    Serial.print("   Temp = ");        Serial.print(temp);

    Wire.beginTransmission(id);
    message[0] = highByte(temp);
    message[1] = lowByte(temp);

    Wire.write(message, messageLengthSlaveTransmit);
    //delay(500); 
    //isTransmiter = false;
  //}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
  //Serial.print(" isTransmiter = ");    Serial.print(isTransmiter);
  Serial.println();
  
  Wire.endTransmission();
  delay(50);
}

