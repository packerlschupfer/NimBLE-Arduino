# NimBLE-Arduino Observer-Only Build Update Guide

## Summary of Fixes Applied

The `feature/observer-core-optimization` branch has been successfully fixed to compile correctly when `CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1` is defined. This enables maximum memory savings (~200-400KB) for passive BLE scanning applications.

## Key Changes Made

### 1. NimBLEDevice.cpp Compilation Guard
- Added `&& !defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)` to prevent compilation in observer-only mode
- This ensures the full NimBLEDevice class is completely excluded from observer-only builds

### 2. NimBLEScan.cpp Fixes
- Added conditional header inclusion (NimBLEDevice.h vs NimBLEObserverOnly.h)
- Fixed `handleGapEvent()` to retrieve scan object from callback arg in observer-only mode
- Fixed `start()` method to:
  - Use `BLE_OWN_ADDR_PUBLIC` instead of `NimBLEDevice::m_ownAddrType`
  - Pass `this` as callback arg for proper scan object retrieval

### 3. Stub Implementations
- `nimble_stubs.cpp` provides weak symbol stubs for unused subsystems
- Prevents linker from pulling in GATT client/server, connection management, etc.

### 4. ESP32 Controller Initialization Fix (CRITICAL UPDATE)
- **Fixed ESP_ERR_INVALID_ARG (258) error** - The ESP32 BT controller requires at least 1 connection to be configured even in observer-only mode
- **Fixed mode mismatch error** - The controller must be enabled with the same mode it was initialized with
- **Proper initialization sequence**:
  1. Controller deinit if already initialized
  2. NVS flash initialization check
  3. Controller init with BLE_MODE (not BTDM_MODE)
  4. Controller enable with matching mode
  5. Host task initialization
- **Successfully tested** with ATC PVVX temperature sensor
- **Memory savings achieved**:
  - Flash: 546KB used (41.7% of 1.31MB) - saves ~200-400KB vs full NimBLE
  - RAM: 31KB used (9.6%) - minimal footprint

## How to Use Observer-Only Mode

### For Boiler Controller Project

1. Include the special configuration header BEFORE any NimBLE headers:
```cpp
#include "nimconfig_observer_only.h"
#include <NimBLEObserverOnly.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>
```

2. Use `NimBLEObserverOnly` instead of `NimBLEDevice`:
```cpp
// Initialize with error checking
if (!NimBLEObserverOnly::init("MyDevice")) {
    Serial.println("ERROR: Failed to initialize NimBLE!");
    return;
}

// Get scan object with null check
NimBLEScan* pScan = NimBLEObserverOnly::getScan();
if (!pScan) {
    Serial.println("ERROR: Failed to get scan object!");
    return;
}

// Configure and start scanning
pScan->setActiveScan(false);
pScan->start(0); // Scan forever
```

### For PlatformIO Users

Include the observer-only configuration in your `platformio.ini`:
```ini
[env:observer_only]
build_flags = 
    -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1
    -include "nimconfig_observer_only.h"

; Optional: exclude unused source files
lib_ignore = 
    NimBLEDevice.cpp
    NimBLEClient.cpp
    NimBLEServer.cpp
    ; ... see nimble_platformio_observer.ini for full list
```

## Memory Savings

The observer-only build provides:
- **~200-400KB flash savings** compared to full NimBLE
- **Minimal RAM usage** for passive scanning
- **No overhead** from unused connection, GATT, or security features

## Example Code

See `examples/Observer_Only_Test/Observer_Only_Test.ino` for a complete working example.

## Migration Path

1. The standard role-disabling approach (47-58KB savings) remains stable and recommended for most users
2. The observer-only build (200-400KB savings) is now available for applications that only need passive scanning
3. Both approaches are fully supported and maintained

## Testing

The observer-only build has been tested to:
- Compile successfully with all optimizations enabled
- Initialize ESP32 BT controller correctly without errors
- Perform continuous passive scanning
- Handle scan callbacks correctly
- Work with existing NimBLEAdvertisedDevice API
- Successfully detect and parse ATC PVVX temperature sensor data

## Known Issues Fixed

1. **ESP_ERR_INVALID_ARG (258)**: ESP32 requires at least 1 connection configured
2. **Controller mode mismatch**: Must use BLE_MODE consistently 
3. **NimBLEObserverOnly::getScan() returns null**: Now properly initializes scan object

## Contact

For issues or questions about the observer-only build, please:
- Check the `OBSERVER_ONLY_BUILD_ISSUES.md` file for technical details
- Open an issue on the NimBLE-Arduino GitHub repository
- Reference this guide when reporting problems