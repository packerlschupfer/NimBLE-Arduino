#include <Arduino.h>
#include "nimconfig_observer_only.h"

// Use compatibility layer that redirects NimBLEDevice to NimBLEObserverOnly
#include "NimBLEDeviceCompat.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

// Test that mimics ATC_MiThermometer usage
void testCompatibility() {
    // CRITICAL TEST: Check whitelist before init (should return 0, not garbage)
    Serial.println("\n--- Testing whitelist BEFORE init ---");
    size_t countBeforeInit = NimBLEDevice::getWhiteListCount();
    Serial.printf("Whitelist count before init: %zu (should be 0)\n", countBeforeInit);
    
    // Try to get address before init (should return invalid address)
    NimBLEAddress addrBeforeInit = NimBLEDevice::getWhiteListAddress(0);
    Serial.printf("Address at index 0 before init: %s (should be 00:00:00:00:00:00)\n", 
                  addrBeforeInit.toString().c_str());
    
    // Now initialize
    Serial.println("\n--- Initializing NimBLE ---");
    NimBLEDevice::init("Test");
    
    // Test again after init
    Serial.println("\n--- Testing whitelist AFTER init ---");
    size_t countAfterInit = NimBLEDevice::getWhiteListCount();
    Serial.printf("Whitelist count after init: %zu (should still be 0)\n", countAfterInit);
    
    // Add to whitelist (like line 221 in ATC_MiThermometer)
    NimBLEAddress addr("A4:C1:38:1D:87:BB", BLE_ADDR_PUBLIC);
    if (NimBLEDevice::whiteListAdd(addr)) {
        Serial.println("Added to whitelist");
    }
    
    // Get whitelist count (like line 363)
    size_t count = NimBLEDevice::getWhiteListCount();
    Serial.printf("Whitelist count: %zu\n", count);
    
    // Get whitelist address (like line 368)
    NimBLEAddress retrieved = NimBLEDevice::getWhiteListAddress(0);
    Serial.printf("First address: %s\n", retrieved.toString().c_str());
    
    // Test invalid index
    NimBLEAddress invalid = NimBLEDevice::getWhiteListAddress(99);
    Serial.printf("Invalid index address: %s (should be 00:00:00:00:00:00)\n", 
                  invalid.toString().c_str());
    
    // Remove from whitelist (like line 369)
    NimBLEDevice::whiteListRemove(addr);
    Serial.printf("After remove, count: %zu\n", NimBLEDevice::getWhiteListCount());
    
    // Get scan object
    NimBLEScan* pScan = NimBLEDevice::getScan();
    if (pScan) {
        Serial.println("Got scan object");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== NimBLEDevice Compatibility Test ===");
    testCompatibility();
    Serial.println("=== Test Complete ===");
}

void loop() {
    delay(1000);
}