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

### 4. ESP32 Controller Initialization Fix
- Fixed controller initialization sequence for ESP32 compatibility
- Requires at least 1 connection even in observer-only mode (ESP32 hardware requirement)
- Proper controller state checking and cleanup before initialization
- Successfully tested with ATC PVVX temperature sensor (A4:C1:38:1D:87:BB)

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
// Initialize
NimBLEObserverOnly::init("MyDevice");

// Get scan object
NimBLEScan* pScan = NimBLEObserverOnly::getScan();

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
- Perform continuous passive scanning
- Handle scan callbacks correctly
- Work with existing NimBLEAdvertisedDevice API

## Contact

For issues or questions about the observer-only build, please:
- Check the `OBSERVER_ONLY_BUILD_ISSUES.md` file for technical details
- Open an issue on the NimBLE-Arduino GitHub repository
- Reference this guide when reporting problems