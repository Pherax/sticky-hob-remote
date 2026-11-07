#pragma once
#include "driver/gpio.h"
#include "driver/rtc_io.h"
#include "esp_sleep.h"
#include "power.h"
namespace sticky {
RTC_DATA_ATTR uint32_t last_time_sync=0;
// Survive deep sleep: when we went to sleep (RTC clock seconds) and the spurious-wake guard.
RTC_DATA_ATTR uint32_t slept_at=0;
RTC_DATA_ATTR WakeGuard wake_guard;
// Last real wake (tap, grey key, power on) for HA; clock-tick timer wakes do not count.
RTC_DATA_ATTR uint32_t last_wake=0;
RTC_DATA_ATTR uint8_t last_wake_reason=0;
// Sleep diagnostics since the last real wake: clock-tick wakes and total awake time (ms).
// At a real wake they move to the last_* values, which are reported to HA on every boot.
RTC_DATA_ATTR uint32_t tick_wakes=0, tick_awake_ms=0, last_tick_wakes=0, last_tick_awake_s=0;
static bool charging=false;
constexpr int TOUCH_INT_PIN=21, TOUCH_RESET_PIN=41, GREY_KEY_PIN=4;
inline void output(int pin,int level) { gpio_set_direction(gpio_num_t(pin),GPIO_MODE_OUTPUT); gpio_set_level(gpio_num_t(pin),level); }
inline void hardware_boot() {
 gpio_deep_sleep_hold_dis();
 for(int p:{45,46,47,42,41,38,10,48,39}) gpio_hold_dis(gpio_num_t(p));
 rtc_gpio_deinit(gpio_num_t(TOUCH_INT_PIN));
 output(45,1); output(46,1); // Power latch: never drop these to save battery.
 output(38,0); output(10,0); output(48,0); // Mic, SD, buzzer unused.
 output(39,0); // Charger enable is active low.
}
// keep_touch: leave the touch controller out of reset so a tap can wake the ESP.
inline void hardware_sleep(bool keep_touch) {
 // Called after the display shutdown hook; retain supply latch across deep sleep.
 output(45,1); output(46,1);
 for(int p:{47,42,38,10,48}) output(p,0);
 output(TOUCH_RESET_PIN,keep_touch?1:0);
 output(39,0);
 for(int p:{45,46,47,42,41,38,10,48,39}) gpio_hold_en(gpio_num_t(p));
 gpio_deep_sleep_hold_en();
}
// Wake sources for the next deep sleep: always the grey key; the touch interrupt only in
// SLEEP_TAP mode and only if its line is idle (high) right now, else it would wake at once.
inline uint64_t wake_mask(int power_mode) {
 uint64_t mask=1ULL<<GREY_KEY_PIN;
 if(power_mode==POWER_SLEEP_TAP && gpio_get_level(gpio_num_t(TOUCH_INT_PIN))==1) {
   rtc_gpio_pullup_en(gpio_num_t(TOUCH_INT_PIN));
   rtc_gpio_pulldown_dis(gpio_num_t(TOUCH_INT_PIN));
   esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
   mask|=1ULL<<TOUCH_INT_PIN;
 }
 return mask;
}
inline bool woke_by_touch() {
 return esp_sleep_get_wakeup_cause()==ESP_SLEEP_WAKEUP_EXT1 && (esp_sleep_get_ext1_wakeup_status() & (1ULL<<TOUCH_INT_PIN));
}
}
