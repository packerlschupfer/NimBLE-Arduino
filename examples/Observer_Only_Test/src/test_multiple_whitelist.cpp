#include <Arduino.h>
#include "nimconfig_observer_only.h"
#include "NimBLEDeviceCompat.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

// Test multiple whitelist entries like ATC_MiThermometer
void testMultipleWhitelist() {
    Serial.println("\n=== Testing Multiple Whitelist Entries ===");
    
    // Initialize
    NimBLEDevice::init("MultiTest");
    
    // Clear any existing whitelist
    if (NimBLEDevice::whiteListClear()) {
        Serial.println("Cleared existing whitelist");
    }
    
    // Test addresses (same as ATC example)
    const char* addresses[] = {
        "a4:c1:38:a1:03:ba",  // Sensor 1
        "a4:c1:38:1d:87:bb",  // Sensor 2  
        "a4:c1:38:07:51:82"   // Sensor 3
    };
    
    // Add each address
    Serial.println("\nAdding devices to whitelist:");
    for (int i = 0; i < 3; i++) {
        NimBLEAddress addr(addresses[i], BLE_ADDR_PUBLIC);
        bool success = NimBLEDevice::whiteListAdd(addr);
        Serial.printf("  [%d] %s: %s\n", i, addresses[i], success ? "SUCCESS" : "FAILED");
        
        // Check count after each add
        size_t count = NimBLEDevice::getWhiteListCount();
        Serial.printf("      Whitelist count: %zu\n", count);
    }
    
    // Verify final whitelist
    Serial.println("\nVerifying whitelist contents:");
    size_t finalCount = NimBLEDevice::getWhiteListCount();
    Serial.printf("Total entries: %zu\n", finalCount);
    
    for (size_t i = 0; i < finalCount; i++) {
        NimBLEAddress addr = NimBLEDevice::getWhiteListAddress(i);
        Serial.printf("  [%zu] %s\n", i, addr.toString().c_str());
    }
    
    // Test with scan
    Serial.println("\nTesting scan with whitelist...");
    NimBLEScan* pScan = NimBLEDevice::getScan();
    if (pScan) {
        pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
        Serial.println("Scan filter policy set to use whitelist");
        
        // Do a quick scan
        pScan->start(2, false);
        Serial.println("Scan completed");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== Multiple Whitelist Test ===");
    testMultipleWhitelist();
    Serial.println("\n=== Test Complete ===");
}

void loop() {
    delay(1000);
}