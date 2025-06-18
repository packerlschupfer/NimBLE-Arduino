/*
 * Stub implementation for ble_sm_alg_encrypt when security is disabled
 * This allows observer-only builds without crypto dependencies
 */

#include "nimble/porting/nimble/include/syscfg/syscfg.h"

#if !MYNEWT_VAL(BLE_SM) && !MYNEWT_VAL(BLE_CRYPTO)

#include <stdint.h>

/* Stub function - always returns error since encryption is not available */
int ble_sm_alg_encrypt(const uint8_t *key, const uint8_t *plaintext,
                       uint8_t *enc_data)
{
    /* Return error - encryption not available in observer-only mode */
    return -1;
}

#endif /* !MYNEWT_VAL(BLE_SM) && !MYNEWT_VAL(BLE_CRYPTO) */