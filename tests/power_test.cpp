#include "../sticky/power.h"
#include <cassert>
#include <cmath>
#include <cstring>
int main(){
 using namespace sticky;
 // Settings row cycles through all three modes.
 assert(next_power_mode(POWER_SLEEP_TAP)==POWER_SLEEP_BUTTON);
 assert(next_power_mode(POWER_SLEEP_BUTTON)==POWER_AWAKE);
 assert(next_power_mode(POWER_AWAKE)==POWER_SLEEP_TAP);
 assert(deep_sleep_mode(POWER_SLEEP_TAP) && deep_sleep_mode(POWER_SLEEP_BUTTON) && !deep_sleep_mode(POWER_AWAKE));
 assert(std::strlen(power_label(POWER_SLEEP_TAP))<40);
 // Charging only on a clearly positive current; NaN and discharge are not charging.
 assert(is_charging(120) && !is_charging(3) && !is_charging(-35) && !is_charging(NAN));
 // Three quick touch wakes in a row disable touch wake; a normal wake resets the count.
 WakeGuard g;
 assert(!g.touch_wake(1) && !g.touch_wake(0));
 assert(!g.touch_wake(400));            // genuine tap after a long sleep resets
 assert(!g.touch_wake(1) && !g.touch_wake(2) && g.touch_wake(0));
 g.other_wake(); assert(!g.touch_wake(1));
}
