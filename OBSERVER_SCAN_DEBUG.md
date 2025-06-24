# Observer-Only Mode Scan Debug Analysis

## Problem Summary

Both our Observer_Only_Test example and ATC_MiThermometer are finding 0 devices when scanning in observer-only mode, even without whitelist filtering.

## Key Observations

1. **Scan completes successfully** - No errors, reason=0 (normal completion)
2. **No devices found** - Even with no whitelist filter
3. **Callbacks not triggered** - onAdvertisement/onResult never called
4. **Timing is correct** - Scan runs for full duration
5. **Initialization succeeds** - Controller and host sync properly

## Potential Root Causes

### 1. Controller Configuration Issue

The controller might not be properly configured for receive-only operation in observer mode:
- Radio not enabled for RX
- PHY not configured
- Channel hopping disabled

### 2. Event Routing Problem

The GAP events might not be reaching the scan handler:
- In observer-only mode, events are passed with `this` pointer
- Normal mode uses global NimBLEDevice::getScan()
- Event handler might not be registered correctly

### 3. ESP32 Specific Issue

The ESP32 controller might require additional configuration for observer-only:
- BT controller mode needs specific flags
- RX buffers not allocated
- Power management interfering

## Debug Steps Taken

1. Added callback debugging - callbacks never fire
2. Checked scan parameters - correct (100ms interval, 99ms window)
3. Verified initialization - controller and host sync OK
4. Tested both whitelist and open scan - both find 0 devices

## Likely Issue

The most likely issue is that the ESP32 BT controller is not actually receiving advertisements in observer-only mode. This could be because:

1. The controller needs different initialization for RX-only operation
2. The scan start command isn't enabling the radio
3. There's a missing configuration flag

## Next Steps

1. **Check controller logs** - Enable BT controller debug output
2. **Test with full mode** - Verify devices are actually advertising
3. **Check ESP-IDF version** - Some versions may have observer mode bugs
4. **Add HCI event logging** - See if controller is sending any events

## Workaround

For now, users experiencing this issue should:
1. Use full NimBLE mode instead of observer-only
2. The memory savings may not be worth the compatibility issues
3. Report the issue with ESP32 model and IDF version