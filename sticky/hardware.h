#pragma once
#include "driver/gpio.h"
#include "esp_sleep.h"
namespace sticky {
RTC_DATA_ATTR uint32_t last_time_sync=0;
inline void output(int pin,int level) { gpio_set_direction(gpio_num_t(pin),GPIO_MODE_OUTPUT); gpio_set_level(gpio_num_t(pin),level); }
inline void hardware_boot() {
 gpio_deep_sleep_hold_dis();
 for(int p:{45,46,47,42,41,38,10,48,39}) gpio_hold_dis(gpio_num_t(p));
 output(45,1); output(46,1); // Power latch: never drop these to save battery.
 output(38,0); output(10,0); output(48,0); // Mic, SD, buzzer unused.
 output(39,0); // Charger enable is active low.
}
inline void hardware_sleep() {
 // Called after the display shutdown hook; retain supply latch across deep sleep.
 output(45,1); output(46,1);
 for(int p:{47,42,41,38,10,48}) output(p,0);
 output(39,0);
 for(int p:{45,46,47,42,41,38,10,48,39}) gpio_hold_en(gpio_num_t(p));
 gpio_deep_sleep_hold_en();
}
}
