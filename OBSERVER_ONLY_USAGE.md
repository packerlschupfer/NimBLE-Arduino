# Observer-Only Mode Usage Guide

## For Library Authors (like ATC_MiThermometer)

When creating a library that should work with both full NimBLE and observer-only mode:

### 1. Include the Compatibility Header

Instead of:
```cpp
#include <NimBLEDevice.h>
```

Use:
```cpp
#include <NimBLEDeviceCompat.h>
```

This header automatically provides the correct API based on whether observer-only mode is enabled.

### 2. Ensure Observer-Only Config is Applied First

In your PlatformIO examples, ensure the observer-only configuration is included before any library code:

```ini
build_flags = 
    ; IMPORTANT: Define observer-only mode first
    -D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1
    ; Then include the configuration header
    -include "${PROJECT_DIR}/.pio/libdeps/${PIOENV}/NimBLE-Arduino/src/nimconfig_observer_only.h"
```

### 3. Test Both Modes

Always test your library with:
- Full NimBLE mode (without CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)
- Observer-only mode (with CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1)

## For End Users

### Simple Setup

In your `platformio.ini`:

```ini
[env:esp32_observer]
platform = espressif32
board = esp32dev
framework = arduino

lib_deps = 
    ; Use the observer-optimized branch
    h2zero/NimBLE-Arduino@^2.2.3
    ; Your BLE libraries
    matthias-bs/ATC_MiThermometer@^latest

build_flags = 
    -D CONFIG_BT_ENABLED=1
    -D CONFIG_BT_NIMBLE_ENABLED=1
    ; Enable observer-only mode
    -D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1
```

### Advanced Setup with Direct Repository

```ini
lib_deps = 
    ; Direct from repository with specific branch
    git+https://github.com/h2zero/NimBLE-Arduino#feature/observer-core-optimization
    git+https://github.com/matthias-bs/ATC_MiThermometer#main

build_flags = 
    ${base_observer.build_flags}
    ; Include the observer config from library location
    -include "${PROJECT_DIR}/.pio/libdeps/${PIOENV}/NimBLE-Arduino/src/nimconfig_observer_only.h"
```

## Memory Savings

Observer-only mode provides:
- **~46KB flash savings** from the basic observer-only configuration
- **200-400KB additional savings** when using NimBLEObserverOnly API
- Reduced RAM usage from smaller buffer allocations

## API Compatibility

The NimBLEDeviceCompat.h header ensures these methods work in observer-only mode:
- `NimBLEDevice::init()`
- `NimBLEDevice::getScan()`
- `NimBLEDevice::whiteListAdd()`
- `NimBLEDevice::whiteListRemove()`
- `NimBLEDevice::whiteListClear()`
- `NimBLEDevice::getWhiteListCount()`
- `NimBLEDevice::getWhiteListAddress()`
- `NimBLEDevice::getAddress()`
- `NimBLEDevice::isInitialized()`
- `NimBLEDevice::deinit()`

## Troubleshooting

### "Incomplete type 'NimBLEDevice'" Error

Make sure:
1. Your library includes `<NimBLEDeviceCompat.h>` instead of `<NimBLEDevice.h>`
2. The observer-only flag is defined before any NimBLE headers are included

### Linker Errors

If you see undefined references to store-related symbols:
1. Ensure you're using the latest NimBLE-Arduino with observer-only support
2. Clean your build: `pio run -t clean`
3. Rebuild: `pio run`

### Feature Not Available

Some features are intentionally disabled in observer-only mode:
- Client connections
- Server/advertising
- Bonding/pairing
- GATT operations

If you need these features, don't use observer-only mode.