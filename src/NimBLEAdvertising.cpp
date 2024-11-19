/*
 * NimBLEAdvertising.cpp
 *
 *  Created: on March 3, 2020
 *      Author H2zero
 *
 * Originally:
 *
 * BLEAdvertising.cpp
 *
 * This class encapsulates advertising a BLE Server.
 *  Created on: Jun 21, 2017
 *      Author: kolban
 *
 */
#include "nimconfig.h"
#if (defined(CONFIG_BT_ENABLED) && defined(CONFIG_BT_NIMBLE_ROLE_BROADCASTER) && !CONFIG_BT_NIMBLE_EXT_ADV) || \
    defined(_DOXYGEN_)

# if defined(CONFIG_NIMBLE_CPP_IDF)
#  include "services/gap/ble_svc_gap.h"
# else
#  include "nimble/nimble/host/services/gap/include/services/gap/ble_svc_gap.h"
# endif
# include "NimBLEAdvertising.h"
# include "NimBLEDevice.h"
# include "NimBLEServer.h"
# include "NimBLEUtils.h"
# include "NimBLELog.h"

static const char* LOG_TAG = "NimBLEAdvertising";

/**
 * @brief Construct a default advertising object.
 */
NimBLEAdvertising::NimBLEAdvertising()
    : m_advData{},
      m_scanData{},
      m_advParams{},
      m_serviceUUIDs{},
      m_customAdvData{false},
      m_customScanResponseData{false},
      m_scanResp{true},
      m_advDataSet{false},
      m_advCompCb{nullptr},
      m_slaveItvl{0},
      m_duration{BLE_HS_FOREVER},
      m_svcData16{},
      m_svcData32{},
      m_svcData128{},
      m_name{},
      m_mfgData{},
      m_uri{} {
# if !defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)
    m_advParams.conn_mode = BLE_GAP_CONN_MODE_NON;
# else
    m_advParams.conn_mode = BLE_GAP_CONN_MODE_UND;
    m_advData.flags       = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
# endif
    m_advParams.disc_mode = BLE_GAP_DISC_MODE_GEN;
} // NimBLEAdvertising

/**
 * @brief Stops the current advertising and resets the advertising data to the default values.
 * @return True if successful.
 */
bool NimBLEAdvertising::reset() {
    if (!stop()) {
        return false;
    }

    *this = NimBLEAdvertising();
    return true;
} // reset

/**
 * @brief Add a service uuid to exposed list of services.
 * @param [in] serviceUUID The UUID of the service to expose.
 */
void NimBLEAdvertising::addServiceUUID(const NimBLEUUID& serviceUUID) {
    m_serviceUUIDs.push_back(serviceUUID);
    m_advDataSet = false;
} // addServiceUUID

/**
 * @brief Add a service uuid to exposed list of services.
 * @param [in] serviceUUID The string representation of the service to expose.
 */
void NimBLEAdvertising::addServiceUUID(const char* serviceUUID) {
    addServiceUUID(NimBLEUUID(serviceUUID));
    m_advDataSet = false;
} // addServiceUUID

/**
 * @brief Remove a service UUID from the advertisement.
 * @param [in] serviceUUID The UUID of the service to remove.
 */
void NimBLEAdvertising::removeServiceUUID(const NimBLEUUID& serviceUUID) {
    std::vector<NimBLEUUID> sVec;
    for (const auto& uuid : m_serviceUUIDs) {
        if (uuid == serviceUUID) {
            continue;
        }
        sVec.push_back(uuid);
    }

    sVec.swap(m_serviceUUIDs);
    m_advDataSet = false;
} // removeServiceUUID

/**
 * @brief Remove all service UUIDs from the advertisement.
 */
void NimBLEAdvertising::removeServices() {
    std::vector<NimBLEUUID>().swap(m_serviceUUIDs);
    m_advDataSet = false;
} // removeServices

/**
 * @brief Set the device appearance in the advertising data.
 * @param [in] appearance The appearance of the device in the advertising data.
 * If the appearance value is 0 then the appearance will not be in the advertisement.
 */
void NimBLEAdvertising::setAppearance(uint16_t appearance) {
    m_advData.appearance            = appearance;
    m_advData.appearance_is_present = appearance > 0;
    m_advDataSet                    = false;
} // setAppearance

/**
 * @brief Add the transmission power level to the advertisement packet.
 */
void NimBLEAdvertising::addTxPower() {
    m_advData.tx_pwr_lvl_is_present = true;
    m_advData.tx_pwr_lvl            = NimBLEDevice::getPower();
    m_advDataSet                    = false;
} // addTxPower

/**
 * @brief Set the advertised name of the device.
 * @param [in] name The name to advertise.
 */
void NimBLEAdvertising::setName(const std::string& name) {
    std::vector<uint8_t>(name.begin(), name.end()).swap(m_name);
    m_advData.name             = &m_name[0];
    m_advData.name_len         = m_name.size();
    m_advData.name_is_complete = true;
    m_advDataSet               = false;
} // setName

/**
 * @brief Set the advertised manufacturer data.
 * @param [in] data The data to advertise.
 * @param [in] length The length of the data.
 */
void NimBLEAdvertising::setManufacturerData(const uint8_t* data, size_t length) {
    std::vector<uint8_t>(data, data + length).swap(m_mfgData);
    m_advData.mfg_data     = &m_mfgData[0];
    m_advData.mfg_data_len = m_mfgData.size();
    m_advDataSet           = false;
} // setManufacturerData

/**
 * @brief Set the advertised manufacturer data.
 * @param [in] data The data to advertise.
 */
void NimBLEAdvertising::setManufacturerData(const std::string& data) {
    setManufacturerData(reinterpret_cast<const uint8_t*>(data.data()), data.length());
} // setManufacturerData

/**
 * @brief Set the advertised manufacturer data.
 * @param [in] data The data to advertise.
 */
void NimBLEAdvertising::setManufacturerData(const std::vector<uint8_t>& data) {
    setManufacturerData(&data[0], data.size());
} // setManufacturerData

/**
 * @brief Set the advertised URI.
 * @param [in] uri The URI to advertise.
 */
void NimBLEAdvertising::setURI(const std::string& uri) {
    std::vector<uint8_t>(uri.begin(), uri.end()).swap(m_uri);
    m_advData.uri     = &m_uri[0];
    m_advData.uri_len = m_uri.size();
    m_advDataSet      = false;
} // setURI

/**
 * @brief Set the service data advertised for the UUID.
 * @param [in] uuid The UUID the service data belongs to.
 * @param [in] data The data to advertise.
 * @param [in] length The length of the data.
 * @note If data length is 0 the service data will not be advertised.
 */
void NimBLEAdvertising::setServiceData(const NimBLEUUID& uuid, const uint8_t* data, size_t length) {
    switch (uuid.bitSize()) {
        case 16: {
            std::vector<uint8_t>(uuid.getValue(), uuid.getValue() + 2).swap(m_svcData16);
            m_svcData16.insert(m_svcData16.end(), data, data + length);
            m_advData.svc_data_uuid16     = &m_svcData16[0];
            m_advData.svc_data_uuid16_len = (length > 0) ? m_svcData16.size() : 0;
            break;
        }

        case 32: {
            std::vector<uint8_t>(uuid.getValue(), uuid.getValue() + 4).swap(m_svcData32);
            m_svcData32.insert(m_svcData32.end(), data, data + length);
            m_advData.svc_data_uuid32     = &m_svcData32[0];
            m_advData.svc_data_uuid32_len = (length > 0) ? m_svcData32.size() : 0;
            break;
        }

        case 128: {
            std::vector<uint8_t>(uuid.getValue(), uuid.getValue() + 16).swap(m_svcData128);
            m_svcData128.insert(m_svcData128.end(), data, data + length);
            m_advData.svc_data_uuid128     = &m_svcData128[0];
            m_advData.svc_data_uuid128_len = (length > 0) ? m_svcData128.size() : 0;
            break;
        }

        default:
            return;
    }

    m_advDataSet = false;
} // setServiceData

/**
 * @brief Set the service data advertised for the UUID.
 * @param [in] uuid The UUID the service data belongs to.
 * @param [in] data The data to advertise.
 * @note If data length is 0 the service data will not be advertised.
 */
void NimBLEAdvertising::setServiceData(const NimBLEUUID& uuid, const std::vector<uint8_t>& data) {
    setServiceData(uuid, data.data(), data.size());
} // setServiceData

/**
 * @brief Set the service data advertised for the UUID.
 * @param [in] uuid The UUID the service data belongs to.
 * @param [in] data The data to advertise.
 * @note If data length is 0 the service data will not be advertised.
 */
void NimBLEAdvertising::setServiceData(const NimBLEUUID& uuid, const std::string& data) {
    setServiceData(uuid, reinterpret_cast<const uint8_t*>(data.data()), data.length());
} // setServiceData

/**
 * @brief Set the type of connectable mode to advertise.
 * @param [in] mode The connectable mode:
 * * BLE_GAP_CONN_MODE_NON    (0) - not connectable advertising
 * * BLE_GAP_CONN_MODE_DIR    (1) - directed connectable advertising
 * * BLE_GAP_CONN_MODE_UND    (2) - undirected connectable advertising
 */
void NimBLEAdvertising::setConnectableMode(uint8_t mode) {
    if (mode > BLE_GAP_CONN_MODE_UND) {
        NIMBLE_LOGE(LOG_TAG, "Invalid connectable mode: %u", mode);
        return;
    }

    m_advParams.conn_mode = mode;
} // setAdvertisementType

/**
 * @brief Set the discoverable mode to use.
 * @param [in] mode The discoverable mode:
 * * BLE_GAP_DISC_MODE_NON    (0) - non-discoverable
 * * BLE_GAP_DISC_MODE_LTD    (1) - limited discoverable
 * * BLE_GAP_DISC_MODE_GEN    (2) - general discoverable
 */
void NimBLEAdvertising::setDiscoverableMode(uint8_t mode) {
    switch (mode) {
        case BLE_GAP_DISC_MODE_NON:
            m_advData.flags = 0;
            break;
        case BLE_GAP_DISC_MODE_LTD:
            m_advData.flags = (BLE_HS_ADV_F_DISC_LTD | BLE_HS_ADV_F_BREDR_UNSUP);
            break;
        case BLE_GAP_DISC_MODE_GEN:
            m_advData.flags = (BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP);
            break;
        default:
            NIMBLE_LOGE(LOG_TAG, "Invalid discoverable mode: %u", mode);
            return;
    }

    m_advParams.disc_mode = mode;
} // setDiscoverableMode

/**
 * @brief Set the advertising interval.
 * @param [in] interval The advertising interval in 0.625ms units, 0 = use default.
 */
void NimBLEAdvertising::setAdvertisingInterval(uint16_t interval) {
    m_advParams.itvl_min = interval;
    m_advParams.itvl_max = interval;
} // setAdvertisingInterval

/**
 * @brief Set the minimum advertising interval.
 * @param [in] minInterval Minimum value for advertising interval in 0.625ms units, 0 = use default.
 */
void NimBLEAdvertising::setMinInterval(uint16_t minInterval) {
    m_advParams.itvl_min = minInterval;
} // setMinInterval

/**
 * @brief Set the maximum advertising interval.
 * @param [in] maxInterval Maximum value for advertising interval in 0.625ms units, 0 = use default.
 */
void NimBLEAdvertising::setMaxInterval(uint16_t maxInterval) {
    m_advParams.itvl_max = maxInterval;
} // setMaxInterval

/**
 * @brief Set the preferred min and max connection intervals to advertise.
 * @param [in] minInterval The minimum preferred connection interval.
 * @param [in] maxInterval The Maximum preferred connection interval.
 * @details  Range = 0x0006(7.5ms) to 0x0C80(4000ms), values not within the range will be limited to this range.
 */
void NimBLEAdvertising::setPreferredConnectionInterval(uint16_t minInterval, uint16_t maxInterval) {
    if (!minInterval) {
        minInterval = *reinterpret_cast<const uint16_t*>(m_advData.slave_itvl_range);
    }

    if (!maxInterval) {
        maxInterval = *reinterpret_cast<const uint16_t*>(m_advData.slave_itvl_range + 2);
    }

    minInterval = std::max<uint16_t>(minInterval, 0x6);
    minInterval = std::min<uint16_t>(minInterval, 0xC80);
    maxInterval = std::max<uint16_t>(maxInterval, 0x6);
    maxInterval = std::min<uint16_t>(maxInterval, 0xC80);
    maxInterval = std::max<uint16_t>(maxInterval, minInterval); // Max must be greater than or equal to min.

    m_slaveItvl[0] = minInterval;
    m_slaveItvl[1] = minInterval >> 8;
    m_slaveItvl[2] = maxInterval;
    m_slaveItvl[3] = maxInterval >> 8;

    m_advDataSet = false;
} // setPreferredConnectionInterval

/**
 * @brief Set the advertised min connection interval preferred by this device.
 * @param [in] minInterval the max interval value.
 * @details  Range = 0x0006 to 0x0C80, values not within the range will be limited to this range.
 */
void NimBLEAdvertising::setMinPreferred(uint16_t minInterval) {
    setPreferredConnectionInterval(minInterval, 0);
} // setMinPreferred

/**
 * @brief Set the advertised max connection interval preferred by this device.
 * @param [in] maxInterval the max interval value. Range = 0x0006 to 0x0C80.
 * @details Range = 0x0006 to 0x0C80, values not within the range will be limited to this range.
 */
void NimBLEAdvertising::setMaxPreferred(uint16_t maxInterval) {
    setPreferredConnectionInterval(0, maxInterval);
} // setMaxPreferred

/**
 * @brief Set if scan response is available.
 * @param [in] enable true if scan response should be available.
 */
void NimBLEAdvertising::setScanResponse(bool enable) {
    m_scanResp   = enable;
    m_advDataSet = false;
} // setScanResponse

/**
 * @brief Set the filtering for the scan filter.
 * @param [in] scanRequestWhitelistOnly If true, only allow scan requests from those on the white list.
 * @param [in] connectWhitelistOnly If true, only allow connections from those on the white list.
 */
void NimBLEAdvertising::setScanFilter(bool scanRequestWhitelistOnly, bool connectWhitelistOnly) {
    if (!scanRequestWhitelistOnly && !connectWhitelistOnly) {
        m_advParams.filter_policy = BLE_HCI_ADV_FILT_NONE;
        return;
    }
    if (scanRequestWhitelistOnly && !connectWhitelistOnly) {
        m_advParams.filter_policy = BLE_HCI_ADV_FILT_SCAN;
        return;
    }
    if (!scanRequestWhitelistOnly && connectWhitelistOnly) {
        m_advParams.filter_policy = BLE_HCI_ADV_FILT_CONN;
        return;
    }
    if (scanRequestWhitelistOnly && connectWhitelistOnly) {
        m_advParams.filter_policy = BLE_HCI_ADV_FILT_BOTH;
        return;
    }
} // setScanFilter

/**
 * @brief Set the advertisement data that is to be broadcast in a regular advertisement.
 * @param [in] data The data to be broadcast.
 * @details The use of this function will replace any data set with addServiceUUID\n
 * or setAppearance. If you wish for these to be advertised you must include them\n
 * in the advertisementData parameter sent.
 */
void NimBLEAdvertising::setAdvertisementData(const NimBLEAdvertisementData& data) {
    int rc = ble_gap_adv_set_data(&data.getPayload()[0], data.getPayload().size());
    if (rc != 0) {
        NIMBLE_LOGE(LOG_TAG, "ble_gap_adv_set_data: %d %s", rc, NimBLEUtils::returnCodeToString(rc));
    }

    m_customAdvData = true; // Set the flag that indicates we are using custom advertising data.
} // setAdvertisementData

/**
 * @brief Set the data that is to be provided in a scan response.
 * @param [in] data The data to be provided in the scan response
 * @details Calling this without also using setAdvertisementData will have no effect.\n
 * When using custom scan response data you must also use custom advertisement data.
 */
void NimBLEAdvertising::setScanResponseData(const NimBLEAdvertisementData& data) {
    int rc = ble_gap_adv_rsp_set_data(&data.getPayload()[0], data.getPayload().size());
    if (rc != 0) {
        NIMBLE_LOGE(LOG_TAG, "ble_gap_adv_rsp_set_data: %d %s", rc, NimBLEUtils::returnCodeToString(rc));
    }

    m_customScanResponseData = true; // Set the flag that indicates we are using custom scan response data.
} // setScanResponseData

/**
 * @brief Start advertising.
 * @param [in] duration The duration, in milliseconds, to advertise, 0 == advertise forever.
 * @param [in] dirAddr The address of a peer to directly advertise to.
 * @return True if advertising started successfully.
 */
bool NimBLEAdvertising::start(uint32_t duration, const NimBLEAddress* dirAddr) {
    NIMBLE_LOGD(LOG_TAG,
                ">> Advertising start: customAdvData: %d, customScanResponseData: %d",
                m_customAdvData,
                m_customScanResponseData);

    if (!NimBLEDevice::m_synced) {
        NIMBLE_LOGE(LOG_TAG, "Host not synced!");
        return false;
    }

    if (ble_gap_adv_active()) {
        NIMBLE_LOGW(LOG_TAG, "Advertising already active");
        return true;
    }

# if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)
    NimBLEServer* pServer = NimBLEDevice::getServer();
    if (pServer != nullptr) {
        if (!pServer->m_gattsStarted) {
            pServer->start();
        } else if (pServer->getConnectedCount() >= NIMBLE_MAX_CONNECTIONS) {
            NIMBLE_LOGE(LOG_TAG, "Unable to advertise; Max connections reached");
            return false;
        }
    }
# endif

    int rc     = 0;
    // Save the duration incase of host reset so we can restart with the same params
    m_duration = duration;
    if (duration == 0) {
        duration = BLE_HS_FOREVER;
    }

    if (m_advParams.conn_mode == BLE_GAP_CONN_MODE_NON) {
        m_advData.flags = 0; // non-connectable advertising does not require AD flags.
    }

    if (!m_customAdvData && !m_advDataSet) {
        // start with 3 bytes for the flags data if required
        uint8_t payloadLen = (m_advData.flags > 0) ? (2 + 1) : 0;
        if (m_advData.mfg_data_len > 0) {
            payloadLen += (2 + m_advData.mfg_data_len);
        }

        if (m_advData.svc_data_uuid16_len > 0) {
            payloadLen += (2 + m_advData.svc_data_uuid16_len);
        }

        if (m_advData.svc_data_uuid32_len > 0) {
            payloadLen += (2 + m_advData.svc_data_uuid32_len);
        }

        if (m_advData.svc_data_uuid128_len > 0) {
            payloadLen += (2 + m_advData.svc_data_uuid128_len);
        }

        if (m_advData.uri_len > 0) {
            payloadLen += (2 + m_advData.uri_len);
        }

        if (m_advData.appearance_is_present) {
            payloadLen += (2 + BLE_HS_ADV_APPEARANCE_LEN);
        }

        if (m_advData.tx_pwr_lvl_is_present) {
            payloadLen += (2 + BLE_HS_ADV_TX_PWR_LVL_LEN);
        }

        if (m_advData.slave_itvl_range != nullptr) {
            payloadLen += (2 + BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN);
        }

        for (const auto& uuid : m_serviceUUIDs) {
            int payloadAdd = 0;
            if (uuid.bitSize() == BLE_UUID_TYPE_16) {
                payloadAdd = (m_advData.num_uuids16 > 0) ? 2 : 4; // 2 bytes more if this is the first of the type
                if ((payloadLen + payloadAdd) > BLE_HS_ADV_MAX_SZ) {
                    m_advData.uuids16_is_complete = 0;
                    continue;
                }

                m_advData.uuids16 = reinterpret_cast<ble_uuid16_t*>(
                    realloc((void*)m_advData.uuids16, (m_advData.num_uuids16 + 1) * sizeof(ble_uuid16_t)));
                if (!m_advData.uuids16) {
                    NIMBLE_LOGE(LOG_TAG, "Error, no mem");
                    return false;
                }

                memcpy((void*)&m_advData.uuids16[m_advData.num_uuids16], uuid.getBase(), sizeof(ble_uuid16_t));
                m_advData.uuids16_is_complete = 1;
                m_advData.num_uuids16++;
            } else if (uuid.bitSize() == BLE_UUID_TYPE_32) {
                payloadAdd = (m_advData.num_uuids32 > 0) ? 4 : 6; // 2 bytes more if this is the first of the type
                if ((payloadLen + payloadAdd) > BLE_HS_ADV_MAX_SZ) {
                    m_advData.uuids32_is_complete = 0;
                    continue;
                }

                m_advData.uuids32 = reinterpret_cast<ble_uuid32_t*>(
                    realloc((void*)m_advData.uuids32, (m_advData.num_uuids32 + 1) * sizeof(ble_uuid32_t)));
                if (!m_advData.uuids32) {
                    NIMBLE_LOGE(LOG_TAG, "Error, no mem");
                    return false;
                }

                memcpy((void*)&m_advData.uuids32[m_advData.num_uuids32], uuid.getBase(), sizeof(ble_uuid32_t));
                m_advData.uuids32_is_complete = 1;
                m_advData.num_uuids32++;
            } else if (uuid.bitSize() == BLE_UUID_TYPE_128) {
                payloadAdd = (m_advData.num_uuids128 > 0) ? 16 : 18; // 2 bytes more if this is the first of the type
                if ((payloadLen + payloadAdd) > BLE_HS_ADV_MAX_SZ) {
                    m_advData.uuids128_is_complete = 0;
                    continue;
                }

                m_advData.uuids128 = reinterpret_cast<ble_uuid128_t*>(
                    realloc((void*)m_advData.uuids128, (m_advData.num_uuids128 + 1) * sizeof(ble_uuid128_t)));
                if (!m_advData.uuids128) {
                    NIMBLE_LOGE(LOG_TAG, "Error, no mem");
                    return false;
                }

                memcpy((void*)&m_advData.uuids128[m_advData.num_uuids128], uuid.getBase(), sizeof(ble_uuid128_t));
                m_advData.uuids128_is_complete = 1;
                m_advData.num_uuids128++;
            }

            payloadLen += payloadAdd;
        }

        // check if there is room for the name, if not put it in scan data
        if (m_advData.name_len && (payloadLen + 2 + m_advData.name_len) > BLE_HS_ADV_MAX_SZ) {
            if (m_scanResp && !m_customScanResponseData) {
                m_scanData.name     = m_advData.name;
                m_scanData.name_len = m_advData.name_len;
                if (m_scanData.name_len > BLE_HS_ADV_MAX_SZ - 2) {
                    m_scanData.name_len         = BLE_HS_ADV_MAX_SZ - 2;
                    m_scanData.name_is_complete = 0;
                } else {
                    m_scanData.name_is_complete = 1;
                }
                m_advData.name             = nullptr;
                m_advData.name_len         = 0;
                m_advData.name_is_complete = 0;
            } else {
                // if not using scan response truncate the name
                if (m_advData.name_len > (BLE_HS_ADV_MAX_SZ - payloadLen - 2)) {
                    m_advData.name_len         = (BLE_HS_ADV_MAX_SZ - payloadLen - 2);
                    m_advData.name_is_complete = 0;
                }
            }
        }

        rc = ble_gap_adv_set_fields(&m_advData);
        if (rc != 0) {
            NIMBLE_LOGE(LOG_TAG, "Error setting advertisement data rc=%d, %s", rc, NimBLEUtils::returnCodeToString(rc));
        }

        if (m_scanResp && !m_customScanResponseData) {
            rc = ble_gap_adv_rsp_set_fields(&m_scanData);
            if (rc != 0) {
                NIMBLE_LOGE(LOG_TAG, "Error setting scan response data rc=%d, %s", rc, NimBLEUtils::returnCodeToString(rc));
            }
        }

        // free the memory used for the UUIDs as they are now copied into the advertisement data.
        if (m_advData.num_uuids128 > 0) {
            free((void*)m_advData.uuids128);
            m_advData.uuids128     = nullptr;
            m_advData.num_uuids128 = 0;
        }

        if (m_advData.num_uuids32 > 0) {
            free((void*)m_advData.uuids32);
            m_advData.uuids32     = nullptr;
            m_advData.num_uuids32 = 0;
        }

        if (m_advData.num_uuids16 > 0) {
            free((void*)m_advData.uuids16);
            m_advData.uuids16     = nullptr;
            m_advData.num_uuids16 = 0;
        }

        if (rc != 0) {
            return false;
        }

        m_advDataSet = true;
    }

# if defined(CONFIG_BT_NIMBLE_ROLE_PERIPHERAL)
    rc = ble_gap_adv_start(NimBLEDevice::m_ownAddrType,
                           (dirAddr != nullptr) ? dirAddr->getBase() : NULL,
                           duration,
                           &m_advParams,
                           (pServer != nullptr) ? NimBLEServer::handleGapEvent : NimBLEAdvertising::handleGapEvent,
                           this);
# else
    rc = ble_gap_adv_start(NimBLEDevice::m_ownAddrType, NULL, duration, &m_advParams, NimBLEAdvertising::handleGapEvent, this);
# endif
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        NIMBLE_LOGE(LOG_TAG, "Error enabling advertising; rc=%d, %s", rc, NimBLEUtils::returnCodeToString(rc));
        return false;
    }

    NIMBLE_LOGD(LOG_TAG, "<< Advertising start");
    return true;
} // start

/**
 * @brief Stop advertising.
 * @return True if advertising stopped successfully.
 */
bool NimBLEAdvertising::stop() {
    int rc = ble_gap_adv_stop();
    if (rc != 0 && rc != BLE_HS_EALREADY) {
        NIMBLE_LOGE(LOG_TAG, "ble_gap_adv_stop rc=%d %s", rc, NimBLEUtils::returnCodeToString(rc));
        return false;
    }

    return true;
} // stop

/**
 * @brief Set the callback to be invoked when advertising stops.
 * @param [in] callback The callback to be invoked when advertising stops.
 */
void NimBLEAdvertising::setAdvertisingCompleteCallback(advCompleteCB_t callback) {
    m_advCompCb = callback;
} // setAdvertisingCompleteCallback

/**
 * @brief Check if currently advertising.
 * @return True if advertising is active.
 */
bool NimBLEAdvertising::isAdvertising() {
    return ble_gap_adv_active();
} // isAdvertising

/*
 * Host reset seems to clear advertising data,
 * we need clear the flag so it reloads it.
 */
void NimBLEAdvertising::onHostSync() {
    m_advDataSet = false;
    // If we were advertising forever, restart it now
    if (m_duration == 0) {
        start(m_duration);
    } else {
        // Otherwise we should tell the app that advertising stopped.
        if (m_advCompCb != nullptr) {
            m_advCompCb(this);
        }
    }
} // onHostSync

/**
 * @brief Handler for gap events when not using peripheral role.
 * @param [in] event the event data.
 * @param [in] arg pointer to the advertising instance.
 */
int NimBLEAdvertising::handleGapEvent(struct ble_gap_event* event, void* arg) {
    NimBLEAdvertising* pAdv = (NimBLEAdvertising*)arg;

    if (event->type == BLE_GAP_EVENT_ADV_COMPLETE) {
        switch (event->adv_complete.reason) {
            // Don't call the callback if host reset, we want to
            // preserve the active flag until re-sync to restart advertising.
            case BLE_HS_ETIMEOUT_HCI:
            case BLE_HS_EOS:
            case BLE_HS_ECONTROLLER:
            case BLE_HS_ENOTSYNCED:
                NIMBLE_LOGE(LOG_TAG, "host reset, rc=%d", event->adv_complete.reason);
                NimBLEDevice::onReset(event->adv_complete.reason);
                return 0;
            default:
                break;
        }

        if (pAdv->m_advCompCb != nullptr) {
            pAdv->m_advCompCb(pAdv);
        }
    }
    return 0;
} // handleGapEvent

/* -------------------------------------------------------------------------- */
/*                             Advertisement Data                             */
/* -------------------------------------------------------------------------- */

/**
 * @brief Add data to the payload to be advertised.
 * @param [in] data The data to be added to the payload.
 * @param [in] length The size of data to be added to the payload.
 */
void NimBLEAdvertisementData::addData(const uint8_t* data, size_t length) {
    if ((m_payload.size() + length) > BLE_HS_ADV_MAX_SZ) {
        NIMBLE_LOGE(LOG_TAG, "Advertisement data length exceeded");
        return;
    }

    m_payload.insert(m_payload.end(), data, data + length);
} // addData

/**
 * @brief Add data to the payload to be advertised.
 * @param [in] data The data to be added to the payload.
 */
void NimBLEAdvertisementData::addData(const std::vector<uint8_t>& data) {
    addData(&data[0], data.size());
} // addData

/**
 * @brief Set the appearance.
 * @param [in] appearance The appearance code value.
 */
void NimBLEAdvertisementData::setAppearance(uint16_t appearance) {
    uint8_t data[4];
    data[0] = 3;
    data[1] = BLE_HS_ADV_TYPE_APPEARANCE;
    data[2] = appearance;
    data[3] = (appearance >> 8) & 0xFF;
    addData(data, 4);
} // setAppearance

/**
 * @brief Set the advertisement flags.
 * @param [in] flag The flags to be set in the advertisement.
 * * BLE_HS_ADV_F_DISC_LTD
 * * BLE_HS_ADV_F_DISC_GEN
 * * BLE_HS_ADV_F_BREDR_UNSUP - must always use with NimBLE
 */
void NimBLEAdvertisementData::setFlags(uint8_t flag) {
    uint8_t data[3];
    data[0] = 2;
    data[1] = BLE_HS_ADV_TYPE_FLAGS;
    data[2] = flag | BLE_HS_ADV_F_BREDR_UNSUP;
    addData(data, 3);
} // setFlag

/**
 * @brief Set manufacturer specific data.
 * @param [in] data The manufacturer data to advertise.
 */
void NimBLEAdvertisementData::setManufacturerData(const std::string& data) {
    uint8_t mdata[31];
    uint8_t length = 2 + std::min<uint8_t>(data.length(), 29);
    mdata[0]       = length - 1;
    mdata[1]       = BLE_HS_ADV_TYPE_MFG_DATA;
    memcpy(&mdata[2], data.c_str(), std::min<uint8_t>(data.length(), 29));
    addData(mdata, length);
} // setManufacturerData

/**
 * @brief Set manufacturer specific data.
 * @param [in] data The manufacturer data to advertise.
 */
void NimBLEAdvertisementData::setManufacturerData(const std::vector<uint8_t>& data) {
    uint8_t mdata[31];
    uint8_t length = 2 + std::min<uint8_t>(data.size(), 29);
    mdata[0]       = length - 1;
    mdata[1]       = BLE_HS_ADV_TYPE_MFG_DATA;
    memcpy(&mdata[2], data.data(), std::min<uint8_t>(data.size(), 29));
    addData(mdata, length);
} // setManufacturerData

/**
 * @brief Set the URI to advertise.
 * @param [in] uri The uri to advertise.
 */
void NimBLEAdvertisementData::setURI(const std::string& uri) {
    uint8_t data[31];
    uint8_t length = 2 + std::min<uint8_t>(uri.length(), 29);
    data[0]        = length - 1;
    data[1]        = BLE_HS_ADV_TYPE_URI;
    memcpy(&data[2], uri.c_str(), std::min<uint8_t>(uri.length(), 29));
    addData(data, length);
} // setURI

/**
 * @brief Set the complete name of this device.
 * @param [in] name The name to advertise.
 * @param [in] isComplete If true the name is complete, otherwise it is shortened.
 */
void NimBLEAdvertisementData::setName(const std::string& name, bool isComplete) {
    uint8_t data[31];
    uint8_t length = 2 + std::min<uint8_t>(name.length(), 29);
    data[0]        = length - 1;
    data[1]        = isComplete ? BLE_HS_ADV_TYPE_COMP_NAME : BLE_HS_ADV_TYPE_INCOMP_NAME;
    memcpy(&data[2], name.c_str(), std::min<uint8_t>(name.length(), 29));
    addData(data, length);
} // setName

/**
 * @brief Set the short name.
 * @param [in] name The short name of the device.
 */
void NimBLEAdvertisementData::setShortName(const std::string& name) {
    setName(name, false);
} // setShortName

/**
 * @brief Set a single service to advertise as a complete list of services.
 * @param [in] uuid The service to advertise.
 */
void NimBLEAdvertisementData::setCompleteServices(const NimBLEUUID& uuid) {
    setServices(true, uuid.bitSize(), {uuid});
} // setCompleteServices

/**
 * @brief Set the complete list of 16 bit services to advertise.
 * @param [in] uuids A vector of 16 bit UUID's to advertise.
 */
void NimBLEAdvertisementData::setCompleteServices16(const std::vector<NimBLEUUID>& uuids) {
    setServices(true, 16, uuids);
} // setCompleteServices16

/**
 * @brief Set the complete list of 32 bit services to advertise.
 * @param [in] uuids A vector of 32 bit UUID's to advertise.
 */
void NimBLEAdvertisementData::setCompleteServices32(const std::vector<NimBLEUUID>& uuids) {
    setServices(true, 32, uuids);
} // setCompleteServices32

/**
 * @brief Set a single service to advertise as a partial list of services.
 * @param [in] uuid The service to advertise.
 */
void NimBLEAdvertisementData::setPartialServices(const NimBLEUUID& uuid) {
    setServices(false, uuid.bitSize(), {uuid});
} // setPartialServices

/**
 * @brief Set the partial list of services to advertise.
 * @param [in] uuids A vector of 16 bit UUID's to advertise.
 */
void NimBLEAdvertisementData::setPartialServices16(const std::vector<NimBLEUUID>& uuids) {
    setServices(false, 16, uuids);
} // setPartialServices16

/**
 * @brief Set the partial list of services to advertise.
 * @param [in] uuids A vector of 32 bit UUID's to advertise.
 */
void NimBLEAdvertisementData::setPartialServices32(const std::vector<NimBLEUUID>& uuids) {
    setServices(false, 32, uuids);
} // setPartialServices32

/**
 * @brief Utility function to create the list of service UUID's from a vector.
 * @param [in] complete If true the vector is the complete set of services.
 * @param [in] size The bit size of the UUID's in the vector. (16, 32, or 128).
 * @param [in] uuids The vector of service UUID's to advertise.
 */
void NimBLEAdvertisementData::setServices(bool complete, uint8_t size, const std::vector<NimBLEUUID>& uuids) {
    uint8_t bytes  = size / 8;
    uint8_t length = 2; // start with 2 for length + type bytes
    uint8_t data[31];

    for (const auto& uuid : uuids) {
        if (uuid.bitSize() != size) {
            NIMBLE_LOGE(LOG_TAG, "Service UUID(%d) invalid", size);
            continue;
        } else {
            if (length + bytes >= 31) {
                NIMBLE_LOGW(LOG_TAG, "Too many services - truncating");
                complete = false;
                break;
            }
            memcpy(&data[length], uuid.getValue(), bytes);
            length += bytes;
        }
    }

    data[0] = length - 1; // don't count the length byte as part of the AD length

    switch (size) {
        case 16:
            data[1] = (complete ? BLE_HS_ADV_TYPE_COMP_UUIDS16 : BLE_HS_ADV_TYPE_INCOMP_UUIDS16);
            break;
        case 32:
            data[1] = (complete ? BLE_HS_ADV_TYPE_COMP_UUIDS32 : BLE_HS_ADV_TYPE_INCOMP_UUIDS32);
            break;
        case 128:
            data[1] = (complete ? BLE_HS_ADV_TYPE_COMP_UUIDS128 : BLE_HS_ADV_TYPE_INCOMP_UUIDS128);
            break;
        default:
            return;
    }

    addData(data, length);
} // setServices

/**
 * @brief Set the service data (UUID + data)
 * @param [in] uuid The UUID to set with the service data.
 * @param [in] data The data to be associated with the service data advertised.
 */
void NimBLEAdvertisementData::setServiceData(const NimBLEUUID& uuid, const std::string& data) {
    uint8_t sData[31];
    uint8_t uuidBytes = uuid.bitSize() / 8;
    uint8_t length = 2 + uuidBytes + data.length(); // 2 bytes for header + uuid bytes + data
    if (length > 31) {
        NIMBLE_LOGE(LOG_TAG, "Service data too long");
        return;
    }

    memcpy(&sData[2], uuid.getValue(), uuidBytes);
    memcpy(&sData[2 + uuidBytes], data.c_str(), data.length());

    switch (uuidBytes) {
        case 2: {
            sData[1] = BLE_HS_ADV_TYPE_SVC_DATA_UUID16;
            break;
        }
        case 16: {
            sData[1] = BLE_HS_ADV_TYPE_SVC_DATA_UUID128;
            break;
        }
        case 4: {
            sData[1] = BLE_HS_ADV_TYPE_SVC_DATA_UUID32;
            break;
        }
        default:
            return;
    }

    addData(sData, length);
} // setServiceData

/**
 * @brief Adds Tx power level to the advertisement data.
 */
void NimBLEAdvertisementData::addTxPower() {
    uint8_t data[3];
    data[0] = BLE_HS_ADV_TX_PWR_LVL_LEN + 1;
    data[1] = BLE_HS_ADV_TYPE_TX_PWR_LVL;
# ifndef CONFIG_IDF_TARGET_ESP32P4
    data[2] = NimBLEDevice::getPower();
# else
    data[2] = 0;
# endif
    addData(data, 3);
} // addTxPower

/**
 * @brief Set the preferred connection interval parameters.
 * @param [in] min The minimum interval desired.
 * @param [in] max The maximum interval desired.
 */
void NimBLEAdvertisementData::setPreferredParams(uint16_t min, uint16_t max) {
    uint8_t data[6];
    data[0] = BLE_HS_ADV_SLAVE_ITVL_RANGE_LEN + 1;
    data[1] = BLE_HS_ADV_TYPE_SLAVE_ITVL_RANGE;
    data[2] = min;
    data[3] = min >> 8;
    data[4] = max;
    data[5] = max >> 8;
    addData(data, 6);
} // setPreferredParams

/**
 * @brief Retrieve the payload that is to be advertised.
 * @return The payload that is to be advertised.
 */
std::vector<uint8_t> NimBLEAdvertisementData::getPayload() const {
    return m_payload;
} // getPayload

/**
 * @brief Clear the advertisement data for reuse.
 */
void NimBLEAdvertisementData::clearData() {
    std::vector<uint8_t>().swap(m_payload);
} // clearData

#endif /* CONFIG_BT_ENABLED && CONFIG_BT_NIMBLE_ROLE_BROADCASTER  && !CONFIG_BT_NIMBLE_EXT_ADV */
