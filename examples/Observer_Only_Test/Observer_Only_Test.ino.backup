/**
 * Observer-Only Build Test
 * 
 * This example demonstrates the minimal observer-only build configuration
 * that provides maximum memory savings (~200-400KB) by excluding all
 * unnecessary BLE features.
 * 
 * Include nimconfig_observer_only.h BEFORE any NimBLE headers!
 */

#include "nimconfig_observer_only.h"
#include <NimBLEObserverOnly.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>

class MyAdvertisedDeviceCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.printf("Device: %s, RSSI: %d", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
        
        if (advertisedDevice->haveName()) {
            Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
        }
        
        if (advertisedDevice->haveManufacturerData()) {
            std::string data = advertisedDevice->getManufacturerData();
            Serial.printf(", MfgData[%d]: ", data.length());
            for (size_t i = 0; i < data.length() && i < 8; i++) {
                Serial.printf("%02X ", (uint8_t)data[i]);
            }
            if (data.length() > 8) Serial.print("...");
        }
        
        Serial.println();
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        Serial.printf("Scan ended, reason: %d, found %d devices\n", 
            reason, results.getCount());
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); } // Wait for serial on some boards
    
    Serial.println("\n=== NimBLE Observer-Only Test ===");
    Serial.printf("Initial free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Initialize observer-only mode with error checking
    Serial.println("Initializing NimBLE Observer-Only mode...");
    if (!NimBLEObserverOnly::init("ESP32_Observer")) {
        Serial.println("ERROR: Failed to initialize NimBLE Observer!");
        Serial.println("Possible causes:");
        Serial.println("- Bluetooth not enabled in menuconfig");
        Serial.println("- Insufficient memory");
        Serial.println("- Controller initialization failed");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ NimBLE initialized successfully");
    Serial.printf("Free heap after init: %d bytes\n", ESP.getFreeHeap());
    
    // Verify initialization
    if (!NimBLEObserverOnly::isInitialized()) {
        Serial.println("ERROR: NimBLE reports not initialized!");
        while(1) { delay(1000); }
    }
    
    // Get the scan object with error checking
    NimBLEScan* pBLEScan = NimBLEObserverOnly::getScan();
    if (!pBLEScan) {
        Serial.println("ERROR: Failed to get scan object!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Got scan object successfully");
    
    // Print device address
    Serial.printf("Device address: %s\n", 
        NimBLEObserverOnly::getAddress().toString().c_str());
    
    // Set callbacks
    pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks(), false);
    
    // Configure scan parameters
    pBLEScan->setActiveScan(false); // Passive scan for lower power
    pBLEScan->setInterval(100);      // 100ms interval
    pBLEScan->setWindow(50);         // 50ms window
    pBLEScan->setMaxResults(0);      // Don't store results (callback only)
    
    // Start scanning with error checking
    Serial.println("Starting BLE scan...");
    if (!pBLEScan->start(0)) {  // 0 = scan forever
        Serial.println("ERROR: Failed to start scan!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Scanning started successfully\n");
}

void loop() {
    delay(10000); // 10 seconds
    
    // Print status periodically
    Serial.println("\n--- Status Report ---");
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Initialized: %s\n", 
        NimBLEObserverOnly::isInitialized() ? "Yes" : "No");
    
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (pScan) {
        Serial.printf("Scanning: %s\n", pScan->isScanning() ? "Yes" : "No");
        
        // Optional: Get scan results if storing them
        // NimBLEScanResults results = pScan->getResults();
        // Serial.printf("Devices in cache: %d\n", results.getCount());
    }
    
    Serial.println("--------------------\n");
}