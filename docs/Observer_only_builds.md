# Observer-Only Builds Guide

This guide explains how to configure NimBLE-Arduino for observer-only (scanner-only) builds, which provide significant memory savings for applications that only need passive BLE scanning capabilities.

## Two Approaches

### 1. Configuration-Based (Moderate Savings: ~47-58KB)
Use configuration defines with standard NimBLEDevice initialization.

### 2. Minimal API (Maximum Savings: 200-400KB)
Use the new NimBLEObserverOnly interface for absolute minimal footprint.

## Overview

Observer-only builds disable all BLE features except passive scanning, making them ideal for:
- Beacon scanners
- Presence detection systems
- BLE traffic monitors
- Battery-powered scanning devices
- Memory-constrained applications

## Memory Savings

With proper configuration, observer-only builds can save:
- ~7KB by disabling CENTRAL role
- ~16KB by disabling PERIPHERAL role  
- ~5KB by disabling BROADCASTER role
- ~7KB by disabling L2CAP COC
- ~4-6KB by disabling BLE store (CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED)
- ~6-9KB by disabling privacy features (CONFIG_BT_NIMBLE_HOST_PRIVACY_DISABLED)
- ~2-3KB by disabling whitelist (CONFIG_BT_NIMBLE_WHITELIST_DISABLED)
- **Total potential savings: 47-58KB+ of flash memory**

## Basic Configuration

Add these defines to your `nimconfig.h` or before including NimBLE headers:

```cpp
// Disable all roles except OBSERVER
#define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED
#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED
#define CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED

// Disable L2CAP Connection Oriented Channels
#define CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM 0
```

## Advanced Configuration for Maximum Memory Savings

For the smallest possible footprint, add these additional settings:

```cpp
// Disable all security/encryption
#define MYNEWT_VAL_BLE_SM_LVL 0
#define CONFIG_BT_NIMBLE_SM_LEGACY 0
#define CONFIG_BT_NIMBLE_SM_SC 0
#define MYNEWT_VAL_BLE_SM_BONDING 0
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_ENCRYPTION 0

// Disable connection-related features
#define CONFIG_BT_NIMBLE_MAX_CONNECTIONS 0
#define CONFIG_BT_NIMBLE_MAX_BONDS 0
#define CONFIG_BT_NIMBLE_MAX_CCCDS 0
#define CONFIG_BT_NIMBLE_NVS_PERSIST 0

// Completely disable the BLE store (saves ~4-6KB)
#define CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED 1

// Disable privacy and address resolution features (saves ~6-9KB)
#define CONFIG_BT_NIMBLE_HOST_PRIVACY_DISABLED 1

// Disable whitelist feature (saves ~2-3KB)
#define CONFIG_BT_NIMBLE_WHITELIST_DISABLED 1

// Disable GATT procedures
#define CONFIG_BT_NIMBLE_GATT_MAX_PROCS 0
#define CONFIG_BT_NIMBLE_GATT_CACHING 0

// Disable extended/periodic advertising scanning if not needed
#define CONFIG_BT_NIMBLE_EXT_ADV 0
#define CONFIG_BT_NIMBLE_ENABLE_PERIODIC_ADV 0

// Reduce memory buffers
#define CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT 6
#define CONFIG_BT_NIMBLE_TRANSPORT_ACL_FROM_LL_COUNT 0
#define CONFIG_BT_NIMBLE_TRANSPORT_EVT_COUNT 8

// Disable all ATT server operations
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_INFO 0
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_TYPE 0
#define MYNEWT_VAL_BLE_ATT_SVR_INDICATE 0
#define MYNEWT_VAL_BLE_ATT_SVR_NOTIFY 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_BLOB 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_GROUP 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_MULT 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_TYPE 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE_CMD 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE_NO_RSP 0
#define MYNEWT_VAL_BLE_ATT_SVR_SIGNED_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_PREP_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_EXEC_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_NOTIFY_MULT 0
#define MYNEWT_VAL_BLE_ATT_SVR_QUEUED_WRITE 0
```

## What Remains Functional

In observer-only mode, these features remain available:
- Passive scanning (BLE advertisements)
- Advertisement data parsing
- RSSI measurements
- Address resolution (without encryption)
- Scan filtering by address, service UUID, etc.
- Duplicate filtering
- Scan timing configuration

## What Is Disabled

These features are NOT available in observer-only mode:
- Connecting to devices
- GATT client operations (reading/writing characteristics)
- GATT server (advertising services)
- Pairing/bonding
- Encryption
- Active scanning (scan requests)
- L2CAP channels
- Any form of bidirectional communication

## Example Usage

See the `examples/NimBLE_Observer_Only` directory for a complete example demonstrating:
- Minimal configuration for observer-only mode
- Efficient scanning with callbacks
- Advertisement data parsing
- Memory usage optimization

## Build Verification

To verify your observer-only build is properly configured:

1. Check the build output for disabled features
2. Monitor flash/RAM usage compared to a full build
3. Ensure scanning functionality works as expected
4. Verify connection attempts fail gracefully (if accidentally called)

## Troubleshooting

If you encounter compilation errors:

1. **"undefined reference to ble_gap_..."** - Ensure you haven't disabled the OBSERVER role
2. **"undefined reference to ble_sm_alg_encrypt"** - The encryption stub should handle this, ensure you're using the latest version
3. **L2CAP-related errors** - Set `CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM` to 0
4. **Missing error codes** - This should be fixed in recent versions, ensure ble_hs.h is included

## Platform-Specific Notes

- **ESP32**: All ESP32 variants support observer-only mode
- **Nordic nRF5x**: Supported with n-able Arduino core
- **Memory savings**: Actual savings may vary by platform and compiler optimization settings

## Migration from Full Build

To migrate an existing application to observer-only:

1. Remove all connection-related code
2. Remove GATT client/server operations
3. Choose your approach:
   - **For moderate savings**: Add configuration defines and continue using NimBLEDevice
   - **For maximum savings**: Switch to NimBLEObserverOnly API
4. Test thoroughly to ensure scanning still works as expected
5. Measure memory savings to verify optimization

## Using the Minimal API (NimBLEObserverOnly)

For maximum flash savings (200-400KB):

```cpp
#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"

void setup() {
    // Initialize observer-only mode
    NimBLEObserverOnly::init();
    
    // Get scanner
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    
    // Configure and start scanning
    pScan->setActiveScan(false);
    pScan->start(0); // 0 = continuous
}
```

### PlatformIO Configuration for Minimal Build

```ini
[env:esp32_observer_minimal]
platform = espressif32
board = esp32dev
framework = arduino
lib_deps = NimBLE-Arduino
build_flags = 
    -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    -Wl,--gc-sections
    -ffunction-sections
    -fdata-sections
```

The build system will automatically detect `CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY` and:
- Exclude ~140 unnecessary source files
- Apply aggressive optimization flags
- Link only scanning-related code

## Future Enhancements

The observer-only build support is actively being improved. Future enhancements may include:
- Additional conditional compilation optimizations
- Finer-grained feature control
- Platform-specific optimizations
- Runtime configuration options