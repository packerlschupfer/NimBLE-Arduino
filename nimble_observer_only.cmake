# NimBLE Observer-Only Build Configuration
# 
# This CMake file can be included to build only the minimal set of files
# needed for BLE observer (scanning) functionality.
#
# Usage in your CMakeLists.txt:
#   include(${NIMBLE_PATH}/nimble_observer_only.cmake)

# Define observer-only mode
add_definitions(-DCONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY=1)

# Core NimBLE sources needed for observer mode
set(NIMBLE_OBSERVER_SRCS
    # Host core
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_hci.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_hci_evt.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_hci_cmd.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_cfg.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_startup.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_misc.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_log.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_mbuf.c"
    
    # GAP (minimal - just for scanning)
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_gap.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_gap_priv.h"
    
    # ATT (minimal)
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_att.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_att_cmd.c"
    
    # UUID handling
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_uuid.c"
    
    # Address handling  
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_id.c"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_hs_conn.c"
    
    # Minimal GAP service
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/gap/src/ble_svc_gap.c"
    
    # Transport
    "${NIMBLE_PATH}/src/nimble/porting/nimble/src/nimble_port.c"
    "${NIMBLE_PATH}/src/nimble/porting/npl/freertos/src/nimble_port_freertos.c"
    "${NIMBLE_PATH}/src/nimble/porting/npl/freertos/src/npl_os_freertos.c"
    
    # HCI transport
    "${NIMBLE_PATH}/src/nimble/nimble/transport/src/transport.c"
    
    # Memory management
    "${NIMBLE_PATH}/src/nimble/porting/nimble/src/os_mempool.c"
    "${NIMBLE_PATH}/src/nimble/porting/nimble/src/os_mbuf.c"
    "${NIMBLE_PATH}/src/nimble/porting/nimble/src/os_msys_init.c"
    
    # ESP32 specific
    "${NIMBLE_PATH}/src/nimble/esp_port/esp-hci/src/esp_nimble_hci.c"
    "${NIMBLE_PATH}/src/nimble/porting/npl/freertos/src/npl_os_freertos.c"
    
    # C++ wrappers (minimal)
    "${NIMBLE_PATH}/src/NimBLEAddress.cpp"
    "${NIMBLE_PATH}/src/NimBLEScan.cpp"
    "${NIMBLE_PATH}/src/NimBLEAdvertisedDevice.cpp"
    "${NIMBLE_PATH}/src/NimBLEUtils.cpp"
    "${NIMBLE_PATH}/src/NimBLEObserverOnly.cpp"
    
    # Stub implementations
    "${NIMBLE_PATH}/src/nimble_stubs.cpp"
)

# Explicitly exclude these directories from build
set(NIMBLE_EXCLUDE_DIRS
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_sm*"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_gatt*"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_l2cap*"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_att_svr*"
    "${NIMBLE_PATH}/src/nimble/nimble/host/src/ble_att_clt*"
    "${NIMBLE_PATH}/src/nimble/nimble/host/store"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/ans"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/bas"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/dis"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/gatt"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/ias"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/ipss"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/lls"
    "${NIMBLE_PATH}/src/nimble/nimble/host/services/tps"
    "${NIMBLE_PATH}/src/nimble/nimble/host/mesh"
    "${NIMBLE_PATH}/src/nimble/ext/tinycrypt"
)

# Only include controller files needed for scanning
set(NIMBLE_CONTROLLER_SRCS
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_hci.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_scan.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_sched.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_xcvr.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_whitelist.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_rand.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_rfmgmt.c"
    "${NIMBLE_PATH}/src/nimble/nimble/controller/src/ble_ll_trace.c"
)

# Set the final source list
set(NIMBLE_SRCS ${NIMBLE_OBSERVER_SRCS} ${NIMBLE_CONTROLLER_SRCS})