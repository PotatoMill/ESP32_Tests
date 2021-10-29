#include <Arduino.h>
float touchVal = 0;
int touchValRaw = 0;
float triggerVal1 = 42;
float triggerVal2 = 41.5;
float triggerVal3 = 41;
#define LED_PIN_1 26
#define LED_PIN_2 25
#define LED_PIN_3 14


void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  digitalWrite(LED_PIN_1, HIGH);
  digitalWrite(LED_PIN_2, HIGH);
  digitalWrite(LED_PIN_3, HIGH); // følger rekkefølge på pinnene
  delay(500);
}

void loop()
{
  touchValRaw = touchRead(32);

  Serial.print(touchVal);
  Serial.print("  ");
  Serial.print(touchValRaw);
  Serial.print("  ");
  if (touchVal < triggerVal1)
  {
    Serial.print("WATER!");
    digitalWrite(LED_PIN_1, HIGH);
  }
  else
    digitalWrite(LED_PIN_1, LOW);
  Serial.print("  ");

  if (touchVal < triggerVal2)
  {
    Serial.print("WATER!");
    digitalWrite(LED_PIN_2, HIGH);
  }
  else
    digitalWrite(LED_PIN_2, LOW);
  Serial.print("  ");

  if (touchVal < triggerVal3)
  {
    Serial.print("WATER!");
    digitalWrite(LED_PIN_3, HIGH);
  }
  else
    digitalWrite(LED_PIN_3, LOW);
  Serial.println("  ");

  if (touchValRaw < touchVal - 10)
    touchValRaw = touchVal; // sletter hvis det er for mye avvik
  touchVal = touchVal * 0.9 + touchValRaw * 0.1;
  delay(50);
}