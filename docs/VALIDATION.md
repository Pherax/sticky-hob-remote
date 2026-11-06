# v0.13 validation

- ESPHome 2026.10.2: Sticky configuration valid; hob R5 configuration (esp32dev, GPIO27/25) plus the new
  sticky-direct.yaml valid.
- Full ESP-IDF target builds: both firmwares compiled and linked to an ELF (Sticky esp32s3, hob esp32).
  Only the final size-report tool step failed in this workspace (tooling, after linking). The test build
  of the Sticky omitted API encryption because its crypto component could not be downloaded here.
- Seven native C++ tests passed, including new ones for push decoding (round trip of the hob's packing,
  stale-tick handling, garbage rejection), requested-level/step/settle logic, the standby "active" rule,
  and the quiet status bar.
- hob-preview.png, hob-off-preview.png and standby-preview.png are rendered from the actual drawing calls.

Not verified on hardware: partial-refresh appearance and ghosting on the Sticky panel, UDP delivery on
your network (broadcast + Wi-Fi power save), key timing, battery impact of the standby glance.

# v0.12 validation

ESPHome 2026.10.2 configuration/C++ generation and six native C++ tests passed. Checked action status callback signature against installed ESPHome source. Physical HA connection, permission, entity execution and target compilation remain unverified. This is diagnostics plus a connection guard, not a confirmed root-cause repair.

# v0.11 validation

ESPHome 2026.10.2 configuration/C++ generation passed. Six native tests passed including exhaustive non-overlap of power, zone, feature and level targets, updated rounded boundaries and visible-state change checks. Preview regenerated from drawing calls. Physical latency and full ESP32 target compilation remain unverified.

# v0.10 validation

ESPHome 2026.10.2 configuration and C++ generation passed. All six native tests passed, including non-overlapping single-row targets, shared separator boundaries, and hob visual-key changes (ignore invisible state, retain selected-zone and pan changes). RGB quantization checked against ESPHome documentation and generated driver colorconv.h. A standalone driver compilation attempt was blocked by missing host HAL in generated target sources; no full target compilation is claimed. The preview is illustrative RGB, not a physical-panel grayscale validation. Hardware refresh timing and grey appearance remain to be checked on the Sticky.

# v0.9 validation — 5 November 2026

ESPHome 2026.10.2 configuration validation and C++ generation passed. Six native C++ tests passed, including appliance button edge/gap checks, existing touch geometry, state gates, and hob rendering. Grey hob preview was rendered from C++ drawing calls and visually checked. Physical grey output, touch operation, Home Assistant delivery, and watchdog recovery require device testing. No watchdog fix or full ESP32 target build is claimed.

# v0.8 validation — 5 November 2026

ESPHome 2026.10.2 configuration and C++ source generation passed. All six native tests passed, including exhaustive control-overlap checks and rounded-zone corner/split boundaries. Preview rendered from actual C++ drawing calls and visually inspected. Physical touch calibration and full ESP32 target compilation remain unverified.

# v0.7 validation — 5 November 2026

ESPHome 2026.10.2 configuration validation and C++ generation passed. Six native tests passed: state gates, layout boundaries, menu, status bar, connection and hob rendering. Exhaustive 800×480 checks found no overlapping hob targets; level target edges/gaps match drawn rectangles. Hob preview rendered from the actual C++ drawing calls and inspected.

Level commands now send on tap. Existing fresh-state, ready/fault and mode checks remain. Power-off uses the existing Hob off button endpoint. Placeholder multi-zone/pause/bridge icons issue no command. Full target compilation and physical touch alignment remain unverified.

Previous validation history follows.

# Validation — 4 November 2026, standalone v0.6

Passed on ESPHome 2026.10.2:
- Sticky configuration validation and C++ source generation with direct HTTP requests, SNTP and no Home Assistant sensor/action dependency.
- Original R5 hob configuration plus sticky-direct.yaml: validation and C++ source generation.
- Native C++ UI-state tests including Master readiness, fault inhibition, explicit zero, stale data, Listen/Auto rejection, timer rollover and navigation.
- Exact REST entity-name routing and response fields checked against ESPHome 2026.10.2 web_server source.

Not verified:
- A successful ESP32 target compilation or on-device operation. The target build for this update was blocked before application compilation by ESP-IDF component discovery (`psutil.NoSuchProcess`) in this environment; configuration/code generation is not a target compilation.
- Actual network delivery, radio coverage, local AP behaviour, display/touch and battery runtime.
- No new MACS or hob protection validation is claimed. Existing MACS implementation is unchanged.

The direct snapshot schema requires all five level/pan/heat values and ready/fault flags. Invalid payloads invalidate local state; unchanged snapshot ticks do not refresh its age. Command requests are immediate and are never queued for reconnect or retried automatically. HTTP 200 is delivery to an entity handler, not confirmation that heating changed.

See README.md for the server-off acceptance test and installation on both devices.

Settings update checks:
- Native C++ tests passed for manual address validation, keypad hit regions, persistence reload, failed-save retention, clearing old control state, discovery deduplication and result cleanup (mocked platform APIs).
- Existing state and menu tests passed.
- Discovery and persistence have not yet been exercised on physical Sticky hardware. Native mocks are not proof of ESP-IDF integration.

MDI/touch update:
- Native C++ menu, connection and exhaustive 800x480 menu/keypad hit-region tests passed, including gaps, unused keypad cell and button boundaries.
- Touch-axis transform matches Seeed's current GT911 example. Physical touch alignment still requires the on-device check in README.
- Full target build remains unverified; the prior attempt was blocked during ESP-IDF component discovery. No hardware test is claimed.
- v0.5 ESPHome 2026.10.2 configuration and C++ source generation passed with bundled MDI glyphs and touch transform. Menu preview rendered and inspected.

v0.6 checks: status icon native tests distinguish Wi-Fi from fresh hob data and show disconnected for stale snapshots. Layout/state/menu tests passed. Sticky configuration/source generation passed. Name-based REST matching and non-2xx response callbacks checked against official ESPHome 2026.10.2 source. The 404 cause on the physical hob is not confirmed; updated add-on installation and endpoint verification are required. Hardware and full target build remain unverified.
