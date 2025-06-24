# Observer-Only Mode Scan Fix

## Problem Description
Observer-only mode was finding 0 devices during scans. The issue was that the scan callback mechanism in observer-only mode was not properly handling the callback argument passed to the GAP event handler.

## Root Cause
In observer-only mode, `NimBLEScan::handleGapEvent()` was being called with `this` as the argument (passed to `ble_gap_disc()`), but the function was trying to cast this argument to `NimBLEScan*`. However, due to how the callback was registered and the lack of a proper instance reference, the scan events were not being processed correctly.

## Solution
The fix uses a global static pointer to track the current scan instance in observer-only mode:

1. Added a static global pointer `g_observerScan` that holds the current `NimBLEScan` instance
2. Set this pointer in the constructor when in observer-only mode
3. Clear it in the destructor
4. Modified `handleGapEvent` to use the global pointer instead of the callback argument
5. Always pass `NULL` as the callback argument to `ble_gap_disc()` to avoid confusion

## Code Changes

### In NimBLEScan.cpp:

```cpp
// Added after defaultScanCallbacks
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
// Global scan instance for observer-only mode
static NimBLEScan* g_observerScan = nullptr;
#endif

// In constructor
NimBLEScan::NimBLEScan() {
    // ... existing code ...
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    // In observer-only mode, we need a global reference
    g_observerScan = this;
#endif
}

// In destructor
NimBLEScan::~NimBLEScan() {
    clearResults();
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    if (g_observerScan == this) {
        g_observerScan = nullptr;
    }
#endif
}

// In handleGapEvent
#ifdef CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    // Use the global scan instance
    NimBLEScan* pScan = g_observerScan;
    if (pScan == nullptr) {
        NIMBLE_LOGE(LOG_TAG, "CRITICAL: g_observerScan is null!");
        return 0;
    }
#else
    // ... existing code ...
#endif

// In start() - always pass NULL
int rc = ble_gap_disc(..., 
                      NimBLEScan::handleGapEvent,
                      NULL);  // Always use NULL
```

## Test Results
After applying the fix:
- GAP events are now being received correctly in observer-only mode
- The scan instance is properly retrieved using the global pointer
- Devices are being discovered and processed
- Observer-only mode scanning is now functional

## Important Note
The fix ensures that scan events are delivered to the NimBLEScan instance in observer-only mode. Users should verify that their custom scan callbacks are being invoked correctly. If using the default callbacks with debug logging enabled, you will see "NimBLEScanCallbacks" messages in the log output.

## Memory Impact
No additional memory overhead - the fix only adds a single static pointer (4-8 bytes) in observer-only builds.

## Compatibility
The fix maintains full backward compatibility and does not affect normal (non-observer-only) builds.