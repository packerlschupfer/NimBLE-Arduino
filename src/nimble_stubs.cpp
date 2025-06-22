/*
 * NimBLE Stub Implementations for Observer-Only Mode
 * 
 * This file provides minimal stub implementations for functions that are
 * referenced during linking but never actually called in observer-only mode.
 * This prevents the linker from pulling in entire subsystems.
 */

#include "nimconfig.h"

#if defined(CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY) || \
    (defined(CONFIG_BT_NIMBLE_ROLE_CENTRAL_DISABLED) && \
     defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL_DISABLED) && \
     defined(CONFIG_BT_NIMBLE_ROLE_BROADCASTER_DISABLED))

#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "host/ble_gatt.h"
#include "host/ble_att.h"
#include "host/ble_l2cap.h"
#include "host/ble_store.h"
#include "host/ble_uuid.h"

extern "C" {

// GATT Client stubs - prevent linking full GATT client code
int ble_gattc_exchange_mtu(uint16_t conn_handle, ble_gatt_mtu_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_disc_all_svcs(uint16_t conn_handle, ble_gatt_disc_svc_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_disc_svc_by_uuid(uint16_t conn_handle, const ble_uuid_t *uuid,
                               ble_gatt_disc_svc_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_disc_all_chrs(uint16_t conn_handle, uint16_t start_handle,
                            uint16_t end_handle, ble_gatt_chr_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_disc_all_dscs(uint16_t conn_handle, uint16_t start_handle,
                            uint16_t end_handle, ble_gatt_dsc_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_read(uint16_t conn_handle, uint16_t attr_handle,
                   ble_gatt_attr_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_write(uint16_t conn_handle, uint16_t attr_handle,
                    struct os_mbuf *om, ble_gatt_attr_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_write_no_rsp(uint16_t conn_handle, uint16_t attr_handle,
                           struct os_mbuf *om) {
    return BLE_HS_ENOTSUP;
}

int ble_gattc_notify_custom(uint16_t conn_handle, uint16_t att_handle,
                            struct os_mbuf *om) {
    return BLE_HS_ENOTSUP;
}

// GATT Server stubs - prevent linking full GATT server code
int ble_gatts_count_cfg(const struct ble_gatt_svc_def *defs) {
    return 0;
}

int ble_gatts_add_svcs(const struct ble_gatt_svc_def *svcs) {
    return BLE_HS_ENOTSUP;
}

int ble_gatts_register_svcs(const struct ble_gatt_svc_def *svcs,
                            ble_gatt_register_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gatts_find_svc(const ble_uuid_t *uuid, uint16_t *out_handle) {
    return BLE_HS_ENOTSUP;
}

// Connection management stubs
int ble_gap_connect(uint8_t own_addr_type, const ble_addr_t *peer_addr,
                    int32_t duration_ms, const struct ble_gap_conn_params *conn_params,
                    ble_gap_event_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_terminate(uint16_t conn_handle, uint8_t hci_reason) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_update_params(uint16_t conn_handle, const struct ble_gap_upd_params *params) {
    return BLE_HS_ENOTSUP;
}

// Advertising stubs (for observer-only mode)
int ble_gap_adv_start(uint8_t own_addr_type, const ble_addr_t *direct_addr,
                      int32_t duration_ms, const struct ble_gap_adv_params *adv_params,
                      ble_gap_event_fn *cb, void *cb_arg) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_adv_stop(void) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_adv_set_data(const uint8_t *data, int data_len) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_adv_rsp_set_data(const uint8_t *data, int data_len) {
    return BLE_HS_ENOTSUP;
}

// Security Manager stubs
int ble_gap_security_initiate(uint16_t conn_handle) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_pair_initiate(uint16_t conn_handle) {
    return BLE_HS_ENOTSUP;
}

int ble_gap_encryption_initiate(uint16_t conn_handle, const uint8_t *ltk,
                                uint16_t ediv, uint64_t rand_val, int auth) {
    return BLE_HS_ENOTSUP;
}

// L2CAP stubs
#if CONFIG_BT_NIMBLE_L2CAP_COC_MAX_NUM == 0
struct ble_l2cap_chan *ble_l2cap_connect(uint16_t conn_handle, uint16_t psm,
                                         uint16_t mtu, struct os_mbuf *sdu_rx,
                                         ble_l2cap_event_fn *cb, void *cb_arg) {
    return NULL;
}

int ble_l2cap_disconnect(struct ble_l2cap_chan *chan) {
    return BLE_HS_ENOTSUP;
}

int ble_l2cap_send(struct ble_l2cap_chan *chan, struct os_mbuf *sdu_tx) {
    return BLE_HS_ENOTSUP;
}
#endif

// ATT server stubs for observer-only
void ble_att_svr_prev_notify_tx(uint16_t conn_handle) {
    // No-op
}

struct ble_att_svr_conn *ble_att_svr_find_conn(uint16_t conn_handle) {
    return NULL;
}

// Minimal store stubs if store is disabled
#if MYNEWT_VAL(BLE_STORE_CONFIG_DISABLED)
int ble_store_status_rr(struct ble_store_status_event *event, void *arg) {
    return 0;
}

int ble_store_util_status_rr(struct ble_store_status_event *event, void *arg) {
    return 0;
}
#endif

} // extern "C"

#endif // CONFIG_BT_NIMBLE_ROLE_OBSERVER_ONLY