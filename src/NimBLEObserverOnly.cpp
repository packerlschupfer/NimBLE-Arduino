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
#include "nimble/nimble/host/include/host/ble_gap.h"

#include <esp_bt.h>
#include <esp_bt_main.h>
#include "nimble/esp_port/esp-hci/include/esp_nimble_hci.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>

#if defined(CONFIG_ENABLE_ARDUINO_DEPENDS) && defined(CONFIG_BT_ENABLED)
extern "C" bool btStarted();
#endif

extern "C" void ble_store_config_init(void);

// Provide stub implementations for store config symbols that are undefined in observer-only builds
extern "C" {
    // These are referenced by ble_store_config.c but not needed for observer-only mode
    void* ble_store_config_csfcs = nullptr;
    int ble_store_config_persist_csfcs(void) { return 0; }
}

static const char* LOG_TAG = "NimBLEObserver";

// Static member definitions
bool NimBLEObserverOnly::m_initialized = false;
NimBLEScan* NimBLEObserverOnly::m_pScan = nullptr;
std::vector<NimBLEAddress> NimBLEObserverOnly::m_whiteList;

/**
 * @brief Initialize NimBLE for observer-only mode
 */
bool NimBLEObserverOnly::init(const std::string& deviceName) {
    NIMBLE_LOGI(LOG_TAG, "init() called with device name: %s", deviceName.c_str());
    
    if (m_initialized) {
        NIMBLE_LOGW(LOG_TAG, "Already initialized");
        // Ensure scan object exists even if already initialized
        if (m_pScan == nullptr) {
            m_pScan = new NimBLEScan();
        }
        return true;
    }

    NIMBLE_LOGI(LOG_TAG, "Starting NimBLE initialization...");
    
#if defined(CONFIG_ENABLE_ARDUINO_DEPENDS) && defined(CONFIG_BT_ENABLED)
    // Make sure Arduino doesn't release BLE memory
    // Note: btStarted() only checks if controller is enabled, doesn't actually start it
    bool bt_started = btStarted();
    NIMBLE_LOGI(LOG_TAG, "btStarted() returned: %d", bt_started);
    
    // Note: We only need to ensure BT memory isn't released by Arduino init
#endif
    
    // Initialize NVS flash
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ret = nvs_flash_erase();
        if (ret == ESP_OK) {
            ret = nvs_flash_init();
        }
    }
    if (ret != ESP_OK) {
        NIMBLE_LOGE(LOG_TAG, "nvs_flash_init() failed: %d", ret);
        return false;
    }
    
    // Check controller status first
    esp_bt_controller_status_t status = esp_bt_controller_get_status();
    NIMBLE_LOGI(LOG_TAG, "Initial controller status: %d", status);
    
    // If controller is already initialized or enabled, we need to handle it
    if (status == ESP_BT_CONTROLLER_STATUS_ENABLED) {
        NIMBLE_LOGI(LOG_TAG, "Controller already enabled, disabling first...");
        ret = esp_bt_controller_disable();
        if (ret != ESP_OK) {
            NIMBLE_LOGE(LOG_TAG, "Failed to disable controller: %d", ret);
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Give time for controller to fully disable
        status = esp_bt_controller_get_status();
        NIMBLE_LOGI(LOG_TAG, "Controller status after disable: %d", status);
    }
    
    if (status == ESP_BT_CONTROLLER_STATUS_INITED) {
        NIMBLE_LOGI(LOG_TAG, "Controller already initialized, deinitializing first...");
        ret = esp_bt_controller_deinit();
        if (ret != ESP_OK) {
            NIMBLE_LOGE(LOG_TAG, "Failed to deinit controller: %d", ret);
            return false;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Give time for controller to fully deinit
        status = esp_bt_controller_get_status();
        NIMBLE_LOGI(LOG_TAG, "Controller status after deinit: %d", status);
    }
    
    // Release Classic BT memory
    ret = esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT);
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        NIMBLE_LOGW(LOG_TAG, "Failed to release BT memory: %d", ret);
    }
    
    // Initialize controller using default configuration
#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0) || !defined(CONFIG_NIMBLE_CPP_IDF)
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    
#if defined(CONFIG_IDF_TARGET_ESP32)
    // For ESP32, we need to set max connections
    // Even for observer-only mode, the controller requires at least 1 connection
    bt_cfg.ble_max_conn = CONFIG_BT_NIMBLE_MAX_CONNECTIONS > 0 ? CONFIG_BT_NIMBLE_MAX_CONNECTIONS : 1;
#elif defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
    bt_cfg.ble_max_act = CONFIG_BT_NIMBLE_MAX_CONNECTIONS > 0 ? CONFIG_BT_NIMBLE_MAX_CONNECTIONS : 1;
#else
    bt_cfg.nimble_max_connections = CONFIG_BT_NIMBLE_MAX_CONNECTIONS > 0 ? CONFIG_BT_NIMBLE_MAX_CONNECTIONS : 1;
#endif

#if CONFIG_BTDM_BLE_SCAN_DUPL
    // Configure scan duplicate filtering for ESP32
    bt_cfg.normal_adv_size = CONFIG_BTDM_SCAN_DUPL_CACHE_SIZE;
    bt_cfg.scan_duplicate_type = CONFIG_BTDM_SCAN_DUPL_TYPE;
#elif CONFIG_BT_LE_SCAN_DUPL
    // Configure scan duplicate filtering for newer chips
    bt_cfg.ble_ll_rsp_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT;
    bt_cfg.ble_ll_adv_dup_list_count = CONFIG_BT_LE_LL_DUP_SCAN_LIST_COUNT;
#endif
    
    NIMBLE_LOGI(LOG_TAG, "Calling esp_bt_controller_init with BLE mode config...");
    NIMBLE_LOGI(LOG_TAG, "BT controller config: mode=%d (BLE=1, CLASSIC=2, DUAL=3), ble_max_conn=%d", 
                bt_cfg.mode, bt_cfg.ble_max_conn);
    NIMBLE_LOGI(LOG_TAG, "Controller stack size=%d, prio=%d", 
                bt_cfg.controller_task_stack_size, bt_cfg.controller_task_prio);
    
    // Check if controller was already initialized by Arduino
    status = esp_bt_controller_get_status();
    if (status == ESP_BT_CONTROLLER_STATUS_INITED) {
        NIMBLE_LOGW(LOG_TAG, "Controller already initialized, skipping init");
    } else {
        ret = esp_bt_controller_init(&bt_cfg);
        if (ret != ESP_OK) {
            NIMBLE_LOGE(LOG_TAG, "Controller init failed: %d (0x%x)", ret, ret);
            NIMBLE_LOGE(LOG_TAG, "ESP_ERR_INVALID_ARG=%d, ESP_ERR_INVALID_STATE=%d, ESP_ERR_NO_MEM=%d", 
                        ESP_ERR_INVALID_ARG, ESP_ERR_INVALID_STATE, ESP_ERR_NO_MEM);
            return false;
        }
    }
    
    status = esp_bt_controller_get_status();
    NIMBLE_LOGI(LOG_TAG, "Controller status after init: %d", status);
    
    // Enable with the same mode that was configured during init
    esp_bt_mode_t enable_mode = static_cast<esp_bt_mode_t>(bt_cfg.mode);
    NIMBLE_LOGI(LOG_TAG, "Enabling controller with mode: %d", enable_mode);
    ret = esp_bt_controller_enable(enable_mode);
    if (ret != ESP_OK) {
        NIMBLE_LOGE(LOG_TAG, "Controller enable failed: %d", ret);
        esp_bt_controller_deinit();
        return false;
    }
    
    // Initialize NimBLE HCI for legacy VHCI
#if CONFIG_BT_NIMBLE_LEGACY_VHCI_ENABLE
    ret = esp_nimble_hci_init();
    if (ret != ESP_OK) {
        NIMBLE_LOGE(LOG_TAG, "esp_nimble_hci_init failed: %d", ret);
        esp_bt_controller_disable();
        esp_bt_controller_deinit();
        return false;
    }
#endif
#endif // ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5, 0, 0)

    // Initialize NimBLE port - CRITICAL: This must be called before any host configuration
    nimble_port_init();
    
    // Configure minimal host settings
    ble_hs_cfg.reset_cb = NimBLEObserverOnly::onReset;
    ble_hs_cfg.sync_cb = NimBLEObserverOnly::onSync;
    
    // Disable security features not needed for observer
    ble_hs_cfg.sm_bonding = 0;
    ble_hs_cfg.sm_mitm = 0;
    ble_hs_cfg.sm_sc = 0;
    ble_hs_cfg.sm_our_key_dist = 0;
    ble_hs_cfg.sm_their_key_dist = 0;
    
    // Set device name if provided
    if (!deviceName.empty()) {
        ble_svc_gap_device_name_set(deviceName.c_str());
    }
    
    // Initialize store config (even if we don't use it, it's required)
    ble_store_config_init();
    
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
    
    // Create the scan object now that we're initialized
    if (m_pScan == nullptr) {
        m_pScan = new NimBLEScan();
        if (m_pScan == nullptr) {
            NIMBLE_LOGE(LOG_TAG, "Failed to create scan object");
            deinit();
            return false;
        }
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
        NIMBLE_LOGE(LOG_TAG, "Not initialized - call NimBLEObserverOnly::init() first");
        return nullptr;
    }
    
    if (m_pScan == nullptr) {
        NIMBLE_LOGW(LOG_TAG, "Scan object was null, creating new instance");
        m_pScan = new NimBLEScan();
        if (m_pScan == nullptr) {
            NIMBLE_LOGE(LOG_TAG, "Failed to create scan object - out of memory?");
        }
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
    NIMBLE_LOGI(LOG_TAG, "NimBLE host synced");
    
    // Ensure we have public and random addresses
    int rc = ble_hs_util_ensure_addr(0);
    if (rc == 0) {
        rc = ble_hs_util_ensure_addr(1);
    }
    
    if (rc != 0) {
        NIMBLE_LOGE(LOG_TAG, "Error ensuring address: %d", rc);
        return;
    }
    
    // Initialize GAP service after sync
    ble_svc_gap_init();
    
    // Use public address if available
    rc = ble_hs_id_copy_addr(BLE_OWN_ADDR_PUBLIC, NULL, NULL);
    if (rc != 0) {
        NIMBLE_LOGW(LOG_TAG, "No public address available, will use random");
    }
    
    // Small delay for housekeeping
    ble_npl_time_delay(1);
    
    m_initialized = true;
    
    // Get and log our address
    NimBLEAddress addr = getAddress();
    NIMBLE_LOGI(LOG_TAG, "Device address: %s", addr.toString().c_str());
}

/**
 * @brief Reset callback
 */
void NimBLEObserverOnly::onReset(int reason) {
    NIMBLE_LOGE(LOG_TAG, "Host reset: %d", reason);
    m_initialized = false;
}

/**
 * @brief Add a device to the whitelist
 * @param [in] address The address to add
 * @return true if successful
 */
bool NimBLEObserverOnly::whiteListAdd(const NimBLEAddress& address) {
    if (!m_initialized) {
        NIMBLE_LOGE(LOG_TAG, "Not initialized");
        return false;
    }
    
    if (!onWhiteList(address)) {
        m_whiteList.push_back(address);
        int rc = ble_gap_wl_set(reinterpret_cast<ble_addr_t*>(&m_whiteList[0]), m_whiteList.size());
        if (rc != 0) {
            NIMBLE_LOGE(LOG_TAG, "Failed adding to whitelist rc=%d", rc);
            m_whiteList.pop_back();
            return false;
        }
    }
    
    return true;
}

/**
 * @brief Remove a device from the whitelist
 * @param [in] address The address to remove
 * @return true if successful
 */
bool NimBLEObserverOnly::whiteListRemove(const NimBLEAddress& address) {
    if (!m_initialized) {
        NIMBLE_LOGE(LOG_TAG, "Not initialized");
        return false;
    }
    
    for (auto it = m_whiteList.begin(); it < m_whiteList.end(); ++it) {
        if (*it == address) {
            m_whiteList.erase(it);
            int rc = ble_gap_wl_set(reinterpret_cast<ble_addr_t*>(&m_whiteList[0]), m_whiteList.size());
            if (rc != 0) {
                m_whiteList.push_back(address);
                NIMBLE_LOGE(LOG_TAG, "Failed removing from whitelist rc=%d", rc);
                return false;
            }
            
            // Shrink vector capacity
            std::vector<NimBLEAddress>(m_whiteList).swap(m_whiteList);
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Clear the whitelist
 * @return true if successful
 */
bool NimBLEObserverOnly::whiteListClear() {
    if (!m_initialized) {
        NIMBLE_LOGE(LOG_TAG, "Not initialized");
        return false;
    }
    
    m_whiteList.clear();
    int rc = ble_gap_wl_set(nullptr, 0);
    if (rc != 0) {
        NIMBLE_LOGE(LOG_TAG, "Failed clearing whitelist rc=%d", rc);
        return false;
    }
    
    // Free vector memory
    std::vector<NimBLEAddress>().swap(m_whiteList);
    return true;
}

/**
 * @brief Check if a device is on the whitelist
 * @param [in] address The address to check
 * @return true if the address is on the whitelist
 */
bool NimBLEObserverOnly::onWhiteList(const NimBLEAddress& address) {
    for (const auto& addr : m_whiteList) {
        if (addr == address) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Get the whitelist size
 * @return The number of addresses in the whitelist
 */
size_t NimBLEObserverOnly::whiteListSize() {
    if (!m_initialized) {
        return 0;
    }
    return m_whiteList.size();
}

/**
 * @brief Get an address from the whitelist by index
 * @param [in] index The index of the address to get
 * @return The address at the specified index, or an invalid address if out of bounds
 */
NimBLEAddress NimBLEObserverOnly::getWhiteListAddress(size_t index) {
    if (!m_initialized || index >= m_whiteList.size()) {
        // Return an invalid address if not initialized or index is out of bounds
        return NimBLEAddress();
    }
    return m_whiteList[index];
}

#endif // CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY