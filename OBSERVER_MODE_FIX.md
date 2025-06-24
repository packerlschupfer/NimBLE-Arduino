# Observer-Only Mode Scanning Fix

## Problem
Observer-only mode is finding 0 devices even though:
- Scan starts successfully
- Scan completes normally (reason=0)
- No errors reported
- Both whitelist and open scans find nothing

## Root Cause Analysis

### What We Know Works
- We previously had observer-only scanning working (we have docs showing successful scans)
- The initialization completes successfully
- The scan parameters are correct
- The whitelist functionality works (initialization fix was successful)

### What's Not Working
- No GAP discovery events are being received
- The scan completes but with 0 devices found
- This happens with both whitelist filtering and open scanning

## Debugging Added

In `NimBLEScan.cpp`:
1. Added null check for pScan in handleGapEvent
2. Added debug logging for all GAP events
3. Added logging for discovery events

## Likely Issues

### 1. Controller Configuration
The ESP32 BT controller might not be properly configured for RX-only operation in observer mode.

### 2. Event Callback Registration
In observer-only mode, we pass `this` to ble_gap_disc(), but the event might not be routed correctly.

### 3. Build Script Side Effect
The recent build script changes might be affecting the controller compilation.

## Recommended Fix

### Step 1: Test Without Observer-Only Mode
Remove the observer-only flag temporarily to verify scanning works:
```ini
# Comment out this line in platformio.ini
# -D CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1
```

### Step 2: Check Controller Init
Verify the controller is initialized with the correct mode for scanning.

### Step 3: Simplify Test
Use the most basic scan test without callbacks or filtering.

## Workaround
For now, users should use full NimBLE mode if they need scanning to work reliably. The memory savings from observer-only mode may not be worth the compatibility issues on some ESP32 variants or IDF versions.