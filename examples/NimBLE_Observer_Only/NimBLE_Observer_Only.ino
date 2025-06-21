/**
 * NimBLE Observer-Only Example
 * 
 * This example demonstrates how to configure NimBLE for observer-only mode,
 * which provides maximum memory savings by disabling all features except
 * passive BLE scanning.
 * 
 * Memory savings with this configuration:
 * - ~40KB+ flash memory saved compared to full NimBLE build
 * - Minimal RAM usage for scanning operations
 * 
 * Author: H2zero
 * Date: 2025
 */

// IMPORTANT: These defines must come BEFORE including NimBLE headers
// They configure NimBLE for observer-only mode with minimal memory footprint

// Disable all roles except OBSERVER
#define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED
#define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED  
#define CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED

// Disable L2CAP Connection Oriented Channels
#define CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM 0

// Disable all security/encryption
#define MYNEWT_VAL_BLE_SM_LVL 0
#define CONFIG_BT_NIMBLE_SM_LEGACY 0
#define CONFIG_BT_NIMBLE_SM_SC 0
#define MYNEWT_VAL_BLE_SM_BONDING 0
#define MYNEWT_VAL_BLE_LL_CFG_FEAT_LE_ENCRYPTION 0

// Disable connection-related features
#define CONFIG_BT_NIMBLE_MAX_CONNECTIONS 0
#define CONFIG_BT_NIMBLE_MAX_BONDS 0
#define CONFIG_BT_NIMBLE_MAX_CCCDS 0
#define CONFIG_BT_NIMBLE_NVS_PERSIST 0

// Completely disable the BLE store (saves additional memory)
#define CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED 1

// Disable privacy and address resolution features (~6-9KB)
#define CONFIG_BT_NIMBLE_HOST_PRIVACY_DISABLED 1

// Disable GATT procedures
#define CONFIG_BT_NIMBLE_GATT_MAX_PROCS 0
#define CONFIG_BT_NIMBLE_GATT_CACHING 0

// Disable extended advertising if not needed (saves more memory)
// Uncomment these lines if you don't need BLE 5.0 extended advertising
// #define CONFIG_BT_NIMBLE_EXT_ADV 0
// #define CONFIG_BT_NIMBLE_ENABLE_PERIODIC_ADV 0

// Reduce memory buffers
#define CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT 6
#define CONFIG_BT_NIMBLE_TRANSPORT_ACL_FROM_LL_COUNT 0
#define CONFIG_BT_NIMBLE_TRANSPORT_EVT_COUNT 8

// Disable all ATT server operations
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_INFO 0
#define MYNEWT_VAL_BLE_ATT_SVR_FIND_TYPE 0
#define MYNEWT_VAL_BLE_ATT_SVR_INDICATE 0
#define MYNEWT_VAL_BLE_ATT_SVR_NOTIFY 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_BLOB 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_GROUP 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_MULT 0
#define MYNEWT_VAL_BLE_ATT_SVR_READ_TYPE 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE_CMD 0
#define MYNEWT_VAL_BLE_ATT_SVR_WRITE_NO_RSP 0
#define MYNEWT_VAL_BLE_ATT_SVR_SIGNED_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_PREP_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_EXEC_WRITE 0
#define MYNEWT_VAL_BLE_ATT_SVR_NOTIFY_MULT 0
#define MYNEWT_VAL_BLE_ATT_SVR_QUEUED_WRITE 0

// Now include NimBLE
#include <NimBLEDevice.h>

// Scan parameters
static uint32_t scanTime = 0; // Continuous scanning
static NimBLEScan* pBLEScan;

// Statistics
static uint32_t devicesFound = 0;
static uint32_t lastReportTime = 0;

/**
 * Callback for each detected device during scan
 */
class AdvertisedDeviceCallbacks: public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        devicesFound++;
        
        // Print basic device info
        Serial.printf("Device found: %s", advertisedDevice->getAddress().toString().c_str());
        
        // Show RSSI
        Serial.printf(", RSSI: %d", advertisedDevice->getRSSI());
        
        // Show name if available
        if (advertisedDevice->haveName()) {
            Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
        }
        
        // Show manufacturer data if available
        if (advertisedDevice->haveManufacturerData()) {
            std::string manufData = advertisedDevice->getManufacturerData();
            if (manufData.length() >= 2) {
                uint16_t manufId = (manufData[1] << 8) | manufData[0];
                Serial.printf(", Manuf ID: 0x%04X", manufId);
            }
        }
        
        Serial.println();
    }
    
    void onScanEnd(NimBLEScanResults results) {
        Serial.println("Scan ended, restarting...");
        
        // Print statistics
        uint32_t now = millis();
        if (now - lastReportTime > 10000) { // Report every 10 seconds
            Serial.printf("Statistics: %lu devices found\n", devicesFound);
            Serial.printf("Free heap: %lu bytes\n", (unsigned long)ESP.getFreeHeap());
            lastReportTime = now;
        }
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("NimBLE Observer-Only Example");
    Serial.println("============================");
    
    // Print memory info at startup
    Serial.printf("Initial free heap: %lu bytes\n", (unsigned long)ESP.getFreeHeap());
    Serial.printf("Initial heap size: %lu bytes\n", (unsigned long)ESP.getHeapSize());
    
    // Initialize NimBLE
    NimBLEDevice::init("");
    
    // Print memory after initialization
    Serial.printf("After init free heap: %lu bytes\n", (unsigned long)ESP.getFreeHeap());
    
    // Get the scan object
    pBLEScan = NimBLEDevice::getScan();
    
    // Set the callback for when devices are discovered
    pBLEScan->setScanCallbacks(new AdvertisedDeviceCallbacks(), false);
    
    // Set scan parameters
    pBLEScan->setActiveScan(false); // Passive scan only in observer mode
    pBLEScan->setInterval(100);     // How often to scan (in 0.625ms units)
    pBLEScan->setWindow(99);        // How long to scan during interval
    pBLEScan->setMaxResults(0);     // Don't store results, use callback only
    
    Serial.println("Setup complete. Starting continuous scan...");
    Serial.println();
}

void loop() {
    // Start scanning (will run for scanTime seconds, 0 = forever)
    pBLEScan->start(scanTime, nullptr, false);
    
    // Small delay to prevent watchdog issues
    delay(1000);
}

/**
 * Example output:
 * 
 * NimBLE Observer-Only Example
 * ============================
 * Initial free heap: 280124 bytes
 * Initial heap size: 380124 bytes
 * After init free heap: 265432 bytes
 * Setup complete. Starting continuous scan...
 * 
 * Device found: 11:22:33:44:55:66, RSSI: -75, Name: MyDevice
 * Device found: AA:BB:CC:DD:EE:FF, RSSI: -82, Manuf ID: 0x004C
 * Device found: 12:34:56:78:90:AB, RSSI: -68
 * Statistics: 127 devices found
 * Free heap: 264892 bytes
 */