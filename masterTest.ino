#include <Wire.h>

struct warningExample {
  char type;
  int distance;
};
int randNumber, randType;
warningExample example;
bool waiting;

void setup() {
  Wire.begin(); 
  Wire.onReceive(receiveEvent);
  randomSeed(analogRead(0));
}

void loop() {
  Wire.beginTransmission(8);
  randType = random(1,3);
  switch(randType)  {
    case 1:
      example = {'L',randNumber};
    break;
    case 2:
      example = {'H',randNumber};
    break;
    case 3:
      example = {'D',randNumber};
    break;  
  }
  Wire.write(example.type);
  Wire.endTransmission();
  waiting = true;
  while (waiting) {
    
  }
  Wire.requestFrom(8, 22);
  while(Wire.available()) {
    char c = Wire.read();
    Serial.print(c); 
  }
  Serial.println("");
}

void receiveEvent(int inputLength) {
  waiting = false;
}

