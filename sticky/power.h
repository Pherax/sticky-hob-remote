#pragma once
#include <cstdint>
namespace sticky {
// Standby power modes (Settings > power row cycles through them).
//   AWAKE         CPU and touch stay on: tap the clock to open. About a day per charge.
//   SLEEP_TAP     deep sleep; a tap on the screen or the grey key wakes it.
//   SLEEP_BUTTON  deep sleep; only the grey key wakes it. Lowest consumption.
enum PowerMode : int { POWER_AWAKE = 0, POWER_SLEEP_TAP = 1, POWER_SLEEP_BUTTON = 2 };

inline int next_power_mode(int m) { return m == POWER_SLEEP_TAP ? POWER_SLEEP_BUTTON : m == POWER_SLEEP_BUTTON ? POWER_AWAKE : POWER_SLEEP_TAP; }
inline const char *power_label(int m) {
  return m == POWER_SLEEP_TAP ? "Battery saver: tap or button wake"
       : m == POWER_SLEEP_BUTTON ? "Battery saver: button wake only"
       : "Always awake (about 1 day)";
}
inline bool deep_sleep_mode(int m) { return m == POWER_SLEEP_TAP || m == POWER_SLEEP_BUTTON; }

// Charging: the fuel gauge reports a positive average current while charging.
inline bool is_charging(float average_ma) { return average_ma == average_ma && average_ma > 5.0f; }

// Why the Sticky last woke up from deep sleep or power-off (kept in RTC memory, shown in HA).
enum WakeReason : uint8_t { WAKE_UNKNOWN = 0, WAKE_POWER_ON = 1, WAKE_TOUCH = 2, WAKE_KEY = 3 };
inline const char *wake_reason_label(int r) {
  return r == WAKE_TOUCH ? "Screen tap" : r == WAKE_KEY ? "Grey key" : r == WAKE_POWER_ON ? "Power on / reset" : "Unknown";
}

// Status for Home Assistant. wifi_off: the radio is about to go (or is) off, so this is the
// last state HA sees until the next wake; HA keeps it because the device has deep sleep.
inline const char *device_status(bool controls, bool maintenance, bool watching, bool charging, bool wifi_off, int mode) {
  if (maintenance) return "Update mode";
  if (controls) return "In use";
  if (watching) return "Showing hob";
  if (wifi_off) return charging ? "Standby, charging" : deep_sleep_mode(mode) ? "Asleep" : "Standby";
  return charging ? "Charging" : "Awake";
}

// Guard against a touch line that keeps waking the device (touch controller unpowered,
// wrong idle level, interference). A touch wake less than 3 s after going to sleep counts
// as suspicious; three in a row switch touch wake off.
struct WakeGuard {
  int quick = 0;
  // Returns true when touch wake should be disabled.
  bool touch_wake(uint32_t slept_s) {
    quick = slept_s < 3 ? quick + 1 : 0;
    return quick >= 3;
  }
  void other_wake() { quick = 0; }
};
}
