# ATC_MiThermometer Whitelist Fix

## Problem
ATC_MiThermometer was finding 0 devices when using hardware whitelist filtering, even though:
- Observer-only scanning worked (found 14 devices)
- The target device was advertising
- Whitelist configuration appeared correct

## Root Cause
The macro `NIMBLE_HAS_WHITELIST` was not defined anywhere, causing all whitelist population code to be skipped by the preprocessor:

```cpp
#if NIMBLE_HAS_WHITELIST
    // This code was never executed!
    NimBLEDevice::whiteListAdd(randomAddr);
#endif
```

Debug output showed:
```
Whitelist count: 0
```

## Solution
Added the following to ATC_MiThermometer.h:
```cpp
// Define whitelist support - NimBLE has whitelist functionality
#define NIMBLE_HAS_WHITELIST 1
```

## Result
With this fix:
1. Whitelist population code now executes
2. Devices are added to the hardware whitelist
3. Whitelist filtering works correctly
4. Only whitelisted devices are reported during scans

## Verification
After the fix, you should see:
```
Whitelist contains 1 entries  // Or however many devices you configured
```

Instead of:
```
Whitelist count: 0
```

## Lesson Learned
Always verify that conditional compilation macros are actually defined. The code looked correct but was completely bypassed due to a missing #define.