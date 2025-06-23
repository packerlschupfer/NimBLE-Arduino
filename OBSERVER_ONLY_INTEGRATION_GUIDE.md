# Observer-Only Mode Integration Guide

This guide is for library/application developers who want to support NimBLE's observer-only mode for maximum memory savings (~200-400KB).

## For Boiler Controller Team

Your implementation is already correct! You're using conditional compilation properly:

```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
    // ... use NimBLEObserverOnly::init(), getScan(), etc.
#else
    #include <NimBLEDevice.h>
    // ... use NimBLEDevice::init(), getScan(), etc.
#endif
```

The compilation errors you're seeing are from the ATC_MiThermometer library, not your code. The library needs to be updated to support observer-only mode.

### Temporary Workaround

Until ATC_MiThermometer is updated, you have two options:

1. **Use standard role-disabling** (still saves 47-58KB):
   ```cpp
   #define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED
   #define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED
   #define CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED
   ```

2. **Fork and patch ATC_MiThermometer** temporarily (see instructions below)

## For ATC_MiThermometer Team

Your library needs to support both standard and observer-only NimBLE modes. Here's how:

### 1. Update Your Include Section

Replace:
```cpp
#include <NimBLEDevice.h>
```

With:
```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
#else
    #include <NimBLEDevice.h>
#endif
```

### 2. Update Initialization Code

Replace all instances of:
```cpp
if (!NimBLEDevice::isInitialized()) {
    NimBLEDevice::init("ATC_MiThermometer");
}
NimBLEScan* pScan = NimBLEDevice::getScan();
```

With:
```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    if (!NimBLEObserverOnly::isInitialized()) {
        NimBLEObserverOnly::init("ATC_MiThermometer");
    }
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
#else
    if (!NimBLEDevice::isInitialized()) {
        NimBLEDevice::init("ATC_MiThermometer");
    }
    NimBLEScan* pScan = NimBLEDevice::getScan();
#endif
```

### 3. Handle Whitelist Functions

The whitelist functions are NOT available in observer-only mode. Update code like:
```cpp
NimBLEDevice::getWhiteListCount();
NimBLEDevice::whiteListAdd(address);
```

To:
```cpp
#ifndef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    // Whitelist operations only available in full mode
    NimBLEDevice::getWhiteListCount();
    NimBLEDevice::whiteListAdd(address);
#else
    // In observer-only mode, whitelist is not supported
    // You could maintain your own filter list if needed
#endif
```

### 4. Create a Compatibility Header (Recommended)

Create `NimBLECompat.h` in your library:

```cpp
#ifndef NIMBLE_COMPAT_H
#define NIMBLE_COMPAT_H

#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
    #define NimBLECompat NimBLEObserverOnly
#else
    #include <NimBLEDevice.h>
    #define NimBLECompat NimBLEDevice
#endif

// Helper macros for features not available in observer-only mode
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #define NIMBLE_HAS_WHITELIST 0
    #define NIMBLE_HAS_CONNECTIONS 0
    #define NIMBLE_HAS_SERVER 0
#else
    #define NIMBLE_HAS_WHITELIST 1
    #define NIMBLE_HAS_CONNECTIONS 1
    #define NIMBLE_HAS_SERVER 1
#endif

#endif // NIMBLE_COMPAT_H
```

Then use it in your code:
```cpp
#include "NimBLECompat.h"

// Works in both modes:
if (!NimBLECompat::isInitialized()) {
    NimBLECompat::init("ATC_MiThermometer");
}
NimBLEScan* pScan = NimBLECompat::getScan();

// Conditional features:
#if NIMBLE_HAS_WHITELIST
    size_t count = NimBLEDevice::getWhiteListCount();
#endif
```

### 5. Complete Example Fix

Here's a complete example of how to fix a typical function:

**Before:**
```cpp
void setupBLE() {
    NimBLEDevice::init("ATC_MiThermometer");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setActiveScan(false);
    
    // Add whitelisted devices
    if (NimBLEDevice::getWhiteListCount() < 10) {
        NimBLEDevice::whiteListAdd(NimBLEAddress("AA:BB:CC:DD:EE:FF"));
    }
    
    pScan->start(0);
}
```

**After:**
```cpp
void setupBLE() {
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    NimBLEObserverOnly::init("ATC_MiThermometer");
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
#else
    NimBLEDevice::init("ATC_MiThermometer");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    
    // Whitelist only available in full mode
    if (NimBLEDevice::getWhiteListCount() < 10) {
        NimBLEDevice::whiteListAdd(NimBLEAddress("AA:BB:CC:DD:EE:FF"));
    }
#endif
    
    pScan->setActiveScan(false);
    pScan->start(0);
}
```

## Testing Your Changes

1. Test with standard NimBLE (no special defines)
2. Test with observer-only mode:
   ```cpp
   #define CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY 1
   #include "nimconfig_observer_only.h"
   // ... rest of your code
   ```

## Benefits

Supporting observer-only mode allows your users to:
- Save ~200-400KB of flash memory
- Reduce RAM usage significantly
- Still have full scanning functionality
- Use the same API (NimBLEScan, NimBLEAdvertisedDevice)

## Questions?

- Check the NimBLE-Arduino examples: `examples/Observer_Only_Test/`
- Review `OBSERVER_ONLY_BUILD_ISSUES.md` for technical details
- Open an issue on the NimBLE-Arduino GitHub repository