# Changelog

## v0.17 (8 November 2026) — standby, then sleep

* New default standby mode **"Tap for 5 min, then button wake"**. After you close the controls (or the hob
  summary disappears) the Sticky stays awake with Wi-Fi off for `standby_minutes` (default 5): a tap opens it
  instantly. After that it redraws *Press the grey button* and goes into deep sleep with grey-key wake only.
* Clock-tick and daily wakes skip the standby phase.
* Settings cycles: tap 5 min then button → button only → tap or button → always awake.
  Devices that already stored a mode keep it; pick the new one once in Settings.

## v0.16 (7 November 2026) — faster taps

* **Page changes use the fast partial refresh** instead of a full refresh, so a new page is ready to tap after
  about half a second instead of several seconds. A full refresh (clears ghosting) now happens when the Sticky
  returns to standby, and still every 30 updates.
* **Taps during a refresh are kept** and handled as soon as the screen is ready, instead of being ignored.
  Only the last tap is kept, for at most 1.5 s; taps within 350 ms of the previous one (accidental double taps)
  and taps in standby are not kept.
* The tap logic moved into a `handle_tap` script; behaviour is otherwise unchanged.

## v0.15.1 (7 November 2026) — sleep diagnostics

* **Default standby is now *Battery saver: button wake only*.** The touch controller is off in sleep; tap wake
  stays available in Settings. Devices that already stored a mode keep it until you change it in Settings.
* **No clock by default** (`clock_step_minutes: "0"`): standby shows *Tap to start* (or *Press the grey button*)
  and the Sticky no longer wakes every minute; only a tap, the grey key or one quiet daily wake (clock sync).
  Set `clock_step_minutes` to 1, 5 or 15 to get the clock back.

For tracking down overnight battery drain. Reported after each tap or key wake, for the sleep period before it:

* **Clock wakes last sleep**: how many times the minute clock woke the Sticky. About one per minute asleep means
  deep sleep works; 0 means it never went to sleep.
* **Awake time last sleep** (s): total time awake during those clock wakes. A few seconds per wake is normal.
* The log shows `Deep sleep after N ms awake` and `Last sleep: N clock wakes, N s awake in total`.

Correction to v0.15: Home Assistant does show the Sticky's entities as *unavailable* while it sleeps.
*Asleep* followed by *unavailable* means it went to sleep.

## v0.15 (6 November 2026) — sleep status in Home Assistant

New entities:

* **Status**: `In use`, `Showing hob`, `Update mode`, `Awake`, `Charging`, and, just before Wi-Fi goes off,
  `Asleep` (battery-saver modes), `Standby` (always awake, Wi-Fi off) or `Standby, charging`.
* **Last wake** (diagnostic, timestamp): last tap, grey-key or power-on wake. Minute clock ticks do not count.
* **Wake reason** (diagnostic): `Screen tap`, `Grey key` or `Power on / reset`.

The Sticky has a `deep_sleep:` component, so Home Assistant keeps these (and the battery sensors) at their last
value while it sleeps instead of showing *unavailable*. The status is sent about 1 s before Wi-Fi switches off.
Last wake and wake reason are kept in RTC memory and re-sent on every connection.

## v0.14.1 (6 November 2026) — glance shortcut

* Tap the hob glance on the standby screen to open the cooktop page directly. A tap anywhere else still opens
  the home page, and the grey key keeps its "back" role.
* The "Clock updates every … minutes" line is hidden while the hob glance is shown, so the two do not overlap.
* Timer label uses an explicit `unsigned` cast (compiler format warning).

## v0.14 (6 November 2026) — battery life

The old default standby kept the ESP32-S3 awake (about 30 mA): a 750 mAh battery lasted about a day.

* **Three standby modes**, cycled in Settings:
  * *Battery saver: tap or button wake* (new default): deep sleep; a tap on the screen or the grey key wakes it.
  * *Battery saver: button wake only*: deep sleep, touch controller held in reset; lowest consumption.
  * *Always awake (about 1 day)*: the previous behaviour.
* Tap-to-wake keeps the touch controller powered in deep sleep and uses its interrupt line (GPIO21) as a second
  wake source next to the grey key. If that line keeps waking the Sticky (three touch wakes less than 3 s after
  going to sleep), touch wake switches itself off and the mode becomes *button wake only*.
* **While charging the Sticky stays awake**, detected from the fuel gauge's average current.
* New sensor **Battery current** (mA, positive = charging) in Home Assistant and in the log (`sticky_power`).
* Deep sleep still waits while the hob is on or hot (standby glance), while the timer runs and in update mode.

Not yet verified on hardware: whether the touch controller wakes the ESP from deep sleep on this board, and the
actual standby current. Check after flashing: put the Sticky to sleep, tap the screen, and look for
`Woken by touch after N s` in the log. Note the battery percentage over a day.

Existing devices start in the new default mode (the setting moved to a new stored value).

## v0.13.1 (6 November 2026)

* Link logging: the log shows `Push from hob OK` every 10 s, or `HTTP snapshot poll` when the push is quiet (logger level INFO).
* Home Assistant entities for coffee, lights and hood are substitutions at the top of `sticky-hob.yaml`.

## v0.13 — calm black-and-white UI, instant feedback, pushed state

**Install on BOTH devices.** Hob: replace `sticky-direct.yaml`, add `sticky_push_key` to the hob's
secrets.yaml, flash. Sticky: replace `sticky-hob.yaml` and the whole `sticky/` folder, add the same
`sticky_push_key` to its secrets.yaml, flash. Any long random string works as the key; it must match.
If the hob's ESPHome name is not `induction`, set `hob_name` in the Sticky substitutions.

### Display
- Pure black and white with partial refreshes (`seeed-reterminal-sticky`, `full_update_every: 30`).
  No grey, so no full-screen flash on every change. A full refresh still runs on every page change
  and every 30 updates to clear ghosting; that one flashes once, by design.
- Cooktop page: zones at 0 are empty outlines; only active zones show a number. The selected zone is
  solid black with a white digit. Residual heat shows "H". Pan icon only when a pan is present.
- Level row: solid = level the hob REPORTS, outlined box = level you REQUESTED that the hob has not
  reported yet. The outline appears on the tap itself; it turns solid once the hob confirms, or
  disappears after 4 s with "Not applied - check hob". Displayed levels are still never optimistic.
- Power icon is inverted (white on black) while the hob is on; a ring shows a pending switch-on.
- Menu and appliance tiles are borderless. Status icons appear only when something is wrong
  (Wi-Fi lost, hob link stale on the cooktop page, battery below 20 %).
- Multi-zone / pause / bridge icons stay in place as reserved, non-operational buttons.

### Keys
- Grey key: wakes the Sticky; otherwise goes back one page (cooktop -> menu -> clock).
- UP / DOWN on the cooktop page: selected zone one level up / down. Repeated presses step from the
  pending request and coalesce into one command, so pressing UP three times quickly sends the final level.
  Levels above 0 need the hob switched on; a notice says so otherwise.
- Hold UP 3 s: update mode (unchanged).
- The Stop-all key action is removed. Stop all zones remains available on the hob's own dashboard and in HA.

### Standby glance
When you leave the cooktop page while the hob is on, a zone is set, or a zone reports residual heat,
the Sticky keeps Wi-Fi on in standby and the clock shows a small live hob symbol ("2 zones on",
"Residual heat"). It switches Wi-Fi off once the hob reports off and cool. If data stops arriving it
shows "Hob status unknown" instead of going blank: it never implies "off" without fresh data.
This costs battery only while the hob is active; battery-saver deep sleep waits until then.

### Push link (hob -> Sticky)
The hob broadcasts its state over encrypted UDP (`packet_transport`, port 18511, rolling code) about once
a second and immediately on every change. The Sticky no longer polls every 2 s; the HTTP snapshot is only
used for Settings > Hob connection tests and as a fallback whenever no push arrived for 4 s
(e.g. a router that blocks broadcast, or client isolation on the IoT network). Commands still go over HTTP.
State is accepted only from the device named `hob_name` with the matching key.

## v0.12 — Home Assistant action diagnostics

The coffee, light and hood controls call Home Assistant via Sticky's encrypted native API, not the hob connection. Add Sticky in Home Assistant Settings > Devices & services > ESPHome, then configure Sticky and enable Allow the device to perform Home Assistant actions. Device Builder log connectivity does not establish Home Assistant connectivity. After waking, allow Home Assistant to reconnect before tapping.

The action script now requires a state-subscribing API client (a logger-only connection is insufficient). No offline actions are queued. The appliance page shows not-connected, sending, completed or failed; sticky_ha logs include the exact service/entity and returned error. Completed means HA reported action execution, not a physical-device state confirmation. Existing entity mappings are unchanged. If it still fails, share the sticky_ha lines after a single tap. HA permission and live entity availability cannot be verified from this workspace.

Replace sticky-hob.yaml and sticky/, preserve secrets, flash Sticky only.

## v0.11 — larger zones and faster readback

Power icon is borderless and 24 pixels lower. Left and right zones extend to y=384, eight pixels above the selector. Middle zone is 236 x 200 pixels; feature icons sit beneath it. Shared geometry keeps touch targets aligned.

Removed the 1.2-second minimum redraw interval. Screen updates are checked every 100ms and remain gated on display idle and visible changes. Normal polling remains every two seconds via next_poll; an immediate readback follows a command. Displayed levels remain reported values, not optimistic command values. Four-grey full-refresh hardware latency/flashing remains. No command retry or offline queue was added.

Replace sticky-hob.yaml and sticky/, preserve secrets.yaml, flash Sticky only.

## v0.10 — corrected greys and single-row power selector

Power levels are one continuous 0–9/P row with vertical dividers. Each 70 × 80 pixel segment is the touch target. The selected reported level and selected zone use RGB (128,128,128).

Correction: v0.9 used RGB (170,170,170), which ESPHome's four-grey quantizer converts to WHITE. The driver bands are <32 black, 32–95 dark grey, 96–159 light grey, >=160 white. Native four-grey mode still requires full refreshes: a black/white flash when the visible screen changes is expected. Partial updates would remove the greys.

The hob page now skips refreshes when only invisible state changes (counters, notices, readiness) occur. Snapshot changes no longer mark unrelated appliance/menu pages dirty. Actual pan/heat/level/connection changes still refresh. This reduces unnecessary refreshes; it does not eliminate full-refresh flashing. Watchdog root cause remains unconfirmed.

Install: replace sticky-hob.yaml and sticky/, preserve secrets, flash Sticky only. Coffee/light/hood actions from v0.9 are retained.

## v0.9 — grey selections and appliance controls

Selected hob zone and reported power level now have light-grey fills with black text. The native four-grey display mode uses full refreshes to preserve grey shades; refreshes are slower and use more energy than monochrome partial updates. The middle zone has no selection underline. Zone outlines use scanline rendering to reduce drawing work.

Coffee: 1 cup calls script.grind_coffee; 2 cups calls script.grind_coffee_for_two (script.turn_on).
Lights: Kitchen top and Kitchen bottom toggle light.kitchen_top and light.kitchen_bottom. Hood presses button.hood_light.
Extraction hood: Light, Timer, Off, +, - press button.hood_light, button.hood_power, button.hood_mid, button.hood_speed, button.hood_speed_2 respectively.

These appliance pages send an immediate Home Assistant action per tap; they do not queue offline actions or claim device state. Add Sticky to the Home Assistant ESPHome integration and enable “Allow the device to perform Home Assistant actions” in its configuration. Home Assistant must be connected for these controls. Direct hob control still operates independently of Home Assistant.

Replace sticky-hob.yaml and the entire sticky/ folder, preserve your secrets.yaml, and flash the Sticky only. No hob firmware update is needed.

The supplied text-style reference was unreadable (black rectangle), so the existing large power-level button layout is retained pending a readable reference.

For the reported DNS stall, use Settings > Hob connection > Enter IP and select the hob by IP. The logs show a 6.18-second failed hostname lookup despite the HTTP timeout. The watchdog root cause is not established or claimed fixed by this update. Watchdog protection remains enabled.

## v0.8 — reference hob layout

Continuous rounded left zone split into upper/lower logical targets, rounded middle and right zones. Smaller adjacent pan/heat icons, no zone text labels. Rounded touch boundaries share the same pixel geometry as the visible zone outlines; corners outside the shape do nothing. Power toggle and immediate level actions retained. Multi-zone/pause/bridge remain non-operational placeholders. Existing hob firmware/add-on is unchanged; flash the Sticky only.

## v0.7 — larger hob controls

- Level taps send immediately; Apply and the hob-page back arrow/footer are removed.
- Power icon above the middle zone toggles Hob on/Hob off. Physical OK returns to the menu; physical Down retains Stop all.
- Five large zone targets and 118 × 64 pixel level buttons. Drawing and hit testing share the same rectangles, with inactive gaps.
- Pan icon appears only when present. Residual-heat flame remains visible after connection loss; it is not proof of current temperature.
- Multi-zone, pause and bridge are borderless outlined placeholders below the middle zone. They send no commands: this package does not implement those MACS functions.
- Battery, Wi-Fi and fresh-hob-link indicators remain on every page. No additional hob-side changes are required if v0.6 already connects.

## Sticky standalone remote — v0.6, 4 November 2026

Seeed reTerminal Sticky SKU 100056818. ESPHome source package, requires ESPHome 2026.10.2 or later on BOTH the Sticky and hob.

### What changed

v0.6: shared battery + Wi-Fi + hob-link icons appear on every screen. The chain is connected only while a validated hob snapshot is fresh; Wi-Fi alone is not proof of hob connectivity. Clock standby deliberately disables Wi-Fi to save power, so disconnected icons there are normal. While any control/menu page is open, snapshots are polled every 2 seconds; failed responses back off to 30 seconds. Selecting/testing a hob remains immediate. A new four-ring cooktop symbol replaces mdi:stove, inspired by google-circles-extended.

### Fixing HTTP 404

404 means an HTTP server answered, but did not expose `/text_sensor/Sticky%20snapshot`. Install the included `hob/sticky-direct.yaml` on your EXISTING hob configuration through `packages`, then compile and flash the HOB as well as the Sticky. The entity must be named exactly `Sticky snapshot` and exposed (`internal: false`). Both devices must use ESPHome 2026.10.2+ with this name-based REST API. If already installed, check the selected host/IP and inspect the hob's actual entity names. A Sticky-only flash cannot create a missing hob endpoint.

Before trying cooking commands, open `http://HOB_IP/text_sensor/Sticky%20snapshot` and verify HTTP 200 with the JSON snapshot. If it is still 404, share the current hob YAML and its ESPHome version. Do not change bus pins or UART settings to troubleshoot this HTTP error.

v0.5 replaces the menu sketches with bundled Material Design Icons (MDI 7.4.47): stove, fan, coffee-outline, timer-outline, lightbulb-outline and cog-outline. Back buttons use mdi:arrow-left. Icons require no runtime internet access.

The GT911 now uses Seeed's `swap_xy: true` transform for the landscape display. Menu tiles, keypad, settings rows, timer controls and cooking buttons use shared drawing/touch rectangles in `sticky/layout.h`. Gaps and the unused keypad position are non-interactive. Touches while a new page is pending or the ePaper is refreshing are ignored, so an old visible button cannot activate the next page's action.

Update **sticky-hob.yaml and the entire sticky directory**, including the new font and layout header. No hob-side update is needed for this v0.5 UI change if the v0.4 discovery add-on is already installed.

Settings now includes **Hob connection**. Tap **Detect hobs**, then a result to check and save it. Alternatively use **Enter IP**, enter an IPv4 address (optionally `:port`), and tap **Test**. Only a valid snapshot response is saved. Failed checks retain the previous address. The saved selection survives reset and deep sleep; no YAML address change is needed.

Discovery requires the updated `hob/sticky-direct.yaml` on the hob and multicast/mDNS reachability on the same network. Up to four unique hobs are displayed per scan. Selection is explicit; the remote never silently switches to another detected hob. Detection is read-only. The manual IP option works when multicast is blocked. Wi-Fi credentials still come from secrets.yaml.

Sticky now talks directly to the hob's existing local web API. Home Assistant, MQTT, the ESPHome Device Builder server and an internet connection are not required for cooking controls. Both ESPs must remain powered and reachable over Wi-Fi. This does not allow control when the hob ESP itself is offline.

The link is always direct; there is no automatic switching between HA and HTTP, no replay of commands, and no second MACS master. The existing hob firmware still owns bus scheduling, modes, no-pan timeout, communication fault handling and command timeout. This package does not replace that component.

### Install on the hob first

1. Keep your current working hob YAML, MACS component, dashboard and pins.
2. Copy `hob/sticky-direct.yaml` beside your hob YAML, then merge this into its top-level `packages:` section (create it if absent):

```yaml
packages:
  sticky_direct: !include sticky-direct.yaml
```

3. Verify the existing component ID is `hob`, `web_server` is enabled on port 80, and `macs_hob.require_api_connection` is `false`. Keep `api.reboot_timeout: 0s`. Do not disable the no-pan, communication or command timeouts.
4. Compile and install the hob configuration with ESPHome 2026.10.2+. It adds one diagnostic text sensor, `Sticky snapshot`, and a `_macshob._tcp` discovery announcement. Match the advertised port to your existing web_server port if it is not 80. No new heating actions are added.
5. Browse `http://HOB_IP/text_sensor/Sticky%20snapshot`. It should return JSON whose `value` or `state` is another JSON string with `schema`, `tick`, `mode`, `hob`, `ready`, `fault`, `levels`, `pan`, and `hot`. `tick` must advance about once a second.

### Install on Sticky

1. Copy `sticky-hob.yaml` and the whole `sticky/` directory into ESPHome. Do not overwrite your hob YAML.
2. Leave `hob_url` as the initial fallback. After flashing, open Settings → Hob connection → Detect hobs and select yours, or enter its IP manually. A saved selection overrides the fallback. Reserve manually entered IPs in DHCP.
3. Merge secrets.example.yaml into secrets.yaml and replace placeholders. API encryption is only for optional HA pairing; OTA credentials are for updates.
4. Compile and USB-flash using Device Builder 2026.10.2+. For wireless updates, hold UP for 3 seconds or use Settings → Enable updates.
5. Do NOT install the old Home Assistant bridge. It is no longer needed. If already installed it may remain for other uses; this firmware never calls it.

This version uses your existing unauthenticated local web server. Do not expose it to the internet. If you have added HTTP authentication, requests will be rejected until client authentication is configured too. Entity names must remain `Hob on`, `Stop all zones`, `Bottom left`, `Top left`, `Middle`, `Bottom right`, `Top right`. The firmware uses the exact name-based REST endpoints from ESPHome 2026.10.2.

### User interface

Clock is standby. Tap it, or press OK, for the landscape 2×3 icon menu:

| | | |
|---|---|---|
| Cooktop | Extraction hood | Coffee |
| Timer | Lights | Settings |

Cooktop: select a zone, then tap a level to send it immediately. The power icon toggles on/off immediately. DOWN sends Stop while the cooktop page is open; OK returns to the menu. Opening a menu icon never activates an appliance. The left zones remain paired logical controls; this update does not add Flex modes.

The UI shows received settings, pan presence and heat. Last known H is retained as H? during stale state. HTTP 200 means request delivered, not proof of heating or shutdown. Observe the reported state. In Dev, the original panel can overwrite remote commands, including Stop.

Hood, coffee and lights remain explicitly unconfigured screens until their interfaces are supplied.

The local timer supports 1–180 minutes. It sounds once and opens the timer page when finished. It continues on the clock screen, prevents deep sleep while running, and resets on reboot. It is a reminder, not an automatic hob shutoff.

### Offline and power behaviour

- Polls the hob every 2 seconds while a menu/control page is open, backing off to 30 seconds on errors. Malformed/failed responses immediately invalidate state and discard pending selections. Repeated snapshots do not extend freshness; state expires after 8 seconds.
- ON and nonzero APPLY require fresh state, an appropriate controller mode, no reported fault and Master ready when in Master. Stop can be attempted with stale data if Wi-Fi is connected.
- No automatic retries or queued commands. A failed POST reports delivery unknown. Check the hob before issuing another request. The HTTP timeout is 2 seconds; during a blocked request UI servicing may be delayed by that interval.
- Clock uses the onboard RTC. SNTP replaces HA time sync. Internet or a configured local NTP server is only needed to initially set/correct time, not for hob commands. A previously set RTC keeps running offline.
- Wi-Fi turns off on clock standby except during time sync or update windows. Default standby keeps CPU and touch awake for tap-to-open. Battery saver uses deep sleep and requires OK to wake. Timer completion keeps the device awake.
- Partial refresh between periodic full refreshes; no seconds animation. Clock step can be set to 5 or 15 minutes. Inactivity timeout defaults to 120 seconds. Going to standby does not turn the hob off.
- No battery-runtime estimate or physical wake verification is claimed.

### Optional operation without a router

The default is direct communication over your existing Wi-Fi, with no HA/server dependency. To eliminate the router too, replace the hob's entire `wifi:` block with the contents of `hob/wifi-ap-only.example.yaml`, and replace Sticky's SSID/password with that AP's credentials. Choose Settings → Hob connection → Enter IP → `192.168.4.1` → Test on Sticky.

This is a dedicated AP-only configuration, not automatic failover. It removes the hob from your home Wi-Fi. Join the hob AP from your phone/computer to access its dashboard or upload OTA. Initial clock synchronization must be done on normal Wi-Fi first, or via a reachable local NTP server; the AP does not provide NTP. The RTC can then keep time offline. Validate AP radio reachability and OTA before adopting this setup.

### Acceptance test

1. Start with hob off. Verify snapshot ticks, mode, fault and ready state.
2. Turn off HA and Device Builder on the server, leaving Wi-Fi running. Open cooktop on Sticky: state must still update.
3. With supervision, test ON, one zone at 1, then Stop. Verify the physical result and the hob's reported state.
4. Disable Sticky Wi-Fi or interrupt reachability: stale state must show unknown, pending ON/level must clear and heating commands must be unavailable. Restoring connectivity must not replay a command.
5. Keep the hob's established physical shutoff available. Sticky sleep/network loss does not itself stop heating; the existing hob-side protections remain responsible.
6. Verify clock/menu, timer completion and optional battery saver on hardware. Test no-pan and fault handling separately using the existing hob test plan.

See VALIDATION.md for checks and limits. Fonts use the included DejaVu license.

### Connection-menu acceptance checks

- With the hob off, Detect hobs should list its hostname. Select it and wait for Connected and saved. This must not activate the hob.
- Reboot Sticky, then check that the address remains selected. Open Cooktop to get fresh state.
- Enter an unused IP: the test must fail and retain the previous hob.
- Block multicast: manual IP entry should still work if HTTP is reachable.
- If two hobs advertise, choose explicitly and confirm the correct physical appliance before cooking. Discovery is not authenticated pairing.
- Changing the selection clears all old state and pending controls; opening Cooktop requires new state before ON or a nonzero level can be applied.
- The discovery query can pause UI servicing for up to 1.5 seconds; Wi-Fi connection waits are bounded to 15 seconds. HTTP checks use the existing 2-second timeout. Scans are user-initiated only; no background discovery drain.

Discovery API reference: https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html

### Touch check after flashing v0.5

With the hob off, open each of the six menu tiles by tapping its centre, then Back. Check all four corners of the manual-IP keypad without pressing Test. Tapping a gap should do nothing. This verifies orientation and bounds without issuing heating commands. If the physical touch position is still displaced, report which displayed tile was tapped and which screen opened; do not compensate by changing cooking-button coordinates.

Hardware reference: https://www.seeedstudio.com/sticky/playground/esphome/esphome-sticky.js (GT911 swap_xy). Your attached sample agrees on the 800x480 display preset; it does not include an enabled touchscreen block.
Icon source and license: https://github.com/Templarian/MaterialDesign-Webfont ; bundled license in sticky/fonts/MDI-LICENSE.txt.
