# Fix for ATC_MiThermometer in Observer-Only Mode

## The Problem

When using ATC_MiThermometer with observer-only mode, the whitelist shows corrupted values (e.g., -1842553072 entries) even though:
1. The sensors are working
2. The MAC addresses are correct  
3. Our whitelist implementation works with multiple devices

## The Root Cause

**Fixed in commit 344314ee**: The issue was that `whiteListSize()` and `getWhiteListAddress()` didn't check if NimBLE was initialized before accessing the static whitelist vector. This could return garbage values from uninitialized memory.

## The Workaround (Before Fix)

Initialize NimBLE explicitly before creating ATC_MiThermometer:

```cpp
void setup() {
    Serial.begin(115200);
    // ... other setup ...
    
    // Initialize NimBLE FIRST in observer-only mode
    Serial.println("Initializing NimBLE in observer-only mode...");
    NimBLEDevice::init("ESP32 ATC Scanner");
    
    // NOW create the ATC_MiThermometer instance
    Serial.println("Creating ATC_MiThermometer instance...");
    thermometer = new ATC_MiThermometer(knownSensors);
    
    // Then call beginFiltered() as normal
    thermometer->beginFiltered(false, true, AddressType::AUTO_DETECT);
}
```

## Why This Happened

The ATC_MiThermometer library correctly initializes NimBLE in its `beginFiltered()` method, but it queries the whitelist size immediately after initialization. The static whitelist vector might not be fully initialized at this point, leading to garbage values.

The fix adds initialization checks to prevent accessing the vector before NimBLE is ready.

## Note

This was not a bug in ATC_MiThermometer - the library correctly checks `isInitialized()` and initializes NimBLE if needed. The issue was in our observer-only implementation not guarding against early access to static members.