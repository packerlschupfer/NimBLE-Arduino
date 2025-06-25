# ATC_MiThermometer Whitelist Troubleshooting

## Issue: No Devices Found with Whitelist

If your ATC_MiThermometer scan is finding 0 devices even though the whitelist is configured correctly, here are the common causes and solutions:

### 1. Verify MAC Addresses

The most common issue is incorrect MAC addresses. Make sure:
- The MAC addresses in your code match your actual sensors
- Use lowercase letters (e.g., `a4:c1:38:1d:87:bb` not `A4:C1:38:1D:87:BB`)

To find your sensor addresses:
1. Temporarily disable whitelist filtering:
   ```cpp
   thermometer->beginFiltered(false, false, AddressType::AUTO_DETECT);
   ```
2. Run a scan to see all nearby devices
3. Note the correct addresses of your ATC sensors
4. Re-enable whitelist with correct addresses

### 2. Check Address Type

BLE devices can use different address types:
- **Public** - Fixed hardware address
- **Random** - Changes periodically

The ATC example uses `AddressType::AUTO_DETECT` which should handle this automatically, but you can try specifying:
```cpp
thermometer->beginFiltered(false, true, AddressType::ADDR_TYPE_PUBLIC);
// or
thermometer->beginFiltered(false, true, AddressType::ADDR_TYPE_RANDOM);
```

### 3. Verify Sensors Are Advertising

Make sure your sensors are:
- Powered on with fresh batteries
- Running ATC_MiThermometer firmware
- Within range (typically 10-20 meters)
- Not connected to another device

### 4. Error "Failed to cancel scan; rc=524"

This error is harmless and can be ignored. It means:
- Error 524 = `BLE_HS_EALREADY` 
- The scan already completed when stop() was called
- This is normal behavior

### 5. Debug Output Analysis

Your debug output shows:
```
Whitelist contains 3 entries for 3 devices
Scan complete - found 0 devices
```

This indicates:
- ✓ Whitelist is configured correctly
- ✓ Hardware filtering is active
- ✗ No matching devices were seen

### 6. Test with Our Example

Try the Observer_Only_Test example first:
```bash
cd examples/Observer_Only_Test
pio run -t upload
pio device monitor
```

This will show:
1. All devices during open scan
2. Only whitelisted devices during filtered scan

### 7. Common MAC Address Mistakes

```cpp
// WRONG - uppercase
"A4:C1:38:1D:87:BB"

// WRONG - missing colons
"a4c1381d87bb"

// WRONG - dashes instead of colons
"a4-c1-38-1d-87-bb"

// CORRECT
"a4:c1:38:1d:87:bb"
```

### 8. Recommended Troubleshooting Steps

1. **First**: Run without whitelist to find devices
2. **Second**: Verify MAC addresses match exactly
3. **Third**: Check sensor battery levels
4. **Fourth**: Try different address types
5. **Fifth**: Move closer to sensors

### Example Debug Code

Add this to your setup to help debug:
```cpp
// Test scan without whitelist first
thermometer->beginFiltered(false, false, AddressType::AUTO_DETECT);
unsigned foundCount = thermometer->getData(5000);
Serial.printf("Open scan found %u devices\n", foundCount);

// List what was found
for (int i = 0; i < knownSensors.size(); i++) {
    if (thermometer->data[i].valid) {
        Serial.printf("Found sensor %d: %s\n", i, knownSensors[i].c_str());
    }
}

// Now enable whitelist
thermometer->beginFiltered(false, true, AddressType::AUTO_DETECT);
```