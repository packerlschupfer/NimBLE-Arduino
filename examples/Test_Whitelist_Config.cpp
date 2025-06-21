/**
 * Test to verify CONFIG_BT_NIMBLE_WHITELIST_DISABLED functionality
 * 
 * This test demonstrates:
 * 1. Normal whitelist functionality when CONFIG_BT_NIMBLE_WHITELIST_DISABLED is not defined
 * 2. Code size reduction when CONFIG_BT_NIMBLE_WHITELIST_DISABLED is defined
 * 3. Observer mode still works without whitelist
 */

#include <Arduino.h>

// Uncomment the following line to test with whitelist disabled
// #define CONFIG_BT_NIMBLE_WHITELIST_DISABLED

#include "NimBLEDevice.h"
#include "NimBLEScan.h"

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.printf("Device found: %s\n", advertisedDevice->getAddress().toString().c_str());
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Observer Mode Test");

    // Initialize NimBLE
    NimBLEDevice::init("");

#if !defined(CONFIG_BT_NIMBLE_WHITELIST_DISABLED)
    Serial.println("Whitelist functions are ENABLED");
    
    // Test whitelist functionality
    NimBLEAddress testAddr("AA:BB:CC:DD:EE:FF");
    
    // Add to whitelist
    if (NimBLEDevice::whiteListAdd(testAddr)) {
        Serial.println("Added device to whitelist");
    }
    
    // Check if on whitelist
    if (NimBLEDevice::onWhiteList(testAddr)) {
        Serial.println("Device is on whitelist");
    }
    
    // Get whitelist count
    Serial.printf("Whitelist count: %d\n", NimBLEDevice::getWhiteListCount());
    
    // Remove from whitelist
    if (NimBLEDevice::whiteListRemove(testAddr)) {
        Serial.println("Removed device from whitelist");
    }
#else
    Serial.println("Whitelist functions are DISABLED - code size reduced!");
    Serial.println("Observer mode scanning without whitelist filter");
#endif

    // Set up scanning (works with or without whitelist)
    NimBLEScan* pBLEScan = NimBLEDevice::getScan();
    pBLEScan->setScanCallbacks(new MyScanCallbacks());
    pBLEScan->setActiveScan(false); // Passive scan for observer mode
    
#if !defined(CONFIG_BT_NIMBLE_WHITELIST_DISABLED)
    // When whitelist is enabled, we can choose to use it for filtering
    // BLE_HCI_SCAN_FILT_NO_WL (0) = no whitelist filter
    // BLE_HCI_SCAN_FILT_USE_WL (1) = use whitelist filter
    pBLEScan->setFilterPolicy(BLE_HCI_SCAN_FILT_NO_WL);
#endif

    Serial.println("Starting passive scan...");
    pBLEScan->start(10); // Scan for 10 seconds
}

void loop() {
    delay(1000);
}