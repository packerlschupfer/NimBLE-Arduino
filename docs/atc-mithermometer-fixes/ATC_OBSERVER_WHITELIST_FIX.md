# ATC_MiThermometer Observer-Only Whitelist Issue

## Problem
The ATC example shows:
- "Whitelist contains 4294256908 entries" (uninitialized memory)
- Finds 0 devices even though observer-only scanning works

## Root Cause
The ATC_MiThermometer library is trying to get the whitelist count before NimBLE is fully initialized. Even though NimBLEObserverOnly checks for initialization, there might be a timing issue.

## Solution
The ATC_MiThermometer library needs to ensure NimBLE is initialized before calling any whitelist methods. The library should:

1. Call `NimBLEDevice::init()` before accessing whitelist
2. Add devices to the whitelist after initialization
3. Clear any previous whitelist entries before adding new ones

## Temporary Workaround
In the example's main.cpp, manually initialize NimBLE and populate the whitelist:

```cpp
// In setup(), before creating ATC_MiThermometer:
// Initialize NimBLE first
NimBLEDevice::init(DEVICE_NAME);

// Clear and populate whitelist
NimBLEDevice::whiteListClear();
for (const auto& addr : knownSensors) {
    NimBLEDevice::whiteListAdd(NimBLEAddress(addr));
}

// Then create ATC_MiThermometer
thermometer = new ATC_MiThermometer(knownSensors);
```

## Proper Fix
The ATC_MiThermometer library should be updated to:
1. Initialize NimBLE in its constructor or beginFiltered() method
2. Properly handle observer-only mode initialization
3. Check for valid whitelist state before accessing count