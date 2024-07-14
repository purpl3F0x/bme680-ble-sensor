/**
 * @file custom_ble_service.c
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @copyright Copyright (c) 2024
 */

#include "custom_ble_service.h"

#include <zephyr/init.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_LEVEL CONFIG_BT_ESS_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(custom_ble_service);

struct custom_ble_service_adv_data custom_service_data = {};

static inline uint16_t float_to_fixed_16(const float input, const uint8_t fractional_bits)
{
    return (uint16_t)(input * (1 << fractional_bits));
}

static void data_ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    ARG_UNUSED(attr);
    LOG_INF("Data CCCD cfg changed %04x", value);
}

static int read_data(struct bt_conn* conn, const struct bt_gatt_attr* attr,
    void* buf, uint16_t len, uint16_t offset)
{
    const struct custom_ble_service_adv_data* value = attr->user_data;

    return bt_gatt_attr_read(conn, attr, buf, len, offset, value, sizeof(*value));
}

BT_GATT_SERVICE_DEFINE(
    custom_ble_service,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_CUSTOM_SERVICE),

    BT_GATT_CHARACTERISTIC(BT_UUID_CUSTOM_CHRC,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_READ,
        read_data, NULL, &custom_service_data),
    BT_GATT_CCC(data_ccc_cfg_changed,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

static int custom_ble_service_init(void)
{
    return 0;
}

int custom_ble_set_data(const struct bme_sample_result* measurements)
{
    custom_service_data.temperature = sys_cpu_to_le16(measurements->temperature * 100);

    custom_service_data.humidity = sys_cpu_to_le16(measurements->humidity * 100);
    custom_service_data.pressure = sys_cpu_to_le24(measurements->pressure * 10);

    custom_service_data.iaq = measurements->iaq;
    custom_service_data.iaq_accuracy = measurements->iaqAccuracy;
    custom_service_data.co2_accuracy = measurements->co2Accuracy;
    custom_service_data.breath_voc_accuracy = measurements->breathVocAccuracy;
    custom_service_data.stab_status = measurements->stabStatus;
    custom_service_data.co2_equivalent = sys_cpu_to_le16(float_to_fixed_16(measurements->co2Equivalent, 4));
    custom_service_data.voc = float_to_fixed_16(measurements->breathVocEquivalent, 4);

    return bt_gatt_notify(NULL, &custom_ble_service.attrs[1], &custom_service_data, sizeof(custom_service_data));
};

SYS_INIT(custom_ble_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
