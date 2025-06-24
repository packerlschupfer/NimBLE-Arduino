/**
 * ATC Sensor Debug Test
 * 
 * This simple test looks for the specific ATC sensor to verify it's advertising
 */

#include <Arduino.h>
#include <NimBLEObserverOnly.h>
#include <cstring>

const char* TARGET_MAC = "a4:c1:38:1d:87:bb";  // Office sensor

class DebugCallbacks : public NimBLEScanCallbacks {
    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
        std::string addr = advertisedDevice->getAddress().toString();
        
        // Show all devices briefly
        Serial.printf("Found: %s RSSI=%d", addr.c_str(), advertisedDevice->getRSSI());
        
        // Check if it's our target
        if (strcasecmp(addr.c_str(), TARGET_MAC) == 0) {
            Serial.print(" <-- TARGET SENSOR!");
            
            // Check for service data (ATC sensors advertise with UUID 0x181a)
            if (advertisedDevice->haveServiceData()) {
                std::string svcData = advertisedDevice->getServiceData();
                if (svcData.length() >= 14) {  // ATC format
                    Serial.print(" [ATC Data Found]");
                }
            }
        }
        Serial.println();
    }
};

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=== ATC Sensor Debug Test ===");
    Serial.printf("Looking for: %s\n\n", TARGET_MAC);
    
    // Initialize observer mode
    if (!NimBLEObserverOnly::init("Debug-Test")) {
        Serial.println("Failed to init!");
        while(1) delay(1000);
    }
    
    // Get scanner
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    pScan->setScanCallbacks(new DebugCallbacks());
    pScan->setActiveScan(false);  // Passive
    pScan->setInterval(100);
    pScan->setWindow(99);
    
    Serial.println("Scanning for 30 seconds...\n");
    pScan->start(30, false);
    
    Serial.println("\n=== Scan Complete ===");
    Serial.println("If you didn't see your sensor:");
    Serial.println("1. Check battery");
    Serial.println("2. Verify MAC address"); 
    Serial.println("3. Try uppercase MAC");
    Serial.println("4. Move sensor closer");
}

void loop() {
    delay(1000);
}