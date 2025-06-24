"""
PlatformIO build script for NimBLE observer-only mode

This script adjusts the build configuration when CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY
is defined, significantly reducing the number of compiled files.
"""

Import("env")

# Check if observer-only mode is enabled
build_flags = env.get("BUILD_FLAGS", [])
cppdefines = env.get("CPPDEFINES", [])

observer_only = False
for flag in build_flags + [str(d) for d in cppdefines]:
    if "CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY" in str(flag):
        observer_only = True
        break

if observer_only:
    print("NimBLE: Building in observer-only mode")
    
    # Files needed for observer-only mode
    observer_cpp_files = [
        "+<NimBLEObserverOnly.cpp>",
        "+<NimBLEScan.cpp>",
        "+<NimBLEAdvertisedDevice.cpp>",
        "+<NimBLEAddress.cpp>",
        "+<NimBLEUUID.cpp>",
        "+<NimBLEUtils.cpp>",
        "+<NimBLELog.cpp>",
        "+<nimble_stubs.cpp>",
    ]
    
    # Exclude all other C++ files
    exclude_cpp_files = [
        "-<NimBLE2904.cpp>",
        "-<NimBLEAdvertising.cpp>",
        "-<NimBLEAdvertisementData.cpp>",
        "-<NimBLEAttValue.cpp>",
        "-<NimBLEBeacon.cpp>",
        "-<NimBLECharacteristic.cpp>",
        "-<NimBLEClient.cpp>",
        "-<NimBLEDescriptor.cpp>",
        "-<NimBLEDevice.cpp>",
        "-<NimBLEEddystoneTLM.cpp>",
        "-<NimBLEExtAdvertising.cpp>",
        "-<NimBLEHIDDevice.cpp>",
        "-<NimBLEL2CAPChannel.cpp>",
        "-<NimBLEL2CAPServer.cpp>",
        "-<NimBLERemoteCharacteristic.cpp>",
        "-<NimBLERemoteDescriptor.cpp>",
        "-<NimBLERemoteService.cpp>",
        "-<NimBLERemoteValueAttribute.cpp>",
        "-<NimBLEServer.cpp>",
        "-<NimBLEService.cpp>",
    ]
    
    # NimBLE C files to include for scanning
    nimble_c_files = [
        # Core host files
        "+<nimble/nimble/host/src/ble_hs*.c>",
        "+<nimble/nimble/host/src/ble_gap.c>",
        "+<nimble/nimble/host/src/ble_uuid.c>",
        "+<nimble/nimble/host/src/ble_att.c>",
        "+<nimble/nimble/host/src/ble_att_cmd.c>",
        "+<nimble/nimble/host/src/ble_ibeacon.c>",
        "+<nimble/nimble/host/src/ble_eddystone.c>",
        
        # Minimal GAP service
        "+<nimble/nimble/host/services/gap/src/ble_svc_gap.c>",
        
        # Transport and porting
        "+<nimble/nimble/transport/src/transport.c>",
        "+<nimble/porting/>",
        "+<nimble/esp_port/>",
        
        # Minimal controller for scanning
        "+<nimble/nimble/controller/src/ble_ll.c>",
        "+<nimble/nimble/controller/src/ble_ll_hci*.c>",
        "+<nimble/nimble/controller/src/ble_ll_scan*.c>",
        "+<nimble/nimble/controller/src/ble_ll_sched.c>",
        "+<nimble/nimble/controller/src/ble_ll_xcvr.c>",
        "+<nimble/nimble/controller/src/ble_ll_whitelist.c>",
        "+<nimble/nimble/controller/src/ble_ll_rand.c>",
        "+<nimble/nimble/controller/src/ble_ll_rfmgmt.c>",
        "+<nimble/nimble/controller/src/ble_ll_trace.c>",
        "+<nimble/nimble/controller/src/ble_ll_ctrl.c>",
        "+<nimble/nimble/controller/src/ble_ll_supp_cmd.c>",
        "+<nimble/nimble/controller/src/ble_ll_utils.c>",
        "+<nimble/nimble/controller/src/ble_phy.c>",
        
        # Exclude files we don't need
        "-<nimble/nimble/host/src/ble_sm*.c>",
        "-<nimble/nimble/host/src/ble_gatt*.c>",
        "-<nimble/nimble/host/src/ble_gatts*.c>",
        "-<nimble/nimble/host/src/ble_l2cap*.c>",
        "-<nimble/nimble/host/src/ble_eatt*.c>",
        "-<nimble/nimble/host/src/ble_att_svr*.c>",
        "-<nimble/nimble/host/src/ble_att_clt*.c>",
        "-<nimble/nimble/host/src/ble_store*.c>",
        "-<nimble/nimble/host/src/ble_hs_pvcy*.c>",
        "-<nimble/nimble/host/src/ble_hs_resolv*.c>",
        "-<nimble/nimble/host/src/ble_hs_flow.c>",
        "-<nimble/nimble/host/src/ble_hs_mbuf.c>",
        "-<nimble/nimble/host/src/ble_hs_periodic*.c>",
        "-<nimble/nimble/host/src/ble_hs_stop.c>",
        "-<nimble/nimble/host/src/ble_monitor*.c>",
        "-<nimble/nimble/host/src/ble_dtm*.c>",
        "-<nimble/nimble/host/src/ble_ead.c>",
        
        # Exclude all other services
        "-<nimble/nimble/host/services/ans/>",
        "-<nimble/nimble/host/services/bas/>",
        "-<nimble/nimble/host/services/dis/>",
        "-<nimble/nimble/host/services/gatt/>",
        "-<nimble/nimble/host/services/ias/>",
        "-<nimble/nimble/host/services/ipss/>",
        "-<nimble/nimble/host/services/lls/>",
        "-<nimble/nimble/host/services/tps/>",
        
        # Exclude all store implementations
        "-<nimble/nimble/host/store/>",
        "-<nimble/nimble/host/store/config/>",
        "-<nimble/nimble/host/store/ram/>",
        
        # Exclude mesh
        "-<nimble/nimble/host/mesh/>",
        
        # Exclude controller features we don't need
        "-<nimble/nimble/controller/src/ble_ll_conn*.c>",
        "-<nimble/nimble/controller/src/ble_ll_adv*.c>",
        "-<nimble/nimble/controller/src/ble_ll_dtm*.c>",
        "-<nimble/nimble/controller/src/ble_ll_sync*.c>",
        "-<nimble/nimble/controller/src/ble_ll_iso*.c>",
        "-<nimble/nimble/controller/src/ble_ll_cs*.c>",
        "-<nimble/nimble/controller/src/ble_ll_phy*.c>",
        
        # Exclude crypto if not needed
        "-<nimble/ext/tinycrypt/>",
        
        # Exclude tests
        "-<nimble/nimble/host/test/>",
        "-<nimble/nimble/controller/test/>",
        "-<nimble/nimble/transport/test/>",
    ]
    
    # Build the complete source filter
    src_filter = observer_cpp_files + exclude_cpp_files + nimble_c_files
    
    # Apply the filter
    env.Replace(SRC_FILTER=src_filter)
    
    # Add observer-only specific flags
    env.Append(CPPDEFINES=[
        ("CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY", 1),
        ("CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED", 1),
        ("CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED", 1),
        ("CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED", 1),
        ("CONFIG_BT_NIMBLE_MAX_CONNECTIONS", 0),
        ("MYNEWT_VAL_BLE_MAX_CONNECTIONS", 0),
        ("CONFIG_BT_NIMBLE_SM_LEGACY", 0),
        ("CONFIG_BT_NIMBLE_SM_SC", 0),
    ])
    
    # Add aggressive optimization flags
    env.Append(CCFLAGS=[
        "-ffunction-sections",
        "-fdata-sections",
        "-fno-exceptions",
    ])
    
    env.Append(LINKFLAGS=[
        "-Wl,--gc-sections",
    ])
    
    print("NimBLE: Observer-only configuration applied")
    print("NimBLE: Excluded all client, server, and advertising code")
else:
    print("NimBLE: Building in standard mode")