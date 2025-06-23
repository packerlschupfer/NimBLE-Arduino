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
        Serial.printf("Advertised Device: %s ", advertisedDevice->getAddress().toString().c_str());
        Serial.printf("RSSI: %d ", advertisedDevice->getRSSI());
        
        if (advertisedDevice->haveName()) {
            Serial.printf("Name: %s ", advertisedDevice->getName().c_str());
        }
        
        if (advertisedDevice->haveManufacturerData()) {
            std::string data = advertisedDevice->getManufacturerData();
            Serial.printf("MfgData[%d]: ", data.length());
            for (size_t i = 0; i < data.length(); i++) {
                Serial.printf("%02X ", (uint8_t)data[i]);
            }
        }
        
        Serial.println();
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting NimBLE Observer-Only Test");
    
    // Initialize observer-only mode
    NimBLEObserverOnly::init("ESP32_Observer");
    
    // Get the scan object
    NimBLEScan* pBLEScan = NimBLEObserverOnly::getScan();
    
    // Set callbacks
    pBLEScan->setScanCallbacks(new MyAdvertisedDeviceCallbacks(), false);
    
    // Configure scan parameters
    pBLEScan->setActiveScan(false); // Passive scan for lower power
    pBLEScan->setInterval(100);      // 100ms interval
    pBLEScan->setWindow(50);         // 50ms window
    
    // Start scanning forever
    pBLEScan->start(0);
    
    Serial.println("Scanning...");
}

void loop() {
    delay(1000);
    
    // Print memory usage periodically
    static uint32_t counter = 0;
    if (++counter % 10 == 0) {
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}