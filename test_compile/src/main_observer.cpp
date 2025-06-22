#include <Arduino.h>
#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"

void setup() {
    Serial.begin(115200);
    
    if (!NimBLEObserverOnly::init("TestDevice")) {
        Serial.println("Failed to initialize");
        return;
    }
    
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (pScan) {
        pScan->setActiveScan(false);
        pScan->start(0);
    }
}

void loop() {
    delay(1000);
}