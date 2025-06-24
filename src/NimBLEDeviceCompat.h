/*
 * NimBLE Device Compatibility Layer for Observer-Only Mode
 * 
 * This header provides compatibility shims for libraries that expect
 * NimBLEDevice to be available even in observer-only mode.
 */

#ifndef NIMBLE_DEVICE_COMPAT_H
#define NIMBLE_DEVICE_COMPAT_H

#include "nimconfig.h"

#if defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)

// In observer-only mode, redirect to NimBLEObserverOnly
#include "NimBLEObserverOnly.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"
#include "NimBLEAddress.h"

// Provide a minimal NimBLEDevice class that forwards to NimBLEObserverOnly
class NimBLEDevice {
public:
    // Whitelist compatibility methods
    static bool whiteListAdd(const NimBLEAddress& address) {
        return NimBLEObserverOnly::whiteListAdd(address);
    }
    
    static bool whiteListRemove(const NimBLEAddress& address) {
        return NimBLEObserverOnly::whiteListRemove(address);
    }
    
    static bool whiteListClear() {
        return NimBLEObserverOnly::whiteListClear();
    }
    
    static size_t getWhiteListCount() {
        return NimBLEObserverOnly::whiteListSize();
    }
    
    static NimBLEAddress getWhiteListAddress(size_t index) {
        return NimBLEObserverOnly::getWhiteListAddress(index);
    }
    
    // Other methods that might be needed
    static NimBLEScan* getScan() {
        return NimBLEObserverOnly::getScan();
    }
    
    static bool init(const std::string& deviceName = "") {
        return NimBLEObserverOnly::init(deviceName);
    }
    
    static bool deinit() {
        return NimBLEObserverOnly::deinit();
    }
    
    static bool isInitialized() {
        return NimBLEObserverOnly::isInitialized();
    }
    
    static NimBLEAddress getAddress() {
        return NimBLEObserverOnly::getAddress();
    }
};

#else
// In normal mode, include the real NimBLEDevice
#include "NimBLEDevice.h"

#endif // CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY

#endif // NIMBLE_DEVICE_COMPAT_H