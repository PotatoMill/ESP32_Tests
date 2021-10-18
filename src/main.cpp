#include <Arduino.h>
float touchVal = 43;
int touchValRaw = 43;
float triggerVal = 53.3;

void setup() {
  Serial.begin(115200);
}

void loop() {
  touchValRaw = touchRead(32);

  Serial.print(touchVal);
  Serial.print("  ");
  Serial.print(touchValRaw);
  Serial.print("  ");
  if(touchVal < triggerVal) Serial.print("WATER!");
  Serial.println("  ");

  if(touchValRaw < touchVal - 10) touchValRaw = touchVal; //sletter hvis det er for mye avvik
  touchVal = touchVal * 0.9 + touchValRaw * 0.1;
  delay(50);
}