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
    
    # Additional source filters for observer-only mode
    env.Replace(SRC_FILTER=[
        "+<*.c>",
        "+<*.cpp>",
        "+<*.h>",
        # Include minimal host files
        "+<nimble/nimble/host/src/ble_hs*.c>",
        "+<nimble/nimble/host/src/ble_gap.c>",
        "+<nimble/nimble/host/src/ble_att.c>",
        "+<nimble/nimble/host/src/ble_att_cmd.c>",
        "+<nimble/nimble/host/src/ble_uuid.c>",
        "+<nimble/nimble/host/services/gap/src/>",
        # Include transport
        "+<nimble/nimble/transport/src/>",
        "+<nimble/porting/>",
        "+<nimble/esp_port/>",
        # Include minimal controller for scanning
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
        "+<nimble/nimble/controller/src/ble_ll_hci_ev.c>",
        "+<nimble/nimble/controller/src/ble_phy.c>",
        "+<nimble/nimble/controller/src/ble_ll_utils.c>",
        # Exclude everything else
        "-<nimble/nimble/host/src/ble_sm*.c>",
        "-<nimble/nimble/host/src/ble_gatt*.c>",
        "-<nimble/nimble/host/src/ble_l2cap*.c>",
        "-<nimble/nimble/host/src/ble_att_svr*.c>",
        "-<nimble/nimble/host/src/ble_att_clt*.c>",
        "-<nimble/nimble/host/src/ble_hs_pvcy*.c>",
        "-<nimble/nimble/host/src/ble_hs_resolv*.c>",
        "-<nimble/nimble/host/src/ble_store*.c>",
        "-<nimble/nimble/host/src/ble_hs_flow.c>",
        "-<nimble/nimble/host/src/ble_hs_mbuf.c>",
        "-<nimble/nimble/host/src/ble_hs_periodic_sync.c>",
        "-<nimble/nimble/host/src/ble_eatt*.c>",
        "-<nimble/nimble/host/src/ble_hs_stop.c>",
        "-<nimble/nimble/host/src/ble_monitor.c>",
        "-<nimble/nimble/host/src/ble_dtm*.c>",
        "-<nimble/nimble/host/src/ble_ead.c>",
        "-<nimble/nimble/host/store/>",
        "-<nimble/nimble/host/services/ans/>",
        "-<nimble/nimble/host/services/bas/>",
        "-<nimble/nimble/host/services/dis/>",
        "-<nimble/nimble/host/services/gatt/>",
        "-<nimble/nimble/host/services/ias/>",
        "-<nimble/nimble/host/services/ipss/>",
        "-<nimble/nimble/host/services/lls/>",
        "-<nimble/nimble/host/services/tps/>",
        "-<nimble/nimble/host/mesh/>",
        "-<nimble/nimble/controller/src/ble_ll_conn*.c>",
        "-<nimble/nimble/controller/src/ble_ll_adv*.c>",
        "-<nimble/nimble/controller/src/ble_ll_dtm*.c>",
        "-<nimble/nimble/controller/src/ble_ll_sync*.c>",
        "-<nimble/nimble/controller/src/ble_ll_iso*.c>",
        "-<nimble/nimble/controller/src/ble_ll_cs*.c>",
        "-<nimble/nimble/controller/src/ble_ll_phy*.c>",
        "-<nimble/ext/tinycrypt/>",
        "-<nimble/nimble/host/test/>",
        "-<nimble/nimble/controller/test/>",
        "-<nimble/nimble/transport/test/>",
        # Exclude C++ wrappers we don't need
        "-<NimBLEClient.cpp>",
        "-<NimBLEServer.cpp>",
        "-<NimBLEService.cpp>",
        "-<NimBLECharacteristic.cpp>",
        "-<NimBLEDescriptor.cpp>",
        "-<NimBLERemote*.cpp>",
        "-<NimBLEAdvertising.cpp>",
        "-<NimBLEExtAdvertising.cpp>",
        "-<NimBLEBeacon.cpp>",
        "-<NimBLEEddystone*.cpp>",
        "-<NimBLEHIDDevice.cpp>",
        "-<NimBLE2904.cpp>",
        "-<NimBLEL2CAP*.cpp>",
    ])
    
    # Add observer-only specific flags
    env.Append(CPPDEFINES=[
        ("CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY", 1),
        ("CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED", 1),
        ("CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED", 1),
        ("CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED", 1),
    ])
    
    # Add aggressive optimization flags
    env.Append(CCFLAGS=[
        "-ffunction-sections",
        "-fdata-sections",
    ])
    
    env.Append(LINKFLAGS=[
        "-Wl,--gc-sections",
    ])
    
    print("NimBLE: Observer-only configuration applied")
else:
    print("NimBLE: Building in standard mode")