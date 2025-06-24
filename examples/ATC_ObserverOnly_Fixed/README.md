# ATC_MiThermometer Observer-Only Mode - Fixed Example

This example demonstrates how to properly use ATC_MiThermometer library with NimBLE's observer-only mode.

## Key Fix

The main issue with the original example is that ATC_MiThermometer tries to access the whitelist before NimBLE is initialized. This example fixes that by:

1. **Initializing NimBLE first** - Before creating the ATC_MiThermometer instance
2. **Configuring the whitelist** - After NimBLE is initialized
3. **Then creating ATC_MiThermometer** - With a properly initialized environment

## Memory Savings

Observer-only mode provides:
- ~200-400KB flash savings
- ~100KB RAM savings
- Full BLE scanning capabilities
- Hardware whitelist support

## Configuration

Edit the MAC addresses in `main.cpp` to match your ATC sensors:

```cpp
std::vector<std::string> knownSensors = {
    "a4:c1:38:1d:87:bb",  // Your sensor 1
    "a4:c1:38:07:51:82",  // Your sensor 2
    "a4:c1:38:a1:03:ba"   // Your sensor 3
};
```

## Building and Running

```bash
pio run -t clean
pio run -t upload
pio device monitor
```

## Expected Output

```
=====================================
ATC_MiThermometer Observer-Only FIXED
=====================================

Initializing NimBLE in observer-only mode...
NimBLE initialized successfully
Initial whitelist count: 0

Configuring hardware whitelist...
Added to whitelist: a4:c1:38:1d:87:bb (Office)
Added to whitelist: a4:c1:38:07:51:82 (Kitchen)
Added to whitelist: a4:c1:38:a1:03:ba (Living Room)
Whitelist now contains 3 entries

Creating ATC_MiThermometer instance...
Starting filtered scanning...

Free heap: 221256 bytes
Flash usage: ~42% (observer-only mode saves ~200-400KB)

Setup complete! Starting continuous scanning...

=====================================
Scan #1 - Starting 10 second scan...
=====================================

Devices found: 2

Office (a4:c1:38:1d:87:bb):
  Temperature: 21.82°C
  Humidity: 58.71%
  Battery: 100% (3.093V)
  RSSI: -45 dBm

Kitchen (a4:c1:38:07:51:82):
  Temperature: 22.51°C
  Humidity: 54.82%
  Battery: 100% (3.083V)
  RSSI: -52 dBm

Living Room (a4:c1:38:a1:03:ba):
  Status: NO DATA
```

## Troubleshooting

If no devices are found:
1. Verify the MAC addresses match your sensors
2. Ensure sensors are powered on and advertising
3. Check that sensors have ATC_MiThermometer custom firmware
4. Move sensors closer to the ESP32

## Technical Details

This example uses:
- NimBLE observer-only mode (CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)
- Hardware whitelist filtering (up to 8 devices)
- Passive scanning (lower power consumption)
- High-resolution timestamps for accurate beacon timing