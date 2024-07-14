/**
 * @file ess.c
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @copyright Copyright (c) 2024
 */

#include "ess.h"

#include <zephyr/init.h>
#include <zephyr/sys/byteorder.h>

#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/uuid.h>

#define LOG_LEVEL CONFIG_BT_ESS_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(ess);

static uint16_t temperature = 0; // Units in 0.01°C
static uint16_t humidity = 0; // Units in 0.01%RH
static uint32_t pressure = 0; // Units in 0.1Pa

static void temp_ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    ARG_UNUSED(attr);
    LOG_INF("Temperature CCCD cfg changed %04x", value);
}

static void hum_ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    ARG_UNUSED(attr);
    LOG_INF("Humidity CCCD cfg changed %04x", value);
}

static void press_ccc_cfg_changed(const struct bt_gatt_attr* attr, uint16_t value)
{
    ARG_UNUSED(attr);
    LOG_INF("Pressure CCCD cfg changed %04x", value);
}

static ssize_t read_u16(struct bt_conn* conn, const struct bt_gatt_attr* attr,
    void* buf, uint16_t len, uint16_t offset)
{
    const uint16_t* u16 = attr->user_data;
    uint16_t value = sys_cpu_to_le16(*u16);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,
        sizeof(value));
}

static ssize_t read_u32(struct bt_conn* conn, const struct bt_gatt_attr* attr,
    void* buf, uint16_t len, uint16_t offset)
{
    const uint32_t* u32 = attr->user_data;
    uint32_t value = sys_cpu_to_le32(*u32);

    return bt_gatt_attr_read(conn, attr, buf, len, offset, &value,
        sizeof(value));
}

BT_GATT_SERVICE_DEFINE(
    ess,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_ESS),

    BT_GATT_CHARACTERISTIC(BT_UUID_TEMPERATURE,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_READ,
        read_u16, NULL, &temperature),
    BT_GATT_CCC(temp_ccc_cfg_changed,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_HUMIDITY,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_READ,
        read_u16, NULL, &humidity),
    BT_GATT_CCC(hum_ccc_cfg_changed,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE),

    BT_GATT_CHARACTERISTIC(BT_UUID_PRESSURE,
        BT_GATT_CHRC_READ | BT_GATT_CHRC_NOTIFY,
        BT_GATT_PERM_READ,
        read_u32, NULL, &pressure),
    BT_GATT_CCC(press_ccc_cfg_changed,
        BT_GATT_PERM_READ | BT_GATT_PERM_WRITE), );

static int ess_init(void)
{
    return 0;
}

int ess_set_temperature(const float temp)
{
    temperature = temp * 100;
    return bt_gatt_notify(NULL, &ess.attrs[1], &temperature, sizeof(temperature));
}

int ess_set_humidity(const float hum)
{
    humidity = hum * 100;
    return bt_gatt_notify(NULL, &ess.attrs[4], &humidity, sizeof(humidity));
}

int ess_set_pressure(const float press)
{
    pressure = press * 10;
    return bt_gatt_notify(NULL, &ess.attrs[7], &pressure, sizeof(pressure));
}

SYS_INIT(ess_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);