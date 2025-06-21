/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/**
 * Stub implementation of BLE store functions for observer-only builds
 * where persistence is not needed.
 */

#include "syscfg/syscfg.h"

#if MYNEWT_VAL(BLE_STORE_CONFIG_DISABLED)

#include "nimble/nimble/host/include/host/ble_store.h"
#include "ble_hs_priv.h"

int
ble_store_read(int obj_type, const union ble_store_key *key,
               union ble_store_value *val)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_write(int obj_type, const union ble_store_value *val)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_delete(int obj_type, const union ble_store_key *key)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_clear(void)
{
    return 0;
}

int
ble_store_overflow_event(int obj_type, const union ble_store_value *value)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_persist_sec(int obj_type,
                      const struct ble_store_value_sec *value_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_persist_our_sec(const struct ble_store_value_sec *value_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_persist_peer_sec(const struct ble_store_value_sec *value_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_delete_our_sec(const struct ble_store_key_sec *key_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_delete_peer_sec(const struct ble_store_key_sec *key_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_read_peer_sec(const struct ble_store_key_sec *key_sec,
                        struct ble_store_value_sec *value_sec)
{
    return BLE_HS_ENOENT;
}

int
ble_store_write_peer_sec(const struct ble_store_value_sec *value_sec)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_read_cccd(const struct ble_store_key_cccd *key,
                    struct ble_store_value_cccd *out_value)
{
    return BLE_HS_ENOENT;
}

int
ble_store_write_cccd(const struct ble_store_value_cccd *value)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_delete_cccd(const struct ble_store_key_cccd *key)
{
    return BLE_HS_ENOTSUP;
}

void
ble_store_key_from_value_cccd(struct ble_store_key_cccd *key,
                              const struct ble_store_value_cccd *val)
{
    /* No-op */
}

void
ble_store_key_from_value_sec(struct ble_store_key_sec *out_key,
                             const struct ble_store_value_sec *value)
{
    /* No-op */
}

int
ble_store_read_csfc(const struct ble_store_key_csfc *key,
                    struct ble_store_value_csfc *out_value)
{
    return BLE_HS_ENOENT;
}

int
ble_store_write_csfc(const struct ble_store_value_csfc *value)
{
    return BLE_HS_ENOTSUP;
}

int
ble_store_delete_csfc(const struct ble_store_key_csfc *key)
{
    return BLE_HS_ENOTSUP;
}

void
ble_store_key_from_value_csfc(struct ble_store_key_csfc *key,
                              const struct ble_store_value_csfc *val)
{
    /* No-op */
}

/* Stub for ble_store_config_init */
void
ble_store_config_init(void)
{
    /* No-op - store is disabled */
}

/* ble_store_util stubs */
int
ble_store_util_bonded_peers(ble_addr_t *out_peer_id_addrs, int *out_num_peers,
                            int max_peers)
{
    if (out_num_peers != NULL) {
        *out_num_peers = 0;
    }
    return 0;
}

int
ble_store_util_delete_all(int type, const union ble_store_key *key)
{
    return 0;
}

int
ble_store_util_delete_peer(const ble_addr_t *peer_id_addr)
{
    return 0;
}

int
ble_store_util_delete_oldest_peer(void)
{
    return BLE_HS_ENOENT;
}

int
ble_store_util_count(int type, int *out_count)
{
    if (out_count != NULL) {
        *out_count = 0;
    }
    return 0;
}

int
ble_store_util_status_rr(struct ble_store_status_event *event, void *arg)
{
    return 0;
}

int
ble_store_util_wipe(void)
{
    return 0;
}

#endif /* MYNEWT_VAL(BLE_STORE_CONFIG_DISABLED) */