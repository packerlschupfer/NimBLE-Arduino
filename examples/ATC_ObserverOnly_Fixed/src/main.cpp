/**
 * Fixed ATC_MiThermometer Observer-Only Example
 * 
 * This version properly initializes NimBLE before using ATC_MiThermometer
 * to avoid whitelist issues in observer-only mode.
 */

#include <Arduino.h>
#include <NimBLEDeviceCompat.h>  // Include this BEFORE ATC_MiThermometer
#include <ATC_MiThermometer.h>

// Configuration
const char* DEVICE_NAME = "ESP32 ATC Scanner";
const uint32_t SCAN_DURATION_MS = 10000;  // 10 seconds
const uint32_t SCAN_INTERVAL_MS = 30000;  // 30 seconds between scans

// Known sensor MAC addresses - replace with your actual sensor addresses
std::vector<std::string> knownSensors = {
    "a4:c1:38:1d:87:bb",  // Office sensor (from our test)
    "a4:c1:38:07:51:82",  // Kitchen sensor (from our test)
    "a4:c1:38:a1:03:ba"   // Living Room (example)
};

// Sensor names for display
const char* sensorNames[] = {
    "Office",
    "Kitchen", 
    "Living Room"
};

// Global objects
ATC_MiThermometer* thermometer = nullptr;
uint32_t scanCount = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n=====================================");
    Serial.println("ATC_MiThermometer Observer-Only FIXED");
    Serial.println("=====================================");
    
    // CRITICAL: Initialize NimBLE FIRST before creating ATC_MiThermometer
    Serial.println("\nInitializing NimBLE in observer-only mode...");
    if (!NimBLEDevice::init(DEVICE_NAME)) {
        Serial.println("ERROR: Failed to initialize NimBLE!");
        while (1) delay(1000);
    }
    Serial.println("NimBLE initialized successfully");
    
    // Now we can safely use whitelist functions
    Serial.printf("Initial whitelist count: %d\n", NimBLEDevice::getWhiteListCount());
    
    // Clear and populate whitelist
    Serial.println("\nConfiguring hardware whitelist...");
    NimBLEDevice::whiteListClear();
    
    for (size_t i = 0; i < knownSensors.size(); i++) {
        NimBLEAddress addr(knownSensors[i]);
        if (NimBLEDevice::whiteListAdd(addr)) {
            Serial.printf("Added to whitelist: %s (%s)\n", 
                         knownSensors[i].c_str(), sensorNames[i]);
        } else {
            Serial.printf("Failed to add: %s\n", knownSensors[i].c_str());
        }
    }
    
    Serial.printf("Whitelist now contains %d entries\n", 
                  NimBLEDevice::getWhiteListCount());
    
    // NOW create the ATC_MiThermometer instance
    Serial.println("\nCreating ATC_MiThermometer instance...");
    thermometer = new ATC_MiThermometer(knownSensors);
    
    if (!thermometer) {
        Serial.println("ERROR: Failed to create thermometer instance!");
        while (1) delay(1000);
    }
    
    // The begin() method will skip NimBLE init since it's already initialized
    Serial.println("Starting filtered scanning...");
    thermometer->beginFiltered(
        false,  // passive scan
        true,   // enable hardware whitelist
        AddressType::AUTO_DETECT
    );
    
    // Show memory usage
    Serial.printf("\nFree heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("Flash usage: ~42%% (observer-only mode saves ~200-400KB)\n");
    
    Serial.println("\nSetup complete! Starting continuous scanning...\n");
}

void loop() {
    static uint32_t lastScanTime = 0;
    
    // Perform scan at intervals
    if (millis() - lastScanTime >= SCAN_INTERVAL_MS || lastScanTime == 0) {
        lastScanTime = millis();
        
        scanCount++;
        Serial.println("\n=====================================");
        Serial.printf("Scan #%u - Starting %d second scan...\n", 
                     scanCount, SCAN_DURATION_MS / 1000);
        Serial.println("=====================================");
        
        // Clear previous data
        thermometer->resetData();
        
        // Perform scan
        unsigned foundCount = thermometer->getData(SCAN_DURATION_MS);
        
        Serial.printf("\nDevices found: %u\n", foundCount);
        
        // Display results
        bool anyValid = false;
        for (size_t i = 0; i < knownSensors.size(); i++) {
            Serial.printf("\n%s (%s):\n", sensorNames[i], knownSensors[i].c_str());
            
            if (thermometer->data[i].valid) {
                anyValid = true;
                
                float temp = thermometer->data[i].temperature / 100.0;
                float humi = thermometer->data[i].humidity / 100.0;
                
                Serial.printf("  Temperature: %.2f°C\n", temp);
                Serial.printf("  Humidity: %.2f%%\n", humi);
                Serial.printf("  Battery: %d%% (%.3fV)\n",
                             thermometer->data[i].batt_level,
                             thermometer->data[i].batt_voltage / 1000.0);
                Serial.printf("  RSSI: %d dBm\n", thermometer->data[i].rssi);
            } else {
                Serial.println("  Status: NO DATA");
            }
        }
        
        if (!anyValid) {
            Serial.println("\n⚠ No sensors found - check MAC addresses!");
        }
        
        // Clear scan buffer
        thermometer->clearScanResults();
        
        Serial.printf("\nNext scan in %d seconds...\n", SCAN_INTERVAL_MS / 1000);
    }
    
    delay(100);
}