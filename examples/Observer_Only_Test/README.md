# Observer-Only Mode Test Example

This example demonstrates NimBLE-Arduino's observer-only mode with hardware whitelist support.

## Features

- Observer-only mode (~200-400KB flash savings)
- Hardware whitelist filtering
- Proper BLE initialization timing
- Two-phase scanning (open vs whitelist)
- Real BLE scanning with ATC sensor detection

## What This Example Tests

1. **BLE Stack Timing**: Waits 2 seconds after init for BLE stack to stabilize
2. **Two-Phase Testing**: Runs tests in loop() instead of setup() for proper timing
3. **Whitelist Operations**: Tests add/remove/clear/count operations
4. **Scan Comparison**: Compares open scan vs whitelist-filtered results
5. **Real Scanning**: Performs actual BLE scans with efficiency metrics

## Expected Output

```
=== NimBLEDevice Compatibility Test ===

--- Testing whitelist BEFORE init ---
Whitelist count before init: 0 (should be 0)
Address at index 0 before init: 00:00:00:00:00:00 (should be 00:00:00:00:00:00)

--- Initializing NimBLE ---

--- Testing whitelist AFTER init ---
Whitelist count after init: 0 (should still be 0)
Added to whitelist
Whitelist count: 1
First address: a4:c1:38:1d:87:bb
Invalid index address: 00:00:00:00:00:00 (should be 00:00:00:00:00:00)
After remove, count: 0
Got scan object

Waiting 2 seconds for BLE stack to stabilize...

*** Main scanning tests will start in loop() ***

=== PHASE 1: Open Scan (10 seconds) ===
Should see ALL nearby BLE devices...

[Open 1] Device: 4c:11:ae:6c:65:0a, RSSI: -89
[Open 2] Device: a4:c1:38:1d:87:bb, RSSI: -68 <-- TEST DEVICE! [ATC Data]
...
Open scan complete. Total devices: 47

=== PHASE 2: Whitelist Scan (10 seconds) ===
Should ONLY see whitelisted devices...

[WL 1] Device: a4:c1:38:1d:87:bb, RSSI: -67 <-- TEST DEVICE! [ATC Data]
[WL 2] Device: a4:c1:38:07:51:82, RSSI: -72 <-- TEST DEVICE!

Whitelist scan complete. Total devices: 2

=== SUMMARY ===
Open scan found: 47 devices
Whitelist scan found: 2 devices
Whitelist efficiency: 95.7% reduction
```

## Configuration

The observer-only mode is configured via `platformio.ini`:

```ini
build_flags = 
    -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
    -I../../../src
```

## Whitelist Addresses

The example uses these test addresses:
- `A4:C1:38:1D:87:BB` - Test address 1
- `A4:C1:38:A1:03:BA` - Test address 2  
- `A4:C1:38:07:51:82` - Test address 3

Replace with your actual sensor MAC addresses for real testing.

## Memory Savings

Observer-only mode provides significant memory savings by excluding:
- GATT Client/Server
- Connection management
- Pairing/bonding
- Advertising

This results in ~200-400KB flash savings while keeping scanning functionality.