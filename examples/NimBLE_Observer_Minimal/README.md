# NimBLE Minimal Observer Example

This example demonstrates the absolute minimal configuration for BLE scanning using the `NimBLEObserverOnly` interface.

## Key Differences from Standard NimBLE

1. **Uses `NimBLEObserverOnly` instead of `NimBLEDevice`**
   - Bypasses standard initialization that pulls in all subsystems
   - Direct initialization of only scanning components

2. **Includes `nimconfig_observer_only.h`**
   - Aggressively disables all non-scanning features at compile time
   - Prevents compilation of unused modules

3. **Links with `nimble_stubs.cpp`**
   - Provides stub implementations for unavoidable dependencies
   - Prevents linker from pulling in full implementations

## Memory Savings

Compared to standard NimBLE initialization:
- **Flash**: 200-400KB reduction
- **RAM**: 20-50KB reduction
- **Compiled files**: ~50 instead of 193

## Build Configuration

To use this minimal configuration in your project:

1. **Include the observer-only config first:**
   ```cpp
   #include "nimconfig_observer_only.h"
   #include "NimBLEObserverOnly.h"
   ```

2. **Add to your platformio.ini:**
   ```ini
   build_flags = 
       -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
       -Wl,--gc-sections
       -ffunction-sections
       -fdata-sections
   ```

3. **Use NimBLEObserverOnly API:**
   ```cpp
   // Initialize
   NimBLEObserverOnly::init();
   
   // Get scanner
   NimBLEScan* scan = NimBLEObserverOnly::getScan();
   
   // Configure and start
   scan->start(0);  // 0 = continuous
   ```

## Use Cases

Perfect for:
- BLE beacon scanners
- Sensor data collectors (MiThermometer, etc.)
- Presence detection
- Asset tracking
- Any application that only receives BLE advertisements

## Limitations

This configuration cannot:
- Connect to BLE devices
- Act as a BLE peripheral/server
- Use GATT services
- Perform pairing/bonding
- Send advertisements

## Troubleshooting

1. **Linker errors about missing symbols**
   - Ensure `nimble_stubs.cpp` is being compiled
   - Check that observer-only config is included first

2. **Still compiling too many files**
   - Verify `CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY` is defined
   - Check build output for which files are being compiled

3. **Runtime crashes**
   - Don't call any connection/GATT/advertising functions
   - Use only scanning APIs

## Measuring Success

Compare build output:
```
# Standard NimBLE:
Compiling .pio/build/esp32/lib/NimBLE-Arduino/nimble/host/src/ble_gattc.c.o
Compiling .pio/build/esp32/lib/NimBLE-Arduino/nimble/host/src/ble_gatts.c.o
... (193 files)

# Observer-only:
Compiling .pio/build/esp32/lib/NimBLE-Arduino/nimble/host/src/ble_gap.c.o
Compiling .pio/build/esp32/lib/NimBLE-Arduino/nimble/host/src/ble_hs.c.o
... (~50 files)
```

Flash usage:
```
# Standard: 1.3MB from NimBLE
# Observer-only: 900KB-1.1MB from NimBLE
```