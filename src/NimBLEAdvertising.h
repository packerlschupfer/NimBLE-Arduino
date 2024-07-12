/*
 * NimBLEAdvertising.h
 *
 *  Created: on March 3, 2020
 *      Author H2zero
 *
 * Originally:
 *
 * BLEAdvertising.h
 *
 *  Created on: Jun 21, 2017
 *      Author: kolban
 */

#ifndef NIMBLE_CPP_ADVERTISING_H_
#define NIMBLE_CPP_ADVERTISING_H_

#include "nimconfig.h"
#if (defined(CONFIG_BT_ENABLED) && defined(CONFIG_BT_NIMBLE_ROLE_BROADCASTER) && !CONFIG_BT_NIMBLE_EXT_ADV) || \
    defined(_DOXYGEN_)

# if defined(CONFIG_NIMBLE_CPP_IDF)
#  include "host/ble_gap.h"
# else
#  include "nimble/nimble/host/include/host/ble_gap.h"
# endif

/****  FIX COMPILATION ****/
# undef min
# undef max
/**************************/

# include "NimBLEUUID.h"
# include "NimBLEAddress.h"

# include <functional>
# include <string>
# include <vector>

class NimBLEAdvertising;

typedef std::function<void(NimBLEAdvertising*)> advCompleteCB_t;

/**
 * @brief Advertisement data set by the programmer to be published by the %BLE server.
 */
class NimBLEAdvertisementData {
    // Only a subset of the possible BLE architected advertisement fields are currently exposed.  Others will
    // be exposed on demand/request or as time permits.
    //
  public:
    void setAppearance(uint16_t appearance);
    void setCompleteServices(const NimBLEUUID& uuid);
    void setCompleteServices16(const std::vector<NimBLEUUID>& uuids);
    void setCompleteServices32(const std::vector<NimBLEUUID>& uuids);
    void setFlags(uint8_t);
    void setManufacturerData(const std::string& data);
    void setManufacturerData(const std::vector<uint8_t>& data);
    void setURI(const std::string& uri);
    void setName(const std::string& name, bool isComplete = true);
    void setPartialServices(const NimBLEUUID& uuid);
    void setPartialServices16(const std::vector<NimBLEUUID>& uuids);
    void setPartialServices32(const std::vector<NimBLEUUID>& uuids);
    void setServiceData(const NimBLEUUID& uuid, const std::string& data);
    void setShortName(const std::string& name);
    void addData(const uint8_t* data, size_t length);
    void addData(const std::vector<uint8_t>& data);
    void addTxPower();
    void setPreferredParams(uint16_t min, uint16_t max);
    void clearData();

    std::vector<uint8_t> getPayload() const;

  private:
    friend class NimBLEAdvertising;
    void                 setServices(bool complete, uint8_t size, const std::vector<NimBLEUUID>& v_uuid);
    std::vector<uint8_t> m_payload; // The payload of the advertisement.
}; // NimBLEAdvertisementData

/**
 * @brief Perform and manage %BLE advertising.
 *
 * A %BLE server will want to perform advertising in order to make itself known to %BLE clients.
 */
class NimBLEAdvertising {
  public:
    NimBLEAdvertising();
    void addServiceUUID(const NimBLEUUID& serviceUUID);
    void addServiceUUID(const char* serviceUUID);
    void removeServiceUUID(const NimBLEUUID& serviceUUID);
    bool start(uint32_t duration = 0, const NimBLEAddress* dirAddr = nullptr);
    void setAdvertisingCompleteCallback(advCompleteCB_t callback);
    void removeServices();
    bool stop();
    void setAppearance(uint16_t appearance);
    void setName(const std::string& name);
    void setManufacturerData(const std::string& data);
    void setManufacturerData(const std::vector<uint8_t>& data);
    void setURI(const std::string& uri);
    void setServiceData(const NimBLEUUID& uuid, const std::string& data);
    void setConnectableMode(uint8_t mode);
    void setDiscoverableMode(uint8_t mode);
    void setAdvertisingInterval(uint16_t interval);
    void setMaxInterval(uint16_t maxInterval);
    void setMinInterval(uint16_t minInterval);
    void setAdvertisementData(const NimBLEAdvertisementData& advertisementData);
    void setScanFilter(bool scanRequestWhitelistOnly, bool connectWhitelistOnly);
    void setScanResponseData(const NimBLEAdvertisementData& advertisementData);
    void setScanResponse(bool enable);
    void setPreferredConnectionInterval(uint16_t minInterval, uint16_t maxInterval);
    void setMinPreferred(uint16_t min);
    void setMaxPreferred(uint16_t max);
    void addTxPower();
    bool reset();
    bool isAdvertising();

  private:
    friend class NimBLEDevice;
    friend class NimBLEServer;

    void       onHostSync();
    static int handleGapEvent(ble_gap_event* event, void* arg);

    ble_hs_adv_fields       m_advData;
    ble_hs_adv_fields       m_scanData;
    ble_gap_adv_params      m_advParams;
    std::vector<NimBLEUUID> m_serviceUUIDs;
    bool                    m_customAdvData;
    bool                    m_customScanResponseData;
    bool                    m_scanResp;
    bool                    m_advDataSet;
    advCompleteCB_t         m_advCompCb;
    uint8_t                 m_slaveItvl[4];
    uint32_t                m_duration;
    std::vector<uint8_t>    m_svcData16;
    std::vector<uint8_t>    m_svcData32;
    std::vector<uint8_t>    m_svcData128;
    std::vector<uint8_t>    m_name;
    std::vector<uint8_t>    m_mfgData;
    std::vector<uint8_t>    m_uri;
};

#endif /* CONFIG_BT_ENABLED && CONFIG_BT_NIMBLE_ROLE_BROADCASTER  && !CONFIG_BT_NIMBLE_EXT_ADV */
#endif /* NIMBLE_CPP_ADVERTISING_H_ */
