#include "settings.h"
void setup() {
  pinMode(6, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int i = 0; i < max_power; i++){
    analogWrite(6, i);
    delay(10);
  }
  delay(10000);
  for(int i = 0; i < max_power; i++){
    analogWrite(6, max_power - i);
    delay(10);
  }
}
