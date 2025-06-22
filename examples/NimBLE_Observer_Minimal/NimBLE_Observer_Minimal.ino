/**
 * NimBLE Minimal Observer Example
 * 
 * This example demonstrates the absolute minimal configuration for BLE scanning
 * using the NimBLEObserverOnly interface. This approach provides maximum flash
 * savings by avoiding the standard NimBLEDevice initialization.
 * 
 * Flash savings: 200-400KB compared to full NimBLE
 * RAM savings: 20-50KB
 * 
 * Perfect for applications that only need to:
 * - Scan for BLE advertisements
 * - Read sensor data from advertising packets
 * - Detect presence of BLE devices
 */

// Use the observer-only configuration and interface
#include "NimBLEObserverOnly.h"

// Scan duration in seconds (0 = continuous)
static uint32_t scanTime = 0;

/**
 * Callback for detected devices
 */
class MyAdvertisedDeviceCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        // Example: Filter for specific device name or manufacturer
        if (advertisedDevice->haveName()) {
            std::string name = advertisedDevice->getName();
            
            // Example: Look for MiThermometer devices
            if (name.find("ATC_") != std::string::npos || 
                name.find("LYWSD") != std::string::npos ||
                name.find("MJ_HT_V1") != std::string::npos) {
                
                Serial.printf("Found sensor: %s, RSSI: %d\n", 
                    name.c_str(), 
                    advertisedDevice->getRSSI());
                
                // Parse service data if available
                if (advertisedDevice->haveServiceData()) {
                    std::string serviceData = advertisedDevice->getServiceData();
                    Serial.printf("  Service data length: %d\n", serviceData.length());
                    // Add your parsing logic here
                }
            }
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("NimBLE Minimal Observer Example");
    Serial.println("==============================");
    
    // Show initial memory state
    Serial.printf("Free heap at start: %d bytes\n", ESP.getFreeHeap());
    
    // Initialize NimBLE in observer-only mode
    if (!NimBLEObserverOnly::init("ObserverDevice")) {
        Serial.println("Failed to initialize NimBLE!");
        return;
    }
    
    Serial.printf("Free heap after init: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Memory used: %d bytes\n", 
        ESP.getHeapSize() - ESP.getFreeHeap());
    
    // Get the scan object
    NimBLEScan* pBLEScan = NimBLEObserverOnly::getScan();
    if (pBLEScan == nullptr) {
        Serial.println("Failed to create scan object!");
        return;
    }
    
    // Configure scanning
    pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(false);  // Passive scan only
    pBLEScan->setInterval(100);      // Scan interval (x0.625ms)
    pBLEScan->setWindow(99);         // Scan window (x0.625ms)
    pBLEScan->setMaxResults(0);      // Don't store results
    
    Serial.println("Starting continuous scan...\n");
}

void loop() {
    // Start scan (0 = continuous)
    NimBLEScan* pBLEScan = NimBLEObserverOnly::getScan();
    if (pBLEScan != nullptr && !pBLEScan->isScanning()) {
        pBLEScan->start(scanTime, nullptr, false);
    }
    
    // Periodic memory report
    static uint32_t lastReport = 0;
    if (millis() - lastReport > 30000) {  // Every 30 seconds
        Serial.printf("\nMemory report - Free heap: %d bytes, Used: %d bytes\n", 
            ESP.getFreeHeap(),
            ESP.getHeapSize() - ESP.getFreeHeap());
        lastReport = millis();
    }
    
    delay(1000);
}