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
 * Stub implementation of BLE host-based privacy resolution functions
 * for observer-only builds where address resolution is not needed.
 */

#include "syscfg/syscfg.h"

#if !MYNEWT_VAL(BLE_HOST_BASED_PRIVACY) && MYNEWT_VAL(BLE_HOST_PRIVACY_DISABLED)

#include <string.h>
#include "ble_hs_priv.h"
#include "ble_hs_resolv_priv.h"

/* Stub implementations for resolution functions */
int
ble_hs_resolv_list_add(const uint8_t *cmds)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_resolv_list_rmv(uint8_t addr_type, const uint8_t *ident_addr)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_resolv_list_clr(void)
{
    return 0;
}

int
ble_hs_resolv_list_size(const uint8_t *cmds)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_resolv_peer_addr_rd(uint8_t peer_identity_addr_type,
                           const uint8_t *peer_identity_addr,
                           const uint8_t *cmds)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_resolv_local_addr_rd(uint8_t peer_identity_addr_type,
                            const uint8_t *peer_identity_addr,
                            const uint8_t *cmds)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_resolv_enable(bool enable)
{
    return 0;
}

int
ble_hs_resolv_set_rpa_tmo(uint16_t tmo_secs)
{
    return 0;
}

void
ble_hs_resolv_set_priv_mode(const ble_addr_t *addr, uint8_t mode)
{
    /* No-op */
}

int
ble_hs_resolv_init(void)
{
    return 0;
}

struct ble_hs_resolv_entry *
ble_hs_resolv_list_find(const uint8_t *ident_addr)
{
    return NULL;
}

int
ble_hs_resolv_rpa(const uint8_t *rpa, const uint8_t *irk, bool *is_resolved)
{
    if (is_resolved != NULL) {
        *is_resolved = false;
    }
    return BLE_HS_ENOTSUP;
}

void
ble_rpa_replace_peer_params_with_rl(uint8_t rl_idx, uint8_t *peer_addr,
                                    uint8_t *peer_addr_type)
{
    /* No-op - no resolution list in observer-only mode */
}

#endif /* !MYNEWT_VAL(BLE_HOST_BASED_PRIVACY) && MYNEWT_VAL(BLE_HOST_PRIVACY_DISABLED) */