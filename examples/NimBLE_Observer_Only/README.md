# NimBLE Observer-Only Example

This example demonstrates how to configure NimBLE-Arduino for observer-only mode, achieving maximum memory savings while maintaining BLE scanning functionality.

## Features

- **Minimal memory footprint**: ~40KB+ flash savings compared to full NimBLE build
- **Passive scanning only**: No connections, pairing, or GATT operations
- **Continuous scanning**: Detects all BLE advertisements in range
- **Device information display**: Shows address, RSSI, name, and manufacturer data
- **Memory statistics**: Reports heap usage to verify memory savings

## Configuration

The example uses extensive configuration defines before including NimBLE headers to disable unused features:

- All roles disabled except OBSERVER
- Security/encryption disabled
- Connection features disabled
- GATT/ATT operations disabled
- Memory buffers minimized

## What This Example Does

1. Initializes NimBLE in observer-only mode
2. Sets up passive BLE scanning
3. Continuously scans for BLE devices
4. Displays information about each discovered device
5. Reports memory usage statistics every 10 seconds

## Expected Output

```
NimBLE Observer-Only Example
============================
Initial free heap: 280124 bytes
Initial heap size: 380124 bytes
After init free heap: 265432 bytes
Setup complete. Starting continuous scan...

Device found: 11:22:33:44:55:66, RSSI: -75, Name: MyDevice
Device found: AA:BB:CC:DD:EE:FF, RSSI: -82, Manuf ID: 0x004C
Statistics: 127 devices found
Free heap: 264892 bytes
```

## Memory Comparison

Typical memory usage comparison:

| Configuration | Flash Usage | RAM Usage |
|--------------|-------------|-----------|
| Full NimBLE | ~440KB | ~70KB |
| Observer-Only | ~400KB | ~15KB |
| Savings | ~40KB | ~55KB |

*Note: Actual values depend on platform and compiler optimization*

## Use Cases

This configuration is ideal for:
- iBeacon/Eddystone scanners
- Presence detection systems
- BLE asset tracking receivers
- Contact tracing applications
- Any application that only needs to receive BLE advertisements

## Customization

To enable extended advertising scanning (BLE 5.0):
- Comment out `#define CONFIG_BT_NIMBLE_EXT_ADV 0`
- This will add ~3KB to flash usage but enable extended advertising features

To adjust scan parameters:
- `setInterval()`: How often to scan (lower = more frequent)
- `setWindow()`: Duration of each scan (should be ≤ interval)
- `setActiveScan()`: Must remain false for observer-only mode

## Limitations

In observer-only mode, you CANNOT:
- Connect to BLE devices
- Read/write characteristics
- Pair or bond with devices
- Act as a BLE server
- Send scan requests (active scanning)

## Troubleshooting

If compilation fails:
1. Ensure all defines are placed BEFORE `#include <NimBLEDevice.h>`
2. Check that you're using the latest NimBLE-Arduino with observer-only support
3. Verify your platform supports the configuration options used

## Further Reading

See the [Observer-Only Builds Guide](../../docs/Observer_only_builds.md) for detailed configuration options and additional optimization possibilities.