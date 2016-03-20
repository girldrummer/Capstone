#include <Wire.h>

bool low, high, drop;

void setup() {
  Wire.begin(8);       
  Wire.onReceive(receiveEvent); 
  Serial.begin(9600);          
}

void loop() {
  delay(100);
}

void receiveEvent(int inputLength) {
    char c = Wire.read(); // receive byte as a character
    switch(c)  {
      case ('L'):
          low = true;
      break;
      case ('H'):
          high = true;
      break;
      case ('D'):
          drop = true;
      break;
    }
  Wire.beginTransmission(8);
  Wire.write(1);
  Wire.endTransmission();
}

void requestEvent() {
  if(low) {
    Wire.write("low  obstacle detected");
  }
  if(high) {
    Wire.write("high obstacle detected");
  }
  if(drop) {
    Wire.write("drop obstacle detected");
  }
}

