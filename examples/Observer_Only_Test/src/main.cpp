/**
 * Observer-Only Mode Test
 * 
 * This example demonstrates NimBLE-Arduino's observer-only mode which provides
 * significant memory savings (~200-400KB flash) by excluding unused BLE features.
 * 
 * Perfect for:
 * - BLE beacon scanners
 * - Sensor monitoring stations
 * - Presence detection systems
 * - Any application that only needs to receive BLE advertisements
 */

#include <Arduino.h>
#include "NimBLEObserverOnly.h"

// Track discovered devices
int deviceCount = 0;

// Custom scan callbacks
class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        deviceCount++;
        Serial.printf("Device %d: %s, RSSI: %d\n",
            deviceCount,
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
            
        // Check if it's an iBeacon
        if (advertisedDevice->isAdvertisingService(NimBLEUUID((uint16_t)0x1234))) {
            Serial.println("  -> Found target service!");
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== NimBLE Observer-Only Mode Example ===");
    
    // Initialize NimBLE in observer-only mode
    if (!NimBLEObserverOnly::init("ESP32-Observer")) {
        Serial.println("Failed to initialize NimBLE!");
        return;
    }
    
    Serial.println("NimBLE initialized in observer-only mode");
    Serial.printf("Flash savings: ~200-400KB compared to full NimBLE\n");
    
    // Get the scan object
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    
    // Set scan callbacks
    pScan->setScanCallbacks(new MyScanCallbacks());
    
    // Configure scan parameters
    pScan->setActiveScan(false);    // Passive scanning uses less power
    pScan->setInterval(100);         // How often to scan
    pScan->setWindow(99);            // How long to scan
    
    // Optional: Use hardware whitelist for specific devices
    // NimBLEObserverOnly::whiteListAdd(NimBLEAddress("aa:bb:cc:dd:ee:ff"));
    // pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
    
    // Start continuous scanning
    Serial.println("Starting continuous scan...\n");
    pScan->start(0, false);
}

void loop() {
    // Print status every 10 seconds
    static uint32_t lastPrint = 0;
    if (millis() - lastPrint > 10000) {
        lastPrint = millis();
        Serial.printf("\n=== Status: Found %d devices total ===\n\n", deviceCount);
    }
    
    delay(1000);
}