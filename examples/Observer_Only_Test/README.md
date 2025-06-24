# Observer-Only Mode Test Example

This example demonstrates NimBLE-Arduino's observer-only mode with hardware whitelist support.

## Features

- Observer-only mode (~200-400KB flash savings)
- Hardware whitelist filtering
- Initialization order testing
- Real BLE scanning with ATC sensor detection

## What This Example Tests

1. **Initialization Safety**: Verifies whitelist methods return safe values before NimBLE is initialized
2. **Whitelist Operations**: Tests add/remove/clear/count operations
3. **Compatibility Layer**: Uses `NimBLEDevice` API that redirects to `NimBLEObserverOnly`
4. **Real Scanning**: Performs actual BLE scans with and without whitelist filtering

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

--- Testing actual BLE scan ---
Starting 5 second scan for all devices...
Scan complete. Found X devices:
  [0] xx:xx:xx:xx:xx:xx, RSSI: -XX, Name: Device Name
  ...

--- Testing scan with whitelist ---
Added 3 addresses to whitelist
Starting 5 second scan with whitelist filter...
Filtered scan complete. Found Y devices:
  [0] a4:c1:38:1d:87:bb, RSSI: -XX [ATC Sensor]
  ...
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