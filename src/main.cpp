#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_attr.h"
#include "esp_log.h"

void pcnt_init_channel(pcnt_unit_t PCNT_UNIT,int PCNT_INPUT_SIG_IO , int PCNT_INPUT_CTRL_IO = PCNT_PIN_NOT_USED,pcnt_channel_t PCNT_CHANNEL = PCNT_CHANNEL_0) {
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config; 
        // Set PCNT input signal and control GPIOs
        pcnt_config.pulse_gpio_num = PCNT_INPUT_SIG_IO;
        pcnt_config.ctrl_gpio_num = PCNT_INPUT_CTRL_IO;
        pcnt_config.channel = PCNT_CHANNEL;
        pcnt_config.unit = PCNT_UNIT;
        pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
        pcnt_config.neg_mode = PCNT_COUNT_DIS;   // Keep the counter value on the negative edge
        pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
        pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Keep the primary counter mode if high
    
    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);
    pcnt_set_filter_value(PCNT_UNIT, 50); //input filter
    pcnt_filter_enable(PCNT_UNIT);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);
    pcnt_counter_resume(PCNT_UNIT);
}

float calculate_frequency(int number_of_pulses, int start_time, int stop_time){
  return 1000.0*number_of_pulses/(stop_time-start_time);
}

void setup() {
  Serial.begin(115200);
}

void loop()
{
  pcnt_init_channel(PCNT_UNIT_0,4, PCNT_PIN_NOT_USED, PCNT_CHANNEL_0); // Initialize Unit 0 to pin 4

  int loops_for_print = 0;
  unsigned long last_time_Stamp = 0;
  int16_t count = 0;
  float frequency = 174; //khz
  float average_frequency = 174; //khz
  float frequency_difference = 0;
  for(;;){
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    if(count<= -30000) {
      frequency = calculate_frequency(count, micros(), last_time_Stamp);
      last_time_Stamp = micros();
      pcnt_counter_clear(PCNT_UNIT_0);
      average_frequency = average_frequency * 0.97 + frequency * 0.03;
      frequency_difference = average_frequency - frequency;
    }
    if(loops_for_print >= (100000)){
    Serial.printf("%0.3f %0.3f %0.3f\n", frequency, average_frequency, frequency_difference);
    loops_for_print = 0;
    }
    loops_for_print++;
  }
}