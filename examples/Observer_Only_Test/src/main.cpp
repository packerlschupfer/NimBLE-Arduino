#include <Arduino.h>
#include "nimconfig_observer_only.h"

// Use compatibility layer that redirects NimBLEDevice to NimBLEObserverOnly
#include "NimBLEDeviceCompat.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

// Test that mimics ATC_MiThermometer usage
void testCompatibility() {
    // These calls should work through the compatibility layer
    NimBLEDevice::init("Test");
    
    // Add to whitelist (like line 221 in ATC_MiThermometer)
    NimBLEAddress addr("A4:C1:38:1D:87:BB", BLE_ADDR_PUBLIC);
    if (NimBLEDevice::whiteListAdd(addr)) {
        Serial.println("Added to whitelist");
    }
    
    // Get whitelist count (like line 363)
    size_t count = NimBLEDevice::getWhiteListCount();
    Serial.printf("Whitelist count: %d\n", count);
    
    // Get whitelist address (like line 368)
    NimBLEAddress retrieved = NimBLEDevice::getWhiteListAddress(0);
    Serial.printf("First address: %s\n", retrieved.toString().c_str());
    
    // Remove from whitelist (like line 369)
    NimBLEDevice::whiteListRemove(addr);
    
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