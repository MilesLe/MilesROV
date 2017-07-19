////////////////////////////////////////////////////////////////
//This is a test program for the temperature sensor DS18B20.
//Parts of this code will be used in the final ROV code. 
//Good tutorail: https://arduinomylifeup.com/arduino-ds18b20-temperature-sensor/ 
//If there are any questions about the code, you should probably watch 
//the tutorial video again. It has a lot of good extra information.
////////////////////////////////////////////////////////////////
#include <DallasTemperature.h>
#include <OneWire.h>

int temp_sensor = 5; //pin
float temp = 0;
OneWire oneWirePin(temp_sensor);
DallasTemperature sensors(&oneWirePin);

void setup() {
  Serial.begin(115200);
  sensors.begin();

  //This was orginally in loop(), so if there are problems MOVE IT BACK!
  Serial.print("Requesting Temp from sensor: ");
  sensors.requestTemperatures();
  Serial.println("DONE");
}

void loop() {
  temp = sensors.getTempCByIndex(0);
  Serial.print("Temperature: ");       Serial.println(temp);
}
