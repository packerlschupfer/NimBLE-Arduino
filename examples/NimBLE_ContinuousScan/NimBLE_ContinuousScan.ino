/**
 * NimBLE Continuous Scan Example
 * 
 * This example demonstrates how to use the new continuous scanning features
 * including scan state management, statistics tracking, and proper error handling.
 * 
 * Features demonstrated:
 * - Continuous scanning with SCAN_DURATION_FOREVER
 * - Scan state monitoring with isScanning(), getScanDuration()
 * - Scan statistics tracking (beacons received, duplicates)
 * - Proper BLE_HS_EBUSY error handling
 * - Real-time beacon counting
 */

#include <NimBLEDevice.h>

// Scan configuration
const uint16_t SCAN_INTERVAL_MS = 100;  // How often to scan
const uint16_t SCAN_WINDOW_MS = 99;     // How long to scan for each interval
const bool ACTIVE_SCAN = false;         // Passive scanning uses less power

// Display configuration  
const uint32_t STATS_INTERVAL_MS = 5000;  // Show stats every 5 seconds
const uint32_t SCAN_DURATION_MS = 30000;  // Total scan time (30 seconds)

NimBLEScan* pBLEScan;
uint32_t scanStartTime = 0;
uint32_t lastStatsTime = 0;
uint32_t deviceCount = 0;

// Scan callbacks
class MyScanCallbacks: public NimBLEScanCallbacks {
    void onDiscovered(const NimBLEAdvertisedDevice* advertisedDevice) {
        // Called immediately when a device is discovered
        // This is called before scan response (if any) is received
        Serial.printf("Discovered: %s, RSSI: %d dBm\n", 
            advertisedDevice->getAddress().toString().c_str(), 
            advertisedDevice->getRSSI());
    }
    
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        // Called when scan result is complete (includes scan response if available)
        deviceCount++;
        
        // Show device details
        Serial.printf("Device %d: %s", deviceCount, advertisedDevice->toString().c_str());
        if (advertisedDevice->haveName()) {
            Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
        }
        if (advertisedDevice->haveServiceUUID()) {
            Serial.printf(", UUID: %s", advertisedDevice->getServiceUUID().toString().c_str());
        }
        Serial.println();
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        // Called when scan ends
        Serial.println("\n====================================");
        Serial.printf("Scan ended. Reason: %d\n", reason);
        Serial.printf("Total devices found: %d\n", results.getCount());
        Serial.println("====================================\n");
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Continuous Scan Example...");

    // Initialize NimBLE
    NimBLEDevice::init("ESP32-Scanner");
    
    // Get the scan object
    pBLEScan = NimBLEDevice::getScan();
    
    // Set scan callbacks
    pBLEScan->setScanCallbacks(new MyScanCallbacks(), false);  // false = don't want duplicates
    
    // Configure scan parameters
    pBLEScan->setInterval(SCAN_INTERVAL_MS);
    pBLEScan->setWindow(SCAN_WINDOW_MS);
    pBLEScan->setActiveScan(ACTIVE_SCAN);
    pBLEScan->setMaxResults(0);  // 0 = unlimited results
    
    Serial.println("\nScan Configuration:");
    Serial.printf("- Interval: %d ms\n", SCAN_INTERVAL_MS);
    Serial.printf("- Window: %d ms\n", SCAN_WINDOW_MS);
    Serial.printf("- Active scan: %s\n", ACTIVE_SCAN ? "Yes" : "No");
    Serial.printf("- Duration: %d seconds\n", SCAN_DURATION_MS / 1000);
    Serial.println("\nStarting continuous scan...\n");
    
    // Reset statistics before starting
    pBLEScan->resetStats();
    
    // Start continuous scan
    scanStartTime = millis();
    lastStatsTime = scanStartTime;
    
    // Use SCAN_DURATION_FOREVER for infinite scanning, or specify duration
    if (!pBLEScan->start(NimBLEScan::SCAN_DURATION_FOREVER)) {
        Serial.println("Failed to start scan!");
    }
}

void loop() {
    // Check if we should display statistics
    if (millis() - lastStatsTime >= STATS_INTERVAL_MS) {
        displayStats();
        lastStatsTime = millis();
    }
    
    // Check if we should stop scanning
    if (millis() - scanStartTime >= SCAN_DURATION_MS) {
        if (pBLEScan->isScanning()) {
            Serial.println("\nStopping scan...");
            
            // Stop the scan - now handles BLE_HS_EBUSY gracefully
            if (pBLEScan->stop()) {
                Serial.println("Scan stopped successfully.");
            } else {
                Serial.println("Failed to stop scan.");
            }
            
            // Display final statistics
            displayStats();
            
            // Get final results
            NimBLEScanResults results = pBLEScan->getResults();
            Serial.printf("\nFinal device count: %d\n", results.getCount());
            
            // Print all discovered devices
            Serial.println("\nDiscovered devices:");
            for (int i = 0; i < results.getCount(); i++) {
                const NimBLEAdvertisedDevice* device = results.getDevice(i);
                Serial.printf("%d. %s, RSSI: %d", 
                    i + 1, 
                    device->getAddress().toString().c_str(), 
                    device->getRSSI());
                if (device->haveName()) {
                    Serial.printf(", Name: %s", device->getName().c_str());
                }
                Serial.println();
            }
            
            // Wait before restarting
            Serial.println("\nWaiting 10 seconds before next scan...");
            delay(10000);
            
            // Reset for next scan
            deviceCount = 0;
            pBLEScan->clearResults();
            pBLEScan->resetStats();
            scanStartTime = millis();
            lastStatsTime = scanStartTime;
            
            Serial.println("\nStarting new scan...\n");
            pBLEScan->start(NimBLEScan::SCAN_DURATION_FOREVER);
        }
    }
    
    delay(100);  // Small delay to prevent watchdog issues
}

void displayStats() {
    if (!pBLEScan->isScanning()) {
        return;
    }
    
    // Get current statistics
    NimBLEScanStats stats = pBLEScan->getStats();
    uint32_t scanDuration = pBLEScan->getScanDuration();
    
    Serial.println("\n--- Scan Statistics ---");
    Serial.printf("Scan duration: %d.%d seconds\n", 
        scanDuration / 1000, (scanDuration % 1000) / 100);
    Serial.printf("Beacons received: %d\n", stats.beaconsReceived);
    Serial.printf("Duplicates filtered: %d\n", stats.duplicatesFiltered);
    Serial.printf("Unique devices: %d\n", deviceCount);
    
    if (stats.beaconsReceived > 0) {
        float beaconsPerSecond = (float)stats.beaconsReceived / (scanDuration / 1000.0);
        Serial.printf("Beacons/second: %.1f\n", beaconsPerSecond);
    }
    
    if (stats.lastBeaconTime > 0) {
        uint32_t timeSinceLastBeacon = millis() - stats.lastBeaconTime;
        Serial.printf("Time since last beacon: %d ms\n", timeSinceLastBeacon);
    }
    
    Serial.println("---------------------\n");
}