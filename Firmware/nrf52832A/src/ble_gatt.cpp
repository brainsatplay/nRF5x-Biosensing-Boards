#include <zephyr.h>

#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <functional>

#include <logging/log.h>

#include <sys/atomic.h>

#include "ble_gatt.hpp"


namespace Bluetooth::Gatt
{

LOG_MODULE_REGISTER(BleGatt);

/********************************************/
/* BLE connection */

atomic_t ads131m08NotificationsEnable = false;

/* BT832A Custom Service  */
bt_uuid_128 sensorServiceUUID = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x0000cafe, 0xb0ba, 0x8bad, 0xf00d, 0xdeadbeef0000));

/* Sensor Control Characteristic */
bt_uuid_128 sensorCtrlCharUUID = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x0001cafe, 0xb0ba, 0x8bad, 0xf00d, 0xdeadbeef0000));

/* Sensor Data Characteristic */
bt_uuid_128 sensorDataUUID = BT_UUID_INIT_128(
	BT_UUID_128_ENCODE(0x0002cafe, 0xb0ba, 0x8bad, 0xf00d, 0xdeadbeef0000));

/**
 * @brief CCCD handler for ADS131M08 characteristic. Used to get notifications if client enables notifications
 *        for ADS131M08 characteristic. CCC = Client Characteristic Configuration
 * 
 * @param attr Ble Gatt attribute
 * @param value characteristic value
 */
static void sensorCccHandler(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);
	//notify_enable = (value == BT_GATT_CCC_NOTIFY);
    atomic_set(&ads131m08NotificationsEnable, value == BT_GATT_CCC_NOTIFY);
	LOG_INF("Notification %s", ads131m08NotificationsEnable ? "enabled" : "disabled");
}


#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)
#define ADV_LEN 12

/* Advertising data */
static uint8_t manuf_data[ADV_LEN] = {
	0x01 /*SKD version */,
	0x83 /* STM32WB - P2P Server 1 */,
	0x00 /* GROUP A Feature  */,
	0x00 /* GROUP A Feature */,
	0x00 /* GROUP B Feature */,
	0x00 /* GROUP B Feature */,
	0x00, /* BLE MAC start -MSB */
	0x00,
	0x00,
	0x00,
	0x00,
	0x00, /* BLE MAC stop */
};

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),
	BT_DATA(BT_DATA_MANUFACTURER_DATA, manuf_data, ADV_LEN)
};

/**
 * @brief Gatt Service definition
 */
BT_GATT_SERVICE_DEFINE(bt832a_svc,
BT_GATT_PRIMARY_SERVICE(&sensorServiceUUID),
BT_GATT_CHARACTERISTIC(&sensorCtrlCharUUID.uuid,
		       BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE_WITHOUT_RESP,
		       BT_GATT_PERM_WRITE, nullptr, nullptr, nullptr),
BT_GATT_CHARACTERISTIC(&sensorDataUUID.uuid, BT_GATT_CHRC_NOTIFY,
		       BT_GATT_PERM_READ, nullptr, nullptr, nullptr), //&ble_tx_buff),
BT_GATT_CCC(sensorCccHandler, BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),
);

/********************************************************/


/**
 * @brief Callback clled when Bluetooth is initialized. Starts BLE server
 * 
 * @param err bluetooth initialization error code
 */
void OnBluetoothStarted(int err)
{
    if (err)
    {
        LOG_ERR("Bluetooth init failed (err %d)", err);
        return;
    }

    bt_le_adv_param param = BT_LE_ADV_PARAM_INIT(BT_LE_ADV_OPT_CONNECTABLE, BT_GAP_ADV_FAST_INT_MIN_2, BT_GAP_ADV_FAST_INT_MAX_2, NULL);

    LOG_INF("Bluetooth initialized");

    /* Start advertising */
    err = bt_le_adv_start(&param, ad, ARRAY_SIZE(ad), nullptr, 0); // nullptr for scan response 0 for scan response data size
    if (err)
    {
        LOG_ERR("Advertising failed to start (err %d)", err);
        return;
    }

    LOG_INF("Configuration mode: waiting connections...");
}

} // namespace Bluetooth::Gatt
