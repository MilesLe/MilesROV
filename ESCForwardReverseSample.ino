#include <Servo.h>

/*ONLY for Afro ESCs With New 2015 SimonK Firmware. Code is different for ESCs with older Firmware.*/

/*
Milisecond input Constants:
- Forward Max = 2000   //Realy only need to go to 1650.
- Forward Min =  1550 
- No power here = 1450 < n < 1549. 
- Reverse Min = 1450
- Reverse Max = 1000   //Realy only need to go to 1350.
*/

Servo SimonKESC;
int ESCmax = 1990; //less that 2000 to be safe. 
int ESCmin = 1010;
int initial = 1500; //safe zone. Motor stops. (if this num does nothing try: 1660)
int n = initial; 
int s = 10; 

void setup(){
  Serial.begin(9600);
  SimonKESC.attach(3); 
  SimonKESC.writeMicroseconds(1520); //1220 to arm in microseconds for Forward ESC SimonK Firmware.
}

void loop(){
  //For Keybaord: (1 = n-s, 2 = n+s, other = n=0)
    while(Serial.available() == 0);
    int val = Serial.read(); //int because the ASCII characters will equal an int.
    
    if(val == 49 && n >= (ESCmin + s)){ //will make min output equal to ESCmin.
      n = n - s;
    }
    if(val == 50 && n <= (ESCmax - s)){ //will make max output equal to ESCmax.
      n = n + s;
    }
    if(val != 50 && val != 49){
      n = initial;
    }
    
    Serial.print("n = "); Serial.println(n);
    SimonKESC.writeMicroseconds(n);
}
