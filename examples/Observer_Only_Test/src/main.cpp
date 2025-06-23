#include <Arduino.h>
#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

// Target device
const char* TARGET_DEVICE = "A4:C1:38:1D:87:BB";

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        // Show all devices briefly
        String addr = advertisedDevice->getAddress().toString().c_str();
        
        // Highlight our target device
        if (addr.equalsIgnoreCase(TARGET_DEVICE)) {
            Serial.println("\n=== TARGET DEVICE FOUND ===");
            Serial.printf("Address: %s\n", addr.c_str());
            Serial.printf("RSSI: %d dBm\n", advertisedDevice->getRSSI());
            
            if (advertisedDevice->haveName()) {
                Serial.printf("Name: %s\n", advertisedDevice->getName().c_str());
            }
            
            // Parse PVVX custom format if available
            if (advertisedDevice->haveServiceData()) {
                std::string svcData = advertisedDevice->getServiceData();
                Serial.printf("Service Data [%d]: ", svcData.length());
                for (size_t i = 0; i < svcData.length(); i++) {
                    Serial.printf("%02X ", (uint8_t)svcData[i]);
                }
                Serial.println();
                
                // PVVX format parsing (if service data matches expected format)
                if (svcData.length() >= 14) {
                    float temp = ((svcData[6] << 8) | svcData[7]) / 10.0;
                    uint8_t humidity = svcData[8];
                    uint16_t battery_mv = (svcData[9] << 8) | svcData[10];
                    uint8_t battery_pct = svcData[11];
                    uint8_t counter = svcData[13];
                    
                    Serial.printf("Temperature: %.1f°C\n", temp);
                    Serial.printf("Humidity: %d%%\n", humidity);
                    Serial.printf("Battery: %dmV (%d%%)\n", battery_mv, battery_pct);
                    Serial.printf("Counter: %d\n", counter);
                }
            }
            
            if (advertisedDevice->haveManufacturerData()) {
                std::string mfgData = advertisedDevice->getManufacturerData();
                Serial.printf("Manufacturer Data [%d]: ", mfgData.length());
                for (size_t i = 0; i < mfgData.length() && i < 20; i++) {
                    Serial.printf("%02X ", (uint8_t)mfgData[i]);
                }
                if (mfgData.length() > 20) Serial.print("...");
                Serial.println();
            }
            
            Serial.println("==========================\n");
        } else {
            // Show other devices in compact format
            Serial.printf("Device: %s, RSSI: %d", addr.c_str(), advertisedDevice->getRSSI());
            if (advertisedDevice->haveName()) {
                Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
            }
            Serial.println();
        }
    }
};

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000) { delay(10); }
    
    Serial.println("\n=== Observer-Only BLE Test ===");
    Serial.printf("Looking for ATC PVVX device: %s\n", TARGET_DEVICE);
    Serial.printf("Initial free heap: %d bytes\n\n", ESP.getFreeHeap());
    
    // Initialize observer-only mode with error checking
    Serial.println("Initializing NimBLE Observer-Only mode...");
    if (!NimBLEObserverOnly::init("Observer-Test")) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        Serial.println("Check:");
        Serial.println("- CONFIG_BT_ENABLED in menuconfig");
        Serial.println("- Sufficient free heap (>100KB)");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Initialized successfully");
    Serial.printf("Free heap after init: %d bytes\n", ESP.getFreeHeap());
    
    // Get scanner instance with error checking
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (!pScan) {
        Serial.println("ERROR: Failed to get scan object!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Got scan object");
    
    // Set callbacks
    pScan->setScanCallbacks(new MyScanCallbacks(), false);
    
    // Configure passive scanning
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setMaxResults(0); // Don't store, callback only
    
    // Start continuous scanning
    Serial.println("Starting scan...");
    if (!pScan->start(0, false)) {
        Serial.println("ERROR: Failed to start scan!");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Scanning started\n");
}

void loop() {
    static uint32_t lastStatus = 0;
    
    // Status report every 30 seconds
    if (millis() - lastStatus > 30000) {
        lastStatus = millis();
        Serial.println("\n--- Status Report ---");
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        
        NimBLEScan* pScan = NimBLEObserverOnly::getScan();
        if (pScan) {
            Serial.printf("Scanning: %s\n", pScan->isScanning() ? "Yes" : "No");
        }
        Serial.println("--------------------\n");
    }
    
    delay(1000);
}