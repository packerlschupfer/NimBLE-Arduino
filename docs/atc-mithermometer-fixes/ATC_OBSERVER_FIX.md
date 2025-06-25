# ATC_MiThermometer Observer-Only Mode Fix

## Problem
ATC_MiThermometer finds 0 devices in observer-only mode because it's using `NimBLEDevice::init()` which doesn't work in observer-only builds. In observer-only mode, we must use `NimBLEObserverOnly::init()` instead.

## Root Cause
The ATC_MiThermometer library's `beginFiltered()` method calls:
```cpp
if (!NimBLEDevice::isInitialized()) {
    if (!NimBLEDevice::init("ble-scan")) {  // <-- This doesn't work in observer-only mode!
        ATC_LOG_E("Failed to initialize NimBLE!");
        return;
    }
}
```

But in observer-only mode, `NimBLEDevice` is not available. We need to use `NimBLEObserverOnly` instead.

## Solution Options

### Option 1: Modify ATC_MiThermometer Library (Recommended)
Add conditional compilation to support both modes:

```cpp
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
    #define NIMBLE_DEVICE NimBLEObserverOnly
#else
    #include "NimBLEDevice.h"
    #define NIMBLE_DEVICE NimBLEDevice
#endif

// Then in beginFiltered():
if (!NIMBLE_DEVICE::isInitialized()) {
    if (!NIMBLE_DEVICE::init("ble-scan")) {
        ATC_LOG_E("Failed to initialize NimBLE!");
        return;
    }
}
_pBLEScan = NIMBLE_DEVICE::getScan();
```

### Option 2: Pre-initialize in Application
Initialize NimBLE before creating ATC_MiThermometer instance:

```cpp
// In main.cpp setup():
// Initialize NimBLE first
if (!NimBLEObserverOnly::init(DEVICE_NAME)) {
    Serial.println("Failed to initialize NimBLE!");
    return;
}

// Then create ATC_MiThermometer instance
thermometer = new ATC_MiThermometer(knownSensors);

// beginFiltered will skip initialization since it's already done
thermometer->beginFiltered(false, true, AddressType::AUTO_DETECT);
```

### Option 3: Create Compatibility Header
Create a header that provides unified API for both modes:

```cpp
// NimBLECompat.h
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
    namespace NimBLEDevice {
        inline bool init(const std::string& name) { return NimBLEObserverOnly::init(name); }
        inline bool isInitialized() { return NimBLEObserverOnly::getInitialized(); }
        inline NimBLEScan* getScan() { return NimBLEObserverOnly::getScan(); }
        // ... other methods
    }
#else
    #include "NimBLEDevice.h"
#endif
```

## Quick Fix for Testing
For immediate testing, modify the main.cpp example to pre-initialize:

```cpp
// Add this before creating ATC_MiThermometer instance:
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    #include "NimBLEObserverOnly.h"
    if (!NimBLEObserverOnly::init(DEVICE_NAME)) {
        Serial.println("Failed to initialize NimBLE!");
        return;
    }
#endif
```