# Observer-Only Build Optimization Recommendations

This document outlines additional subsystems and features that could be conditionally compiled out to achieve even greater memory savings for observer-only builds.

## Summary

Beyond the current observer-only support that saves ~42KB, an additional 35-50KB of flash memory could be saved by implementing the recommendations below, achieving a total savings of **75-90KB**.

## High Priority Optimizations (15-20KB)

### 1. Store/Persistence Mechanisms (~4-6KB) ✅ IMPLEMENTED
- **Files**: `ble_store.c`, `ble_store_util.c`
- **Implementation**: Added `CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED` option
- **Usage**: Set `#define CONFIG_BT_NIMBLE_STORE_CONFIG_DISABLED 1`
- **Status**: Complete - stub implementations provided for all store functions

### 2. Privacy and Address Resolution (~3-5KB)
- **Files**: `ble_hs_pvcy.c`, `ble_hs_resolv.c`
- **Recommendation**: Wrap with `#if MYNEWT_VAL(BLE_HOST_BASED_PRIVACY) && NIMBLE_BLE_CONNECT`
- **Rationale**: RPA generation and resolution lists aren't needed for passive scanning

### 3. Buffer Pool Optimization (~4-8KB)
- **Configuration**: Reduce memory pool allocations
- **Recommendation**: 
  ```cpp
  #define CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT 4  // From 12
  #define CONFIG_BT_NIMBLE_TRANSPORT_EVT_COUNT 4  // From 16
  ```
- **Rationale**: Fewer buffers needed without connections

### 4. GATT Caching (~5-7KB)
- **Files**: `ble_gattc_cache.c`
- **Status**: Already configurable via `MYNEWT_VAL(BLE_GATT_CACHING)`
- **Action**: Ensure it's set to 0 in observer-only builds

## Medium Priority Optimizations (10-15KB)

### 5. Whitelist/Filter Accept List (~2-3KB)
- **Files**: `ble_gap.c` (whitelist functions)
- **Recommendation**: Add `CONFIG_BT_NIMBLE_WHITELIST_DISABLED`
- **Rationale**: Connection whitelists not needed for scanning

### 6. Timer/Scheduling Features (~2-3KB)
- **Various files**: Connection supervision timers, ATT timers
- **Recommendation**: Conditionally compile timer initialization
- **Rationale**: Connection-related timers unnecessary

### 7. Enhanced ATT (EATT) (~3-4KB)
- **Files**: `ble_eatt.c`
- **Recommendation**: Wrap with `#if MYNEWT_VAL(BLE_EATT) && NIMBLE_BLE_CONNECT`
- **Rationale**: Multiple ATT bearers only used with connections

### 8. Direct Test Mode (DTM) (~1-2KB)
- **Files**: `ble_dtm.c`
- **Recommendation**: Add `MYNEWT_VAL(BLE_DTM_ENABLED)`
- **Rationale**: RF testing not needed in production

## Low Priority Optimizations (5-10KB)

### 9. UUID Handling (~1-2KB)
- **Files**: `ble_uuid.c`
- **Recommendation**: Remove GATT-specific UUID comparison functions
- **Rationale**: Some UUID operations only needed for service discovery

### 10. iBeacon Advertising (~1KB)
- **Files**: `ble_ibeacon.c`
- **Recommendation**: Keep parsing, remove advertising functions
- **Rationale**: Observer only receives beacons, doesn't transmit

### 11. Encrypted Advertising Data (~2-3KB)
- **Files**: `ble_ead.c`
- **Status**: Already wrapped with `MYNEWT_VAL(ENC_ADV_DATA)`
- **Action**: Ensure disabled for observer-only

### 12. Extended/Periodic Advertising (~4-5KB)
- **Files**: `ble_hs_periodic_sync.c`
- **Status**: Already wrapped with `MYNEWT_VAL(BLE_PERIODIC_ADV)`
- **Note**: Can be kept if extended advertising scanning is needed

## Proposed Observer-Only Master Configuration

Create a single configuration option that automatically sets all optimizations:

```cpp
// In nimconfig.h
#define CONFIG_BT_NIMBLE_OBSERVER_ONLY 1

#ifdef CONFIG_BT_NIMBLE_OBSERVER_ONLY
    // Disable all roles except observer
    #define CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED
    #define CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED
    #define CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED
    
    // Disable connections and L2CAP
    #define CONFIG_BT_NIMBLE_MAX_CONNECTIONS 0
    #define CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM 0
    
    // Disable security
    #define MYNEWT_VAL_BLE_SM_LVL 0
    
    // Disable storage
    #define CONFIG_BT_NIMBLE_STORE_DISABLED 1
    
    // Disable GATT
    #define CONFIG_BT_NIMBLE_GATT_MAX_PROCS 0
    #define CONFIG_BT_NIMBLE_GATT_CACHING 0
    
    // Disable privacy
    #define MYNEWT_VAL_BLE_HOST_BASED_PRIVACY 0
    
    // Disable whitelist
    #define CONFIG_BT_NIMBLE_WHITELIST_DISABLED 1
    
    // Disable test features
    #define MYNEWT_VAL_BLE_DTM_ENABLED 0
    #define MYNEWT_VAL_BLE_EATT 0
    
    // Reduce buffers
    #define CONFIG_BT_NIMBLE_MSYS_1_BLOCK_COUNT 4
    #define CONFIG_BT_NIMBLE_TRANSPORT_EVT_COUNT 4
#endif
```

## Implementation Strategy

1. **Phase 1**: Implement high-priority optimizations
   - Most benefit for least effort
   - Well-defined subsystems
   - Low risk of breaking functionality

2. **Phase 2**: Add medium-priority optimizations
   - Requires more careful code review
   - May need refactoring of existing code

3. **Phase 3**: Consider low-priority optimizations
   - Evaluate if complexity justifies savings
   - May require architectural changes

## Testing Requirements

Each optimization should be tested to ensure:
1. Observer functionality remains intact
2. No compilation errors on all platforms
3. Memory savings match estimates
4. No performance degradation in scanning

## Next Steps

1. Create feature request issues for each optimization category
2. Implement high-priority optimizations first
3. Add comprehensive testing for observer-only mode
4. Update documentation with new configuration options
5. Consider creating a `CONFIG_BT_NIMBLE_OBSERVER_ONLY` master switch