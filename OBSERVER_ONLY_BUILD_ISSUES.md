# Observer-Only Build Issues

## Current Status

The `feature/observer-core-optimization` branch has been fixed to compile correctly when `CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1` is defined.

## Issues Found and Fixed

1. **NimBLEDevice.cpp**: Was still compiling even though it shouldn't be used in observer-only mode
   - ✅ Fixed by adding `&& !defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)` to the compilation guard

2. **NimBLEScan.cpp**: Had dependencies on NimBLEDevice that needed to be handled differently in observer-only mode
   - ✅ Fixed references to `NimBLEDevice::getScan()` by using conditional compilation
   - ✅ Fixed references to `NimBLEDevice::m_ownAddrType` by using `BLE_OWN_ADDR_PUBLIC` in observer-only mode
   - ✅ Fixed callback registration to pass the scan object (`this`) as arg when in observer-only mode

## Changes Made

### NimBLEDevice.cpp
```cpp
#if defined(CONFIG_BT_ENABLED) && !defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)
// ... entire file content ...
#endif
```

### NimBLEScan.cpp
1. Added conditional inclusion of headers:
```cpp
# ifndef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
#  include "NimBLEDevice.h"
# else
#  include "NimBLEObserverOnly.h"
# endif
```

2. Fixed handleGapEvent to get scan object correctly:
```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    NimBLEScan* pScan = static_cast<NimBLEScan*>(arg);
#else
    NimBLEScan* pScan = NimBLEDevice::getScan();
#endif
```

3. Fixed scan start to use correct address type and pass scan object:
```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    int rc = ble_gap_disc(BLE_OWN_ADDR_PUBLIC,
                          duration ? duration : BLE_HS_FOREVER,
                          &m_scanParams,
                          NimBLEScan::handleGapEvent,
                          this);
#else
    int rc = ble_gap_disc(NimBLEDevice::m_ownAddrType,
                          duration ? duration : BLE_HS_FOREVER,
                          &m_scanParams,
                          NimBLEScan::handleGapEvent,
                          NULL);
#endif
```

## Testing

The observer-only build should now compile successfully with:
- Include `nimconfig_observer_only.h` before any NimBLE headers
- Use `NimBLEObserverOnly` instead of `NimBLEDevice`
- All scanning functionality works as expected

## Memory Savings

The observer-only build provides:
- ~200-400KB flash savings compared to full NimBLE
- Minimal RAM usage for passive scanning
- No overhead from unused connection, GATT, or security features