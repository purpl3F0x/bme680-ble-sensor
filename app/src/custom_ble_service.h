/**
 * @file custom_ble_service.h
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @copyright Copyright (c) 2024
 */


#pragma once

#include <zephyr/bluetooth/uuid.h>

#include <drivers/bme68x_bsec.h>

#ifdef __cplusplus
extern "C" {
#endif

// 355545c8-c32a-44c5-979d-33a5e643d800
// 355545c8-c32a-44c5-979d-33a5e643d801

#define BT_UUID_CUSTOM_SERVICE_VAL BT_UUID_128_ENCODE(0x355545c8, 0xc32a, 0x44c5, 0x979d, 0x33a5e643d800)
#define BT_UUID_CUSTOM_CHRC_VAL BT_UUID_128_ENCODE(0x355545c8, 0xc32a, 0x44c5, 0x979d, 0x33a5e643d801)

#define BT_UUID_CUSTOM_SERVICE BT_UUID_DECLARE_128(BT_UUID_CUSTOM_SERVICE_VAL)
#define BT_UUID_CUSTOM_CHRC BT_UUID_DECLARE_128(BT_UUID_CUSTOM_CHRC_VAL)


struct custom_ble_service_adv_data {
    int16_t temperature;
    uint16_t humidity;

    uint32_t pressure : 24;

    uint16_t iaq : 10;
    uint16_t iaq_accuracy : 2;
    uint16_t co2_accuracy : 2;
    uint16_t breath_voc_accuracy : 2;

    uint16_t co2_equivalent;

    uint16_t voc : 14;
    uint16_t stab_status : 1;
    uint16_t reserved: 1;
} __attribute__((packed));


int custom_ble_set_data(const struct bme_sample_result* measurements);


#ifdef __cplusplus
}
#endif