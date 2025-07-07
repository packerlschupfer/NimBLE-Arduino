# NimBLE Scanning API Documentation

This document describes the enhanced scanning features added to NimBLE-Arduino for better continuous scanning support.

## New Features

### 1. Scan State Management

#### Constants
```cpp
static constexpr uint32_t NimBLEScan::SCAN_DURATION_FOREVER = 0;
```
Use this constant to start an infinite scan that continues until explicitly stopped.

#### Methods

##### `bool isScanning() const`
Check if a scan is currently active.
```cpp
if (pBLEScan->isScanning()) {
    Serial.println("Scan is active");
}
```

##### `uint32_t getScanStartTime() const`
Get the timestamp (in milliseconds) when the current scan started.
```cpp
uint32_t startTime = pBLEScan->getScanStartTime();
Serial.printf("Scan started at: %d ms\n", startTime);
```

##### `uint32_t getScanDuration() const`
Get how long the current scan has been running (in milliseconds). Returns 0 if not scanning.
```cpp
uint32_t duration = pBLEScan->getScanDuration();
Serial.printf("Scanning for: %d seconds\n", duration / 1000);
```

### 2. Scan Statistics

#### Structure
```cpp
struct NimBLEScanStats {
    uint32_t beaconsReceived;     // Total beacons received
    uint32_t duplicatesFiltered;  // Duplicates filtered out
    uint32_t lastBeaconTime;      // Time of last beacon (ms)
    uint32_t scanStartTime;       // When current scan started (ms)
    uint32_t scanStopTime;        // When last scan stopped (ms)
};
```

#### Methods

##### `NimBLEScanStats getStats() const`
Get current scan statistics.
```cpp
NimBLEScanStats stats = pBLEScan->getStats();
Serial.printf("Beacons received: %d\n", stats.beaconsReceived);
Serial.printf("Duplicates filtered: %d\n", stats.duplicatesFiltered);
```

##### `void resetStats()`
Reset all scan statistics to zero.
```cpp
pBLEScan->resetStats();  // Clear statistics before starting new scan
```

### 3. Improved Error Handling

The `stop()` method now properly handles the `BLE_HS_EBUSY` (524) error that can occur when:
- A scan is already stopping
- The scan has already been stopped
- The BLE stack is busy

This error is now logged as a warning rather than an error, and the method will still return `true` as the scan will stop.

## Usage Examples

### Continuous Scanning
```cpp
// Start infinite scan
pBLEScan->start(NimBLEScan::SCAN_DURATION_FOREVER);

// Monitor scan progress
while (pBLEScan->isScanning()) {
    uint32_t duration = pBLEScan->getScanDuration();
    NimBLEScanStats stats = pBLEScan->getStats();
    
    Serial.printf("Scan time: %d s, Beacons: %d\n", 
        duration / 1000, stats.beaconsReceived);
    
    delay(5000);  // Update every 5 seconds
}
```

### Scan with Statistics
```cpp
// Reset statistics before scan
pBLEScan->resetStats();

// Start 30-second scan
pBLEScan->start(30);

// Wait for completion
while (pBLEScan->isScanning()) {
    delay(100);
}

// Get final statistics
NimBLEScanStats stats = pBLEScan->getStats();
float beaconsPerSecond = (float)stats.beaconsReceived / 30.0;

Serial.printf("Scan complete:\n");
Serial.printf("  Total beacons: %d\n", stats.beaconsReceived);
Serial.printf("  Duplicates: %d\n", stats.duplicatesFiltered);
Serial.printf("  Rate: %.1f beacons/sec\n", beaconsPerSecond);
```

### Phase-Locked Scanning Example
```cpp
// For libraries implementing phase-locked scanning:
// 1. Use continuous scan during learning phase
pBLEScan->start(NimBLEScan::SCAN_DURATION_FOREVER);

// 2. Monitor beacons in real-time via callbacks
class PhaseLockCallbacks : public NimBLEScanCallbacks {
    void onDiscovered(const NimBLEAdvertisedDevice* device) {
        // Process beacon immediately for timing analysis
        uint32_t now = millis();
        analyzeBeaconTiming(device, now);
    }
};

// 3. Stop scan when phase lock achieved
if (phaseLockAchieved) {
    pBLEScan->stop();  // Handles BLE_HS_EBUSY gracefully
}
```

## Best Practices

1. **Always reset statistics** before starting a new scan session for accurate measurements.

2. **Use `SCAN_DURATION_FOREVER`** for continuous scanning scenarios instead of repeatedly starting/stopping scans.

3. **Monitor scan duration** to implement timeouts or periodic actions during long scans.

4. **Check `isScanning()`** before calling `stop()` to avoid unnecessary operations.

5. **Handle the scan end callback** to know when a scan completes and why (timeout, manual stop, or error).

## Migration from Repeated Scans

If your code currently does this:
```cpp
// OLD: Repeated short scans with gaps
for (int i = 0; i < 10; i++) {
    pBLEScan->start(3);  // 3-second scan
    delay(3000);
    // Gap here where beacons are missed!
}
```

Change to continuous scanning:
```cpp
// NEW: Continuous scan without gaps
pBLEScan->start(30);  // Single 30-second scan
// Or use SCAN_DURATION_FOREVER and stop when needed
```

This eliminates gaps between scans where beacons could be missed.