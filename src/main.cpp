//simple code that fades a led up and down using pwm

#include <Arduino.h>

#define PWM_CHANNEL0 0
#define PWM_CHANNEL0_PIN 5
#define PWM_CHANNEL0_RESOLUTION 8
#define PWM_CHANNEL0_FREQUENCY 10000

int pwm_channel0_duty_cycle = 0;
bool goingUp = true;

void setup() {
  ledcAttachPin(PWM_CHANNEL0_PIN, PWM_CHANNEL0);
  ledcSetup(PWM_CHANNEL0, PWM_CHANNEL0_FREQUENCY, PWM_CHANNEL0_RESOLUTION);
}

void loop() {
  ledcWrite(PWM_CHANNEL0, pwm_channel0_duty_cycle);

  if(goingUp) pwm_channel0_duty_cycle++;
  else pwm_channel0_duty_cycle--;

  if (pwm_channel0_duty_cycle>=255) goingUp=false;
  if (pwm_channel0_duty_cycle<=0) goingUp=true;

  delay(1);
}