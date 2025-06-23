# NimBLE Observer-Only Mode Update Guide

## Overview
This guide helps applications update to use the optimized NimBLE observer-only mode, which saves 79KB of flash memory.

## Quick Check: Is Your Application Already Updated?

### ✅ Your application is already updated if:
1. You have `-D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1` in platformio.ini
2. You're using `#include "NimBLEObserverOnly.h"` instead of `NimBLEDevice.h`
3. You're initializing with `NimBLEObserverOnly::init()` instead of `NimBLEDevice::init()`

**Example: ESPlan-blueprint-Boiler-Controller-MB8ART-workspace is already fully updated!**

## Update Instructions for Applications Still Using NimBLEDevice

### Step 1: Update platformio.ini Build Flags

Add these flags to your `build_flags`:

```ini
; Enable observer-only mode
-D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1

; Disable unused roles
-D CONFIG_BT_NIMBLE_ROLE_CENTRAL=0
-D CONFIG_BT_NIMBLE_ROLE_PERIPHERAL=0
-D CONFIG_BT_NIMBLE_ROLE_BROADCASTER=0
-D CONFIG_BT_NIMBLE_ROLE_OBSERVER=1

; Disable unused features
-D CONFIG_BT_NIMBLE_MAX_CONNECTIONS=0
-D CONFIG_BT_NIMBLE_SM_LEGACY=0
-D CONFIG_BT_NIMBLE_SM_SC=0
-D CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM=0
```

### Step 2: Update Your Code

#### Option A: Minimal Changes (Using NimBLEDevice)
If your code uses `NimBLEDevice::init()` and `NimBLEDevice::getScan()`:

1. Just add the build flags from Step 1
2. No code changes needed!
3. You'll get most of the savings (60-70KB)

#### Option B: Maximum Savings (Using NimBLEObserverOnly)
For maximum 79KB savings, update your code:

**Before:**
```cpp
#include <NimBLEDevice.h>

void setup() {
    NimBLEDevice::init("");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    pScan->setActiveScan(false);
    pScan->start(0);
}
```

**After:**
```cpp
#include "NimBLEObserverOnly.h"

void setup() {
    NimBLEObserverOnly::init("");
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    pScan->setActiveScan(false);
    pScan->start(0);
}
```

### Step 3: Create nimconfig.h (Optional)

For better control, create `src/nimconfig.h`:

```cpp
#pragma once

// Force observer-only mode
#define CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY 1

// Include the library's default config
#include_next <nimconfig.h>
```

## Common Use Cases

### MiThermometer Sensor (ATC_MiThermometer)
```cpp
#include "NimBLEObserverOnly.h"
#include <ATC_MiThermometer.h>

ATC_MiThermometer miThermometer;

void setup() {
    // Initialize observer-only mode
    NimBLEObserverOnly::init("ESP32-Scanner");
    
    // Configure whitelist (optional)
    std::vector<std::string> whitelist = {
        "A4:C1:38:XX:XX:XX"  // Your sensor MAC
    };
    miThermometer.setWhitelist(whitelist);
    
    // Start scanning
    miThermometer.begin();
}
```

### Generic BLE Scanner
```cpp
#include "NimBLEObserverOnly.h"

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.printf("Device: %s, RSSI: %d\n", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
    }
};

void setup() {
    NimBLEObserverOnly::init("Scanner");
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    pScan->setScanCallbacks(new MyScanCallbacks());
    pScan->setActiveScan(false);
    pScan->start(0);
}
```

## Troubleshooting

### Compilation Errors

**Error:** `undefined reference to NimBLEClient::connect`
**Solution:** Your code is trying to connect. Observer-only mode can't connect to devices. Remove connection code or don't use observer-only mode.

**Error:** `NimBLEObserverOnly.h: No such file`
**Solution:** Update to the latest NimBLE-Arduino library that includes observer-only support.

### Runtime Issues

**Issue:** Scanning doesn't find any devices
**Check:** 
- BLE is enabled: `-D CONFIG_BT_ENABLED=1`
- Observer role is enabled: `-D CONFIG_BT_NIMBLE_ROLE_OBSERVER=1`
- Not accidentally disabled: `-D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1`

## Memory Savings Summary

| Configuration | Flash Usage | Savings |
|--------------|-------------|---------|
| Full NimBLE (all features) | 605KB | - |
| With build flags only | ~540KB | 65KB |
| With NimBLEObserverOnly | 526KB | 79KB |

## Projects Already Updated

- ✅ ESPlan-blueprint-Boiler-Controller-MB8ART-workspace
- ✅ [Add your project here after updating]

## Need Help?

1. Check if your use case needs features beyond scanning
2. If you need to connect to devices, you can't use observer-only mode
3. For scanning-only applications, observer-only mode is perfect!

## Migration Checklist

- [ ] Add observer-only build flags to platformio.ini
- [ ] Update includes from NimBLEDevice.h to NimBLEObserverOnly.h (optional)
- [ ] Change init calls from NimBLEDevice to NimBLEObserverOnly (optional)
- [ ] Test scanning functionality
- [ ] Verify memory savings with build output
- [ ] Update documentation