#include <Arduino.h>
#include "NimBLEObserverOnly.h"
#include <cstring>

// Enable more debugging
#define NIMBLE_LOGE(tag, fmt, ...) Serial.printf("[E][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGW(tag, fmt, ...) Serial.printf("[W][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGI(tag, fmt, ...) Serial.printf("[I][%s] " fmt "\n", tag, ##__VA_ARGS__)
#define NIMBLE_LOGD(tag, fmt, ...) Serial.printf("[D][%s] " fmt "\n", tag, ##__VA_ARGS__)

// Test devices for whitelist (replace with your device MACs)
const char* TEST_DEVICES[] = {
    "a4:c1:38:1d:87:bb",  // ATC sensor (lowercase)
    "A4:C1:38:1D:87:BB",  // Same sensor (uppercase) 
    "a4:c1:38:07:51:82"   // Another ATC sensor
};
const int NUM_TEST_DEVICES = 3;

int openScanCount = 0;
int whitelistScanCount = 0;

class MyScanCallbacks : public NimBLEScanCallbacks {
public:
    bool useWhitelist = false;
    
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        if (useWhitelist) {
            whitelistScanCount++;
            Serial.printf("[WL %d] ", whitelistScanCount);
        } else {
            openScanCount++;
            Serial.printf("[Open %d] ", openScanCount);
        }
        
        Serial.printf("Device: %s, RSSI: %d", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
            
        // Check if it's one of our test devices
        std::string addr = advertisedDevice->getAddress().toString();
        for (int i = 0; i < NUM_TEST_DEVICES; i++) {
            if (strcasecmp(addr.c_str(), TEST_DEVICES[i]) == 0) {
                Serial.print(" <-- TEST DEVICE!");
                break;
            }
        }
        
        // Check for ATC service data
        if (advertisedDevice->haveServiceData()) {
            std::string svcData = advertisedDevice->getServiceData(NimBLEUUID((uint16_t)0x181a));
            if (svcData.length() >= 13) {
                Serial.print(" [ATC Data]");
            }
        }
        
        Serial.println();
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        Serial.printf("Scan ended: reason=%d, found=%d devices\n", reason, results.getCount());
    }
};

void testWhitelistAddresses() {
    Serial.println("\n=== Testing Whitelist Address Formats ===");
    
    // Clear whitelist
    NimBLEObserverOnly::whiteListClear();
    Serial.printf("Whitelist cleared. Count: %d\n", NimBLEObserverOnly::whiteListSize());
    
    // Test adding different address formats
    for (int i = 0; i < NUM_TEST_DEVICES; i++) {
        NimBLEAddress addr(TEST_DEVICES[i], BLE_ADDR_PUBLIC);
        bool added = NimBLEObserverOnly::whiteListAdd(addr);
        Serial.printf("Adding %s: %s\n", TEST_DEVICES[i], added ? "SUCCESS" : "FAILED");
    }
    
    Serial.printf("Final whitelist count: %d\n", NimBLEObserverOnly::whiteListSize());
    
    // Display whitelist contents
    Serial.println("Whitelist contents:");
    for (size_t i = 0; i < NimBLEObserverOnly::whiteListSize(); i++) {
        NimBLEAddress addr = NimBLEObserverOnly::getWhiteListAddress(i);
        Serial.printf("  [%d] %s\n", i, addr.toString().c_str());
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n\n=== Observer-Only BLE Test with Whitelist ===");
    
    // Show ESP32 info
    Serial.printf("ESP32 Model: %s\n", ESP.getChipModel());
    Serial.printf("ESP32 Revision: %d\n", ESP.getChipRevision());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Initialize observer-only mode
    Serial.println("\nInitializing NimBLE Observer...");
    if (!NimBLEObserverOnly::init("Observer-Test")) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        return;
    }
    Serial.println("NimBLE Observer initialized successfully");
    
    // Test whitelist functionality
    testWhitelistAddresses();
    
    // Get scanner instance
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (!pScan) {
        Serial.println("ERROR: Failed to get scan object!");
        return;
    }
    
    // Create callbacks
    MyScanCallbacks* pCallbacks = new MyScanCallbacks();
    pScan->setScanCallbacks(pCallbacks);
    
    // Configure passive scanning
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setDuplicateFilter(0);  // Report all devices
    
    // PHASE 1: Open scan (no whitelist)
    Serial.println("\n=== PHASE 1: Open Scan (10 seconds) ===");
    Serial.println("Should see ALL nearby BLE devices...\n");
    pCallbacks->useWhitelist = false;
    pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_NO_WL);
    
    if (!pScan->start(10, false)) {
        Serial.println("ERROR: Failed to start open scan!");
        return;
    }
    
    // Wait for scan to complete
    delay(11000);
    
    Serial.printf("\nOpen scan complete. Total devices: %d\n", openScanCount);
    
    // PHASE 2: Whitelist scan
    Serial.println("\n=== PHASE 2: Whitelist Scan (10 seconds) ===");
    Serial.println("Should ONLY see whitelisted devices...\n");
    pCallbacks->useWhitelist = true;
    pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);
    
    whitelistScanCount = 0;
    if (!pScan->start(10, false)) {
        Serial.println("ERROR: Failed to start whitelist scan!");
        return;
    }
    
    // Wait for scan to complete
    delay(11000);
    
    Serial.printf("\nWhitelist scan complete. Total devices: %d\n", whitelistScanCount);
    
    // Summary
    Serial.println("\n=== SUMMARY ===");
    Serial.printf("Open scan found: %d devices\n", openScanCount);
    Serial.printf("Whitelist scan found: %d devices\n", whitelistScanCount);
    Serial.printf("Whitelist efficiency: %.1f%% reduction\n", 
                  openScanCount > 0 ? (100.0 * (openScanCount - whitelistScanCount) / openScanCount) : 0);
    
    if (whitelistScanCount == 0 && NimBLEObserverOnly::whiteListSize() > 0) {
        Serial.println("\nWARNING: Whitelist configured but no devices found!");
        Serial.println("Possible issues:");
        Serial.println("1. MAC addresses don't match (try both upper/lowercase)");
        Serial.println("2. Devices not advertising");
        Serial.println("3. Address type mismatch (public vs random)");
    }
    
    // Continue with open scanning
    Serial.println("\nContinuing with open scan...");
    pCallbacks->useWhitelist = false;
    pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_NO_WL);
    pScan->start(0, false);
}

void loop() {
    delay(5000);
    
    // Print status every 5 seconds
    Serial.printf("Status: Free heap=%d, Uptime=%lu sec\n", 
                  ESP.getFreeHeap(), millis() / 1000);
}