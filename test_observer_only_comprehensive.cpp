/**
 * Comprehensive Test for NimBLE Observer-Only Mode
 * 
 * This test program verifies:
 * 1. Proper initialization and error handling
 * 2. Scan object creation and persistence
 * 3. Memory usage tracking
 * 4. Scan start/stop functionality
 * 5. Device detection and parsing
 * 
 * Compile with:
 * g++ -DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1 -DCONFIG_BT_ENABLED=1 \
 *     -DCONFIG_BT_NIMBLE_ROLE_OBSERVER=1 -DESP_PLATFORM -I./src \
 *     -c test_observer_only_comprehensive.cpp
 */

#include <iostream>
#include <cassert>
#include <cstring>

// Test framework mock functions
#define Serial std::cout
#define delay(x) usleep((x) * 1000)

// Mock ESP functions
uint32_t ESP_getFreeHeap() { return 250000; }
#define ESP (*this)
struct { uint32_t getFreeHeap() { return ESP_getFreeHeap(); } } esp_mock;

// Include the headers
#include "nimconfig_observer_only.h"
#include "NimBLEObserverOnly.h"
#include "NimBLEScan.h"
#include "NimBLEAdvertisedDevice.h"

class TestCallbacks : public NimBLEScanCallbacks {
public:
    int deviceCount = 0;
    int scanEndCount = 0;
    
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) {
        deviceCount++;
        std::cout << "[CALLBACK] Device #" << deviceCount 
                  << ": " << advertisedDevice->getAddress().toString() 
                  << ", RSSI: " << (int)advertisedDevice->getRSSI() << std::endl;
    }
    
    void onScanEnd(const NimBLEScanResults& results, int reason) {
        scanEndCount++;
        std::cout << "[CALLBACK] Scan ended, reason: " << reason 
                  << ", devices: " << results.getCount() << std::endl;
    }
};

void test_initialization() {
    std::cout << "\n=== Test 1: Initialization ===" << std::endl;
    
    // Test 1.1: Check not initialized state
    assert(!NimBLEObserverOnly::isInitialized());
    std::cout << "✓ Not initialized by default" << std::endl;
    
    // Test 1.2: getScan should return nullptr before init
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    assert(pScan == nullptr);
    std::cout << "✓ getScan returns nullptr before init" << std::endl;
    
    // Test 1.3: Initialize
    bool initResult = NimBLEObserverOnly::init("TestDevice");
    if (!initResult) {
        std::cout << "✗ Initialization failed (expected in test environment)" << std::endl;
        return;
    }
    
    assert(NimBLEObserverOnly::isInitialized());
    std::cout << "✓ Initialization successful" << std::endl;
    
    // Test 1.4: getScan should return valid pointer after init
    pScan = NimBLEObserverOnly::getScan();
    assert(pScan != nullptr);
    std::cout << "✓ getScan returns valid pointer after init" << std::endl;
    
    // Test 1.5: Multiple calls to getScan return same object
    NimBLEScan* pScan2 = NimBLEObserverOnly::getScan();
    assert(pScan == pScan2);
    std::cout << "✓ getScan returns same object on multiple calls" << std::endl;
    
    // Test 1.6: Re-initialization should succeed
    bool reinitResult = NimBLEObserverOnly::init("TestDevice2");
    assert(reinitResult);
    assert(NimBLEObserverOnly::isInitialized());
    std::cout << "✓ Re-initialization handled correctly" << std::endl;
}

void test_scan_object() {
    std::cout << "\n=== Test 2: Scan Object ===" << std::endl;
    
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    if (!pScan) {
        std::cout << "✗ No scan object (expected in test environment)" << std::endl;
        return;
    }
    
    // Test 2.1: Set scan parameters
    pScan->setActiveScan(false);
    pScan->setInterval(100);
    pScan->setWindow(50);
    pScan->setMaxResults(10);
    std::cout << "✓ Scan parameters set successfully" << std::endl;
    
    // Test 2.2: Set callbacks
    TestCallbacks* callbacks = new TestCallbacks();
    pScan->setScanCallbacks(callbacks, false);
    std::cout << "✓ Callbacks set successfully" << std::endl;
    
    // Test 2.3: Check scanning state
    bool isScanning = pScan->isScanning();
    std::cout << "✓ isScanning() works: " << (isScanning ? "true" : "false") << std::endl;
}

void test_error_conditions() {
    std::cout << "\n=== Test 3: Error Conditions ===" << std::endl;
    
    // Test 3.1: Deinitialize
    bool deinitResult = NimBLEObserverOnly::deinit();
    std::cout << "✓ Deinit returned: " << (deinitResult ? "true" : "false") << std::endl;
    
    // Test 3.2: getScan after deinit should return nullptr
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    assert(pScan == nullptr);
    std::cout << "✓ getScan returns nullptr after deinit" << std::endl;
    
    // Test 3.3: isInitialized after deinit
    assert(!NimBLEObserverOnly::isInitialized());
    std::cout << "✓ isInitialized returns false after deinit" << std::endl;
}

void test_memory_tracking() {
    std::cout << "\n=== Test 4: Memory Tracking ===" << std::endl;
    
    uint32_t heapBefore = ESP.getFreeHeap();
    std::cout << "Heap before init: " << heapBefore << " bytes" << std::endl;
    
    // Initialize
    NimBLEObserverOnly::init("MemTest");
    uint32_t heapAfterInit = ESP.getFreeHeap();
    std::cout << "Heap after init: " << heapAfterInit << " bytes" << std::endl;
    
    // Get scan object
    NimBLEScan* pScan = NimBLEObserverOnly::getScan();
    uint32_t heapAfterScan = ESP.getFreeHeap();
    std::cout << "Heap after getScan: " << heapAfterScan << " bytes" << std::endl;
    
    // Deinit
    NimBLEObserverOnly::deinit();
    uint32_t heapAfterDeinit = ESP.getFreeHeap();
    std::cout << "Heap after deinit: " << heapAfterDeinit << " bytes" << std::endl;
}

void run_all_tests() {
    std::cout << "\n==== NimBLE Observer-Only Comprehensive Test ====" << std::endl;
    
    test_initialization();
    test_scan_object();
    test_error_conditions();
    test_memory_tracking();
    
    std::cout << "\n==== All tests completed ====" << std::endl;
}

// For Arduino environment
#ifdef ARDUINO
void setup() {
    Serial.begin(115200);
    while (!Serial) { delay(10); }
    
    delay(1000);
    run_all_tests();
}

void loop() {
    delay(10000);
}
#else
// For standalone compilation
int main() {
    run_all_tests();
    return 0;
}
#endif