/**
 * @file main.c
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @copyright Copyright (c) 2024
 */

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/kernel.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/services/bas.h>
#include <zephyr/bluetooth/uuid.h>

#include <ram_pwrdn.h>

#include <drivers/bme68x_bsec.h>

#include "custom_ble_service.h"
#include "ess.h"

#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(main);

static const struct device* const bme680_dev = DEVICE_DT_GET_ONE(bosch_bme680);

#define ADV_PARAM BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_NAME, \
    BT_GAP_ADV_SLOW_INT_MIN * 3,                                                      \
    BT_GAP_ADV_SLOW_INT_MAX * 3,                                                      \
    NULL)

static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
        // BT_UUID_16_ENCODE(BT_UUID_DIS_VAL),
        BT_UUID_16_ENCODE(BT_UUID_ESS_VAL), ),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

static void connected(struct bt_conn* conn, uint8_t err)
{
    if (err) {
        LOG_ERR("Connection failed (err 0x%02x)\n", err);
    } else {
        LOG_INF("Connected\n");
    }
}

static void disconnected(struct bt_conn* conn, uint8_t reason)
{
    LOG_INF("Disconnected (reason 0x%02x)\n", reason);
}

BT_CONN_CB_DEFINE(conn_callbacks) = {
    .connected = connected,
    .disconnected = disconnected,
};

static void bt_ready(void)
{
    int err;

    LOG_INF("Bluetooth initialized\n");
    // BT_LE_ADV_CONN_NAME
    err = bt_le_adv_start(ADV_PARAM, ad, ARRAY_SIZE(ad), NULL, 0);
    if (err) {
        LOG_ERR("Advertising failed to start (err %d)\n", err);
        return;
    }

    LOG_INF("Advertising successfully started\n");
}

static void auth_cancel(struct bt_conn* conn)
{
    char addr[BT_ADDR_LE_STR_LEN];

    bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

    LOG_INF("Pairing cancelled: %s\n", addr);
}

static const struct bt_conn_auth_cb auth_cb_display = {
    .cancel = auth_cancel,
};

static const struct sensor_trigger sensor_trig = {
    .chan = SENSOR_CHAN_ALL,
    .type = SENSOR_TRIG_DATA_READY,
};

static void trigger_handler(const struct device* dev, const struct sensor_trigger* trig)
{
    ARG_UNUSED(trig);

    // This read is unlocked, should be relatilely safe to do.
    const struct bme_sample_result measurements = *(struct bme_sample_result*)dev->data;

    ess_set_temperature(measurements.temperature);
    ess_set_humidity(measurements.humidity);
    ess_set_pressure(measurements.pressure);

    custom_ble_set_data(&measurements);

    // LOG_INF(" %llu %.2f %.2f %.2f %.2f %.2f %d",
    //     measurements.timestamp,
    //     measurements.temperature,
    //     measurements.humidity,
    //     measurements.pressure,
    //     measurements.breathVocEquivalent,
    //     measurements.co2Equivalent,
    //     measurements.iaq);
}

int main(void)
{
    int err;
    // Initalize sensors
    if (!device_is_ready(bme680_dev)) {
        LOG_ERR("bme680_dev: device not ready.");
        return 0;
    }

    if (sensor_trigger_set(bme680_dev, &sensor_trig, trigger_handler)) {
        LOG_ERR("Cannot configure trigger.");
        return 0;
    }

    // Initialize bluetooth
    err = bt_enable(NULL);
    if (err) {
        LOG_ERR("Bluetooth init failed (err %d)\n", err);
        return 0;
    }

    bt_ready();
    bt_conn_auth_cb_register(&auth_cb_display);

    if (IS_ENABLED(CONFIG_RAM_POWER_DOWN_LIBRARY)) {
        power_down_unused_ram();
    }

    return 0;
}
