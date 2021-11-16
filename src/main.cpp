
/* Pulse counter module - Example
   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <Arduino.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/ledc.h"
#include "driver/gpio.h"
#include "driver/pcnt.h"
#include "esp_attr.h"
#include "esp_log.h"

// the number of the LED pin
const int pwmFanPin1 = 21;  // 21 corresponds to GPIO21?

// setting PWM properties
const int freq = 50200;
const int ledChannel = 0;
const int resolution = 4;

int lower_limit = -35000;
int upper_limit =  500;
int threshold_value = -50;

pcnt_isr_handle_t user_isr_handle = NULL; //user's ISR service handle

/* A sample structure to pass events from the PCNT
 * interrupt handler to the main program.
 */
typedef struct {
    int unit;  // the PCNT unit that originated an interrupt
    uint32_t status; // information on the event type that caused the interrupt
    unsigned long timeStamp; // The time the event occured
} pcnt_evt_t;

/* Decode what PCNT's unit originated an interrupt
 * and pass this information together with the event type
 * and timestamp to the main program using a queue.
 */
//static void IRAM_ATTR pcnt_intr_handler(void *arg)
//{
//}


/* Initialize PCNT functions for one channel:
 *  - configure and initialize PCNT with pos-edge counting 
 *  - set up the input filter
 *  - set up the counter events to watch
 * Variables:
 * UNIT - Pulse Counter #, INPUT_SIG - Signal Input Pin, INPUT_CTRL - Control Input Pin,
 * Channel - Unit input channel, H_LIM - High Limit, L_LIM - Low Limit,
 * THRESH1 - configurable limit 1, THRESH0 - configurable limit 2, 
 */
void pcnt_init_channel(pcnt_unit_t PCNT_UNIT,int PCNT_INPUT_SIG_IO , int PCNT_INPUT_CTRL_IO = PCNT_PIN_NOT_USED,pcnt_channel_t PCNT_CHANNEL = PCNT_CHANNEL_0, int PCNT_H_LIM_VAL = 19, int PCNT_L_LIM_VAL = -20, int PCNT_THRESH1_VAL = 50, int PCNT_THRESH0_VAL = -50 ) {
    /* Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config; 
        // Set PCNT input signal and control GPIOs
        pcnt_config.pulse_gpio_num = PCNT_INPUT_SIG_IO;
        pcnt_config.ctrl_gpio_num = PCNT_INPUT_CTRL_IO;
        pcnt_config.channel = PCNT_CHANNEL;
        pcnt_config.unit = PCNT_UNIT;
        // What to do on the positive / negative edge of pulse input?
        pcnt_config.pos_mode = PCNT_COUNT_INC;   // Count up on the positive edge
        pcnt_config.neg_mode = PCNT_COUNT_DIS;   // Keep the counter value on the negative edge
        // What to do when control input is low or high?
        pcnt_config.lctrl_mode = PCNT_MODE_REVERSE; // Reverse counting direction if low
        pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Keep the primary counter mode if high
        // Set the maximum and minimum limit values to watch
        pcnt_config.counter_h_lim = PCNT_H_LIM_VAL;
        pcnt_config.counter_l_lim = PCNT_L_LIM_VAL;
    
    /* Initialize PCNT unit */
    pcnt_unit_config(&pcnt_config);
    /* Configure and enable the input filter */
    pcnt_set_filter_value(PCNT_UNIT, 50);
    pcnt_filter_enable(PCNT_UNIT);

    /* Set threshold 0 and 1 values and enable events to watch */
    // pcnt_set_event_value(PCNT_UNIT, PCNT_EVT_THRES_1, PCNT_THRESH1_VAL);
    // pcnt_event_enable(PCNT_UNIT, PCNT_EVT_THRES_1);
    // pcnt_set_event_value(PCNT_UNIT, PCNT_EVT_THRES_0, PCNT_THRESH0_VAL);
    // pcnt_event_enable(PCNT_UNIT, PCNT_EVT_THRES_0);
    /* Enable events on zero, maximum and minimum limit values */
    pcnt_event_enable(PCNT_UNIT, PCNT_EVT_ZERO);
    pcnt_event_enable(PCNT_UNIT, PCNT_EVT_H_LIM);
    // pcnt_event_enable(PCNT_UNIT, PCNT_EVT_L_LIM);

    /* Initialize PCNT's counter */
    pcnt_counter_pause(PCNT_UNIT);
    pcnt_counter_clear(PCNT_UNIT);
    /* Register ISR handler and enable interrupts for PCNT unit */
    //pcnt_isr_register(pcnt_intr_handler, NULL, 0, &user_isr_handle);
    pcnt_intr_enable(PCNT_UNIT);

    /* Everything is set up, now go to counting */
    pcnt_counter_resume(PCNT_UNIT);

}

float calculate_frequency(int number_of_pulses, int start_time, int stop_time){
  return 1000.0*number_of_pulses/(stop_time-start_time);
}

void setup() {
  Serial.begin(115200);

  ledcSetup(ledChannel,210000,resolution);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(21, ledChannel);
  ledcWrite(ledChannel, 10); // 1Hz PWM with duty cycle of 10/255
  Serial.println("setup complete");
}

void loop()
{
  /* Initialize PCNT event queue and PCNT functions */
  pcnt_init_channel(PCNT_UNIT_0,4, PCNT_PIN_NOT_USED, PCNT_CHANNEL_0, upper_limit, lower_limit); // Initialize Unit 0 to pin 4

  Serial.println("starting infinite loop");

  int loops_for_print = 0;
  unsigned long last_time_Stamp = 0;
  int16_t count = 0;
  float frequency = 0;
  for(;;){
    pcnt_get_counter_value(PCNT_UNIT_0, &count);
    if(count<= -30000) {
      frequency = calculate_frequency(count, micros(), last_time_Stamp);
      last_time_Stamp = micros();
      pcnt_counter_clear(PCNT_UNIT_0);
    }
    if(loops_for_print >= (100000)){
    Serial.printf("\n Counter %0.3f kHz", frequency);
    loops_for_print = 0;
    }
    loops_for_print++;

    

  }
    if(user_isr_handle) {
    //Free the ISR service handle.
    esp_intr_free(user_isr_handle);
    user_isr_handle = NULL;
  }
}