/**
 * ATC Sensor Debug Test with Whitelist
 * 
 * Uses hardware whitelist to filter for specific ATC sensor only
 */

#include <Arduino.h>
#include <NimBLEObserverOnly.h>
#include <cstring>

const char* TARGET_MAC = "a4:c1:38:1d:87:bb";  // Office sensor
int deviceCount = 0;
int targetCount = 0;

class DebugCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        deviceCount++;
        std::string addr = advertisedDevice->getAddress().toString();
        
        Serial.printf("[%d] Found: %s RSSI=%d", deviceCount, addr.c_str(), advertisedDevice->getRSSI());
        
        // Should only see our target with whitelist
        if (strcasecmp(addr.c_str(), TARGET_MAC) == 0) {
            targetCount++;
            Serial.print(" <-- TARGET SENSOR!");
            
            // Check for service data (ATC sensors advertise with UUID 0x181a)
            if (advertisedDevice->haveServiceData()) {
                std::string svcData = advertisedDevice->getServiceData(BLEUUID((uint16_t)0x181a));
                if (svcData.length() >= 13) {  // ATC PVVX format
                    // Parse temperature and humidity
                    int16_t temp = (svcData[6] | (svcData[7] << 8));
                    uint16_t humi = (svcData[8] | (svcData[9] << 8));
                    uint8_t batt = svcData[10];
                    
                    Serial.printf(" [Temp=%.2f°C Humi=%.2f%% Batt=%d%%]", 
                                  temp/100.0, humi/100.0, batt);
                }
            }
        }
        Serial.println();
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        Serial.printf("\nScan ended: reason=%d, total devices=%d\n", reason, deviceCount);
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== ATC Sensor Debug Test with Whitelist ===");
    Serial.printf("Target sensor: %s\n\n", TARGET_MAC);
    
    // Initialize observer mode
    Serial.println("Initializing NimBLE Observer-Only mode...");
    if (!NimBLEObserverOnly::init("Debug-Test")) {
        Serial.println("Failed to init!");
        while(1) delay(1000);
    }
    Serial.println("NimBLE initialized successfully");
    
    // Configure whitelist
    Serial.println("\nConfiguring hardware whitelist...");
    NimBLEObserverOnly::whiteListClear();
    NimBLEAddress targetAddr(TARGET_MAC);
    
    if (NimBLEObserverOnly::whiteListAdd(targetAddr)) {
        Serial.printf("Added %s to whitelist\n", TARGET_MAC);
    } else {
        Serial.printf("Failed to add %s to whitelist!\n", TARGET_MAC);
    }
    
    Serial.printf("Whitelist now contains %d entries\n", NimBLEObserverOnly::whiteListSize());
    
    // Get scanner
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    pScan->setScanCallbacks(new DebugCallbacks());
    pScan->setActiveScan(false);  // Passive scan
    pScan->setInterval(100);
    pScan->setWindow(99);
    pScan->setFilterPolicy(BLE_HCI_SCAN_FILT_USE_WL);  // Use whitelist
    
    Serial.println("\nStarting 30 second whitelist-filtered scan...");
    Serial.println("(Should ONLY see the target device)\n");
    pScan->start(30, false);
    
    Serial.println("\n=== Scan Complete ===");
    Serial.printf("Target sensor seen %d times\n", targetCount);
    
    if (targetCount == 0) {
        Serial.println("\nTroubleshooting:");
        Serial.println("1. Verify MAC address is correct");
        Serial.println("2. Try uppercase: A4:C1:38:1D:87:BB");
        Serial.println("3. Check sensor battery");
        Serial.println("4. Move sensor closer");
        Serial.println("5. Verify sensor has ATC firmware");
    }
}

void loop() {
    delay(1000);
}