// Test compilation for observer-only mode
// Compile with: g++ -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1 -DCONFIG_BT_ENABLED=1 -DCONFIG_BT_NIMBLE_ROLE_OBSERVER=1 -I./src -c test_observer_only_compilation.cpp

#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

class MyAdvertisedDeviceCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        // Process result
    }
};

int main() {
    // Test observer-only compilation
    NimBLEObserverOnly::init("TestDevice");
    
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (pScan) {
        MyAdvertisedDeviceCallbacks* callbacks = new MyAdvertisedDeviceCallbacks();
        pScan->setScanCallbacks(callbacks);
        pScan->setActiveScan(false);
        pScan->start(0); // Scan forever
    }
    
    return 0;
}