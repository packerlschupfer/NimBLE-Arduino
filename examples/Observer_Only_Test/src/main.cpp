#include <Arduino.h>
#include "NimBLEObserverOnly.h"

class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        Serial.printf("Device: %s, RSSI: %d\n", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Observer-Only BLE Test");
    
    // Initialize observer-only mode
    NimBLEObserverOnly::init("Observer-Test");
    
    // Get scanner instance
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    
    // Set callbacks
    pScan->setScanCallbacks(new MyScanCallbacks());
    
    // Configure passive scanning
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(99);
    
    // Start continuous scanning
    Serial.println("Starting scan...");
    pScan->start(0, false);
}

void loop() {
    delay(1000);
    
    // Print free heap every second
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
}