/*
 * NimBLE Observer-Only Implementation
 */

#include "nimconfig.h"

#if defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY)

#include "NimBLEObserverOnly.h"
#include "NimBLELog.h"

#include "nimble/porting/nimble/include/nimble/nimble_port.h"
#include "nimble/porting/npl/freertos/include/nimble/nimble_port_freertos.h"
#include "nimble/nimble/host/include/host/ble_hs.h"
#include "nimble/nimble/host/include/host/ble_hs_pvcy.h"
#include "nimble/nimble/host/util/include/host/util/util.h"
#include "nimble/nimble/host/services/gap/include/services/gap/ble_svc_gap.h"

#include <esp_bt.h>
#include <esp_bt_main.h>
#include "nimble/esp_port/esp-hci/include/esp_nimble_hci.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static const char* LOG_TAG = "NimBLEObserver";

// Static member definitions
bool NimBLEObserverOnly::m_initialized = false;
NimBLEScan* NimBLEObserverOnly::m_pScan = nullptr;

/**
 * @brief Initialize NimBLE for observer-only mode
 */
bool NimBLEObserverOnly::init(const std::string& deviceName) {
    if (m_initialized) {
        NIMBLE_LOGW(LOG_TAG, "Already initialized");
        return true;
    }

    // Initialize ESP controller with minimal config
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
    // Optimize controller for scanning only
    bt_cfg.mode = ESP_BT_MODE_BLE;
    bt_cfg.normal_adv_size = 0;  // No advertising
    bt_cfg.scan_duplicate_type = 1;  // Hardware duplicate filter
    
    esp_err_t ret = esp_bt_controller_init(&bt_cfg);
    if (ret != ESP_OK) {
        NIMBLE_LOGE(LOG_TAG, "Controller init failed: %d", ret);
        return false;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret != ESP_OK) {
        NIMBLE_LOGE(LOG_TAG, "Controller enable failed: %d", ret);
        esp_bt_controller_deinit();
        return false;
    }

    // Initialize NimBLE host
    esp_nimble_hci_init();
    
    // Configure minimal host settings
    ble_hs_cfg.reset_cb = NimBLEObserverOnly::onReset;
    ble_hs_cfg.sync_cb = NimBLEObserverOnly::onSync;
    
    // Disable store callbacks - not needed for observer
    ble_hs_cfg.store_status_cb = nullptr;
    ble_hs_cfg.store_read_cb = nullptr;
    ble_hs_cfg.store_write_cb = nullptr;
    ble_hs_cfg.store_delete_cb = nullptr;
    
    // Minimal GAP service for observer mode
    ble_svc_gap_init();
    
    // Create host task with reduced stack size
    nimble_port_freertos_init(NimBLEObserverOnly::host_task);
    
    // Wait for sync
    int attempts = 0;
    while (!m_initialized && attempts < 20) {
        vTaskDelay(pdMS_TO_TICKS(100));
        attempts++;
    }
    
    if (!m_initialized) {
        NIMBLE_LOGE(LOG_TAG, "Failed to sync with controller");
        deinit();
        return false;
    }
    
    NIMBLE_LOGI(LOG_TAG, "Observer-only mode initialized");
    return true;
}

/**
 * @brief Deinitialize NimBLE
 */
bool NimBLEObserverOnly::deinit() {
    if (!m_initialized) {
        return true;
    }
    
    if (m_pScan != nullptr) {
        if (m_pScan->isScanning()) {
            m_pScan->stop();
        }
        delete m_pScan;
        m_pScan = nullptr;
    }
    
    // Stop NimBLE host
    nimble_port_stop();
    
    // Deinit host and controller  
    nimble_port_deinit();
    esp_nimble_hci_deinit();
    
    esp_bt_controller_disable();
    esp_bt_controller_deinit();
    
    m_initialized = false;
    
    NIMBLE_LOGI(LOG_TAG, "Observer deinitialized");
    return true;
}

/**
 * @brief Get the scan object
 */
NimBLEScan* NimBLEObserverOnly::getScan() {
    if (!m_initialized) {
        NIMBLE_LOGE(LOG_TAG, "Not initialized");
        return nullptr;
    }
    
    if (m_pScan == nullptr) {
        m_pScan = new NimBLEScan();
    }
    
    return m_pScan;
}

/**
 * @brief Check if initialized
 */
bool NimBLEObserverOnly::isInitialized() {
    return m_initialized;
}

/**
 * @brief Get device address
 */
NimBLEAddress NimBLEObserverOnly::getAddress() {
    ble_addr_t addr{};
    
    if (m_initialized) {
        int is_nrpa;
        int rc = ble_hs_id_copy_addr(BLE_OWN_ADDR_PUBLIC, addr.val, &is_nrpa);
        addr.type = BLE_OWN_ADDR_PUBLIC;
        if (rc != 0) {
            NIMBLE_LOGE(LOG_TAG, "Failed to get address: %d", rc);
        }
    }
    
    return NimBLEAddress(addr);
}

/**
 * @brief Set scan duplicate cache size
 */
void NimBLEObserverOnly::setScanDuplicateCacheSize(uint16_t size) {
#ifdef ESP_PLATFORM
    // In observer-only mode, these settings are handled at controller init
    // The controller config in init() already sets scan_duplicate_type
    NIMBLE_LOGW(LOG_TAG, "Scan duplicate cache size should be set before init()");
#endif
}

/**
 * @brief Set scan filter mode
 */
void NimBLEObserverOnly::setScanFilterMode(uint8_t mode) {
#ifdef ESP_PLATFORM
    // In observer-only mode, these settings are handled at controller init
    // The controller config in init() already sets scan_duplicate_type
    NIMBLE_LOGW(LOG_TAG, "Scan filter mode should be set before init()");
#endif
}

/**
 * @brief Minimal host task
 */
void NimBLEObserverOnly::host_task(void* param) {
    NIMBLE_LOGI(LOG_TAG, "Host task started");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

/**
 * @brief Sync callback
 */
void NimBLEObserverOnly::onSync() {
    // Set public address
    int rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        NIMBLE_LOGE(LOG_TAG, "Error setting address: %d", rc);
        return;
    }
    
    // Get and log our address
    NimBLEAddress addr = getAddress();
    NIMBLE_LOGI(LOG_TAG, "Device address: %s", addr.toString().c_str());
    
    m_initialized = true;
}

/**
 * @brief Reset callback
 */
void NimBLEObserverOnly::onReset(int reason) {
    NIMBLE_LOGE(LOG_TAG, "Host reset: %d", reason);
    m_initialized = false;
}

#endif // CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY