/**
 * Observer-Only Mode Test for ATC_MiThermometer
 * 
 * This example shows how to properly use observer-only mode
 * for scanning ATC MiThermometer devices (Xiaomi sensors with custom firmware).
 * 
 * Demonstrates:
 * - Proper initialization with error handling
 * - Scanning for specific device types
 * - Parsing custom manufacturer data
 * - Memory-efficient operation
 */

#include "nimconfig_observer_only.h"
#include <NimBLEObserverOnly.h>
#include <NimBLEScan.h>
#include <NimBLEAdvertisedDevice.h>

// ATC MiThermometer custom format detection
bool isATCDevice(const NimBLEAdvertisedDevice* device) {
    if (!device->haveManufacturerData()) return false;
    
    std::string mfgData = device->getManufacturerData();
    // ATC custom format typically starts with specific bytes
    // Check your ATC_MiThermometer documentation for exact format
    return mfgData.length() >= 13;  // Typical ATC format length
}

class ATCDeviceCallbacks : public NimBLEScanCallbacks {
private:
    int deviceCount = 0;
    unsigned long lastReportTime = 0;
    
public:
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        // Filter for ATC devices only
        if (!isATCDevice(advertisedDevice)) return;
        
        deviceCount++;
        
        Serial.printf("[ATC Device] %s, RSSI: %d", 
            advertisedDevice->getAddress().toString().c_str(),
            advertisedDevice->getRSSI());
        
        // Parse manufacturer data (example - adjust for your format)
        if (advertisedDevice->haveManufacturerData()) {
            std::string data = advertisedDevice->getManufacturerData();
            if (data.length() >= 13) {
                // Example parsing - adjust based on actual ATC format
                uint16_t temp = (data[6] << 8) | data[7];  // Temperature
                uint8_t humidity = data[8];                 // Humidity
                uint16_t battery_mv = (data[9] << 8) | data[10]; // Battery
                
                Serial.printf(", Temp: %.1f°C, Humidity: %d%%, Battery: %dmV",
                    temp / 10.0, humidity, battery_mv);
            }
        }
        
        if (advertisedDevice->haveName()) {
            Serial.printf(", Name: %s", advertisedDevice->getName().c_str());
        }
        
        Serial.println();
        
        // Report summary every 30 seconds
        if (millis() - lastReportTime > 30000) {
            Serial.printf("\n--- Found %d ATC devices in last 30 seconds ---\n\n", 
                deviceCount);
            deviceCount = 0;
            lastReportTime = millis();
        }
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        Serial.printf("Scan ended (reason %d). Restarting...\n", reason);
        // Scan will be restarted in loop()
    }
};

// Global variables
NimBLEScan* pBLEScan = nullptr;
ATCDeviceCallbacks* pCallbacks = nullptr;
bool scanActive = false;

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000) { delay(10); }
    
    Serial.println("\n=== ATC MiThermometer Observer Test ===");
    Serial.println("This demo shows proper observer-only initialization");
    Serial.printf("Initial free heap: %d bytes\n", ESP.getFreeHeap());
    
    // Step 1: Initialize NimBLE Observer-Only mode
    Serial.println("\n1. Initializing NimBLE Observer-Only mode...");
    
    if (!NimBLEObserverOnly::init("ATC_Scanner")) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        Serial.println("Troubleshooting:");
        Serial.println("- Check CONFIG_BT_ENABLED in menuconfig");
        Serial.println("- Ensure sufficient free memory (>100KB)");
        Serial.println("- Try erasing flash and reflashing");
        
        // Blink LED to indicate error
        pinMode(LED_BUILTIN, OUTPUT);
        while(1) {
            digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
            delay(500);
        }
    }
    
    Serial.println("✓ NimBLE initialized successfully");
    
    // Step 2: Verify initialization
    if (!NimBLEObserverOnly::isInitialized()) {
        Serial.println("ERROR: NimBLE not properly initialized!");
        while(1) { delay(1000); }
    }
    
    // Step 3: Get scan object
    Serial.println("\n2. Getting scan object...");
    pBLEScan = NimBLEObserverOnly::getScan();
    
    if (!pBLEScan) {
        Serial.println("ERROR: Failed to get scan object!");
        Serial.println("This should not happen after successful init.");
        while(1) { delay(1000); }
    }
    
    Serial.println("✓ Got scan object successfully");
    
    // Step 4: Configure scanning
    Serial.println("\n3. Configuring scan parameters...");
    
    pCallbacks = new ATCDeviceCallbacks();
    pBLEScan->setScanCallbacks(pCallbacks, false);  // false = don't want duplicates
    
    pBLEScan->setActiveScan(false);  // Passive scan (lower power, no scan requests)
    pBLEScan->setInterval(100);       // How often to scan (100 * 0.625ms = 62.5ms)
    pBLEScan->setWindow(50);          // How long to scan (50 * 0.625ms = 31.25ms)
    pBLEScan->setMaxResults(0);       // 0 = don't store results (callback only)
    
    Serial.println("✓ Scan configured for ATC devices");
    
    // Print configuration summary
    Serial.println("\n=== Configuration Summary ===");
    Serial.printf("Device address: %s\n", 
        NimBLEObserverOnly::getAddress().toString().c_str());
    Serial.printf("Free heap after setup: %d bytes\n", ESP.getFreeHeap());
    Serial.println("Scan mode: Passive (observer-only)");
    Serial.println("Target devices: ATC MiThermometer");
    Serial.println("=============================\n");
    
    // Start scanning
    Serial.println("Starting scan for ATC devices...\n");
    
    if (!pBLEScan->start(0)) {  // 0 = scan forever
        Serial.println("ERROR: Failed to start scan!");
        while(1) { delay(1000); }
    }
    
    scanActive = true;
}

void loop() {
    // Check if scan is still active
    if (pBLEScan && !pBLEScan->isScanning() && scanActive) {
        Serial.println("\nWARNING: Scan stopped unexpectedly, restarting...");
        
        delay(1000);  // Brief delay before restart
        
        if (!pBLEScan->start(0)) {
            Serial.println("ERROR: Failed to restart scan!");
            scanActive = false;
        }
    }
    
    // Print periodic status every minute
    static unsigned long lastStatusTime = 0;
    if (millis() - lastStatusTime > 60000) {
        lastStatusTime = millis();
        
        Serial.println("\n=== Status Report ===");
        Serial.printf("Uptime: %lu seconds\n", millis() / 1000);
        Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
        Serial.printf("Initialized: %s\n", 
            NimBLEObserverOnly::isInitialized() ? "Yes" : "No");
        Serial.printf("Scanning: %s\n", 
            (pBLEScan && pBLEScan->isScanning()) ? "Yes" : "No");
        Serial.println("====================\n");
    }
    
    delay(1000);
}