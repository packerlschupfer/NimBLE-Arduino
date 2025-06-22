#include <Arduino.h>
#include <NimBLEDevice.h>

void setup() {
    Serial.begin(115200);
    
    // Test full NimBLE functionality
    NimBLEDevice::init("TestDevice");
    
    NimBLEScan* pScan = NimBLEDevice::getScan();
    if (pScan) {
        pScan->setActiveScan(false);
        pScan->start(5, false);
    }
}

void loop() {
    delay(1000);
}