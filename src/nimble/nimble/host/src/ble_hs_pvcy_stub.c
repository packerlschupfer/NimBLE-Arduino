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
 * Stub implementation of BLE privacy functions for observer-only builds
 * where privacy features are not needed.
 */

#include "syscfg/syscfg.h"

#if MYNEWT_VAL(BLE_HOST_PRIVACY_DISABLED)

#include <string.h>
#include "ble_hs_priv.h"
#include "nimble/nimble/host/include/host/ble_hs_pvcy.h"

/* Stub implementations for privacy functions */
int
ble_hs_pvcy_set_resolve_enabled(int enable)
{
    return 0;
}

int
ble_hs_pvcy_remove_entry(uint8_t addr_type, const uint8_t *addr)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_pvcy_add_entry(const uint8_t *addr, uint8_t addr_type,
                      const uint8_t *irk)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_pvcy_ensure_started(void)
{
    return 0;
}

bool
ble_hs_pvcy_enabled(void)
{
    return false;
}

int
ble_hs_pvcy_set_our_irk(const uint8_t *irk)
{
    return BLE_HS_ENOTSUP;
}

int
ble_hs_pvcy_our_irk(const uint8_t **out_irk)
{
    if (out_irk != NULL) {
        *out_irk = NULL;
    }
    return BLE_HS_ENOTSUP;
}

int
ble_hs_pvcy_set_mode(const ble_addr_t *addr, uint8_t priv_mode)
{
    return BLE_HS_ENOTSUP;
}

bool
ble_host_rpa_enabled(void)
{
    return false;
}

int
ble_hs_pvcy_clear_entries(void)
{
    return 0;
}

int
ble_hs_pvcy_read_entries(ble_hs_pvcy_read_entries_fn *cb, void *cb_arg)
{
    return 0;
}

/* Provide basic RPA checking for observer mode */
bool
ble_hs_is_rpa(const uint8_t *addr, uint8_t addr_type)
{
    if (addr == NULL || addr_type != BLE_ADDR_RANDOM) {
        return false;
    }
    /* Check if upper 2 bits are 01 (RPA identifier) */
    return (addr[5] & 0xc0) == 0x40;
}

bool
ble_addr_is_rpa(const ble_addr_t *addr)
{
    return addr != NULL && ble_hs_is_rpa(addr->val, addr->type);
}

#endif /* MYNEWT_VAL(BLE_HOST_PRIVACY_DISABLED) */