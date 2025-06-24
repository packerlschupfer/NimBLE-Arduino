# Observer-Only Mode Scanning Fix

## Problem Solved
Observer-only mode was finding 0 devices even though scans completed successfully.

## Root Cause
The ESP32 BT controller requires at least 1 connection slot to be allocated, even when only using observer (passive scanning) functionality. Setting `ble_max_conn=0` causes the controller initialization to fail with `ESP_ERR_INVALID_ARG` (error 258).

## Solution
1. **Controller Configuration**: Force minimum of 1 connection slot for all ESP32 variants:
   ```cpp
   bt_cfg.ble_max_conn = CONFIG_BT_NIMBLE_MAX_CONNECTIONS > 0 ? CONFIG_BT_NIMBLE_MAX_CONNECTIONS : 1;
   ```

2. **Transport Buffers**: Increased buffers to allow scan events:
   - `CONFIG_BT_NIMBLE_TRANSPORT_ACL_FROM_LL_COUNT`: 0 → 4
   - `CONFIG_BT_NIMBLE_TRANSPORT_EVT_COUNT`: 4 → 8

## Results
- Observer-only mode now successfully finds BLE devices
- Memory impact is minimal (still ~200-400KB savings vs full mode)
- Hardware whitelist filtering works correctly
- Both open and filtered scans function properly

## Technical Details
The ESP32 controller uses the connection infrastructure for:
- Managing scan state
- Buffering advertisement reports
- Routing events to the host
- Managing whitelist filters

Even in receive-only mode, this infrastructure must be initialized.

## Verification
The Observer_Only_Test example now successfully:
- Initializes in observer-only mode
- Performs passive BLE scanning
- Finds and reports all nearby devices
- Supports hardware whitelist filtering
- Uses minimal memory (~42% flash, ~10% RAM)