#include <Arduino.h>
#include "NimBLEObserverOnly.h"

// Enable more debugging
#define NIMBLE_LOGE(tag, fmt, ...) Serial.printf("[E][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGW(tag, fmt, ...) Serial.printf("[W][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGI(tag, fmt, ...) Serial.printf("[I][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGD(tag, fmt, ...) Serial.printf("[D][%s] " fmt "\n", tag, ##__VA_ARGS__)

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.printf("Device: %s, RSSI: %d\n", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        Serial.printf("Scan ended: reason=%d, count=%d\n", reason, results.getCount());
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== Starting Simple Observer-Only BLE Test ===");
    
    // Show ESP32 info
    Serial.printf("ESP32 Model: %s\n", ESP.getChipModel());
    Serial.printf("ESP32 Revision: %d\n", ESP.getChipRevision());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Initialize observer-only mode
    Serial.println("Initializing NimBLE Observer...");
    if (!NimBLEObserverOnly::init("Observer-Test")) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        return;
    }
    Serial.println("NimBLE Observer initialized successfully");
    
    // Get scanner instance
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (!pScan) {
        Serial.println("ERROR: Failed to get scan object!");
        return;
    }
    Serial.println("Got scan object");
    
    // Set callbacks
    pScan->setScanCallbacks(new MyScanCallbacks());
    
    // Configure passive scanning
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setDuplicateFilter(0);  // Report all devices
    
    // Start scanning for 10 seconds
    Serial.println("Starting 10 second scan...");
    if (!pScan->start(10, false)) {
        Serial.println("ERROR: Failed to start scan!");
        return;
    }
    
    // Wait for scan to complete
    delay(11000);
    
    // Get results
    NimBLEScanResults results = pScan->getResults();
    Serial.printf("\nFinal results: %d devices found\n", results.getCount());
    
    // Start continuous scanning
    Serial.println("\nStarting continuous scan...");
    pScan->start(0, false);
}

void loop() {
    delay(5000);
    
    // Print status every 5 seconds
    Serial.printf("Status: Free heap=%d, Uptime=%lu sec\n", 
                  ESP.getFreeHeap(), millis() / 1000);
}