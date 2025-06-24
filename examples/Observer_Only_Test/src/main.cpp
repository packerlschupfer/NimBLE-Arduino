#include <Arduino.h>
#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

// Target device for whitelist
const char* TARGET_DEVICE = "A4:C1:38:1D:87:BB";

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        String addr = advertisedDevice->getAddress().toString().c_str();
        Serial.printf("Device: %s, RSSI: %d", addr.c_str(), advertisedDevice->getRSSI());
        
        if (advertisedDevice->haveName()) {
            Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
        }
        
        // Show if this was our whitelisted device
        if (addr.equalsIgnoreCase(TARGET_DEVICE)) {
            Serial.print(" <-- WHITELISTED DEVICE");
        }
        
        Serial.println();
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000) { delay(10); }
    
    Serial.println("\n=== Observer-Only Whitelist Test ===");
    Serial.printf("Target device: %s\n", TARGET_DEVICE);
    Serial.printf("Initial free heap: %d bytes\n\n", ESP.getFreeHeap());
    
    // Initialize observer-only mode
    if (!NimBLEObserverOnly::init("Observer-Whitelist-Test")) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Initialized successfully");
    
    // Add target device to whitelist
    NimBLEAddress targetAddr(TARGET_DEVICE, BLE_ADDR_PUBLIC);
    if (NimBLEObserverOnly::whiteListAdd(targetAddr)) {
        Serial.printf("✓ Added %s to whitelist\n", TARGET_DEVICE);
    } else {
        Serial.println("ERROR: Failed to add device to whitelist!");
    }
    
    Serial.printf("Whitelist size: %d\n", NimBLEObserverOnly::whiteListSize());
    
    // Get scanner instance
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (!pScan) {
        Serial.println("ERROR: Failed to get scan object!");
        while(1) { delay(1000); }
    }
    
    // Set callbacks
    pScan->setScanCallbacks(new MyScanCallbacks(), false);
    
    // Configure passive scanning with whitelist filter
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setMaxResults(0); // Don't store, callback only
    
    // Use whitelist filter policy
    pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
    Serial.println("✓ Scan filter policy set to USE_WHITELIST");
    
    // Start scanning
    Serial.println("\nStarting scan with whitelist filter...");
    Serial.println("Only devices in whitelist should appear:\n");
    
    if (!pScan->start(0, false)) {
        Serial.println("ERROR: Failed to start scan!");
        while(1) { delay(1000); }
    }
}

void loop() {
    static uint32_t lastStatus = 0;
    
    // Status report every 30 seconds
    if (millis() - lastStatus > 30000) {
        lastStatus = millis();
        Serial.println("\n--- Status Report ---");
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Whitelist size: %d\n", NimBLEObserverOnly::whiteListSize());
        Serial.println("--------------------\n");
    }
    
    delay(1000);
}