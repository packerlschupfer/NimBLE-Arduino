/*
 * NimBLE Observer-Only Interface
 * 
 * Lightweight alternative to NimBLEDevice for applications that only need
 * passive BLE scanning. This bypasses the standard initialization chain
 * that pulls in all BLE subsystems.
 */

#ifndef NIMBLE_OBSERVER_ONLY_H
#define NIMBLE_OBSERVER_ONLY_H

#include "nimconfig_observer_only.h"
#include "NimBLEAddress.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"
#include "NimBLEUtils.h"

#include <functional>

class NimBLEObserverOnly {
public:
    /**
     * @brief Initialize NimBLE for observer-only mode
     * @param [in] deviceName The device name (unused in observer mode)
     * @return true on success
     */
    static bool init(const std::string& deviceName = "");
    
    /**
     * @brief Deinitialize NimBLE
     * @return true on success
     */
    static bool deinit();
    
    /**
     * @brief Get the scan object
     * @return Pointer to the NimBLEScan object or nullptr
     */
    static NimBLEScan* getScan();
    
    /**
     * @brief Check if NimBLE is initialized
     * @return true if initialized
     */
    static bool isInitialized();
    
    /**
     * @brief Get device address
     * @return The device's BLE address
     */
    static NimBLEAddress getAddress();
    
    /**
     * @brief Set scan duplicate cache size
     * @param [in] size The cache size
     */
    static void setScanDuplicateCacheSize(uint16_t size);
    
    /**
     * @brief Set scan filter mode
     * @param [in] mode The filter mode
     */
    static void setScanFilterMode(uint8_t mode);

private:
    static bool m_initialized;
    static NimBLEScan* m_pScan;
    
    // Minimal host task for observer mode
    static void host_task(void* param);
    
    // Simplified sync callback
    static void onSync();
    
    // Simplified reset callback  
    static void onReset(int reason);
};

#endif // NIMBLE_OBSERVER_ONLY_H