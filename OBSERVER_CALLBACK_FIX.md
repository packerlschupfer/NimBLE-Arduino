# Observer-Only Custom Callback Fix

## Problem
The custom scan callbacks in the Observer_Only_Test example were not being invoked. Instead, the default callbacks were being used, as evidenced by the log output:
```
D NimBLEScanCallbacks: Discovered: Name: , Address: a4:c1:38:1d:87:bb
```

## Root Cause
The `MyScanCallbacks` class was missing the `public:` access specifier, making all its methods private by default. In C++, class members are private by default (unlike struct members which are public).

## The Issue
```cpp
// INCORRECT - methods are private
class MyScanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        // This is PRIVATE and cannot be called by NimBLEScan
    }
};
```

## The Fix
```cpp
// CORRECT - methods are public
class MyScanCallbacks : public NimBLEScanCallbacks {
public:  // <-- This was missing!
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
        // This is PUBLIC and can be called by NimBLEScan
    }
};
```

## Additional Fix
The method signature also needed to use `const` to match the base class:
```cpp
// Base class declaration in NimBLEScanCallbacks:
virtual void onResult(const NimBLEAdvertisedDevice* advertisedDevice);

// So the override must also use const:
void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
```

## Verification
With the fix applied, custom callbacks are now properly invoked:
```
Device 1: a4:c1:38:1d:87:bb, RSSI: -70
```

## Lesson Learned
Always remember to specify `public:` access for callback methods in C++ classes. The compiler won't warn about this because private virtual methods are valid C++ (they can be called by the base class), but they won't work as expected for callbacks.