/**
 * @file bme68x_sensor.h
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @brief Zephyr driver for BME68X + BSEC
 * @copyright Copyright (c) 2023 Nordic Semiconductor ASA - 2024 Stavros Avramidis
 */

#include <zephyr/kernel.h>

#include "bme68x.h"
#include "bsec_interface.h"

#include <drivers/bme68x_bsec.h>

#ifndef DRIVERS_SENSOR_BME68X_BSEC
#define DRIVERS_SENSOR_BME68X_BSEC

#define DT_DRV_COMPAT bosch_bme680

#define BME680_BUS_SPI DT_ANY_INST_ON_BUS_STATUS_OKAY(spi)
#define BME680_BUS_I2C DT_ANY_INST_ON_BUS_STATUS_OKAY(i2c)

#if !BME680_BUS_SPI && !BME680_BUS_I2C
#error "Unsupported bus for BME680"
#endif

#if BME680_BUS_SPI
#define BME680_SPI_OPERATION \
    (SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_OP_MODE_MASTER)
#endif


struct bme68x_bsec_data {
    /* Variable to store intermediate sample result */
    struct bme_sample_result latest;

    /* Trigger and corresponding handler */
    sensor_trigger_handler_t trg_handler;
    const struct sensor_trigger* trigger;

    /* Internal BSEC thread metadata value. */
    struct k_thread thread;

    /* Buffer used to maintain the BSEC library state. */
    uint8_t state_buffer[BSEC_MAX_STATE_BLOB_SIZE];

    /* Size of the saved state */
    int32_t state_len;

    bsec_sensor_configuration_t required_sensor_settings[BSEC_MAX_PHYSICAL_SENSOR];
    uint8_t n_required_sensor_settings;

    /* some RAM space needed by bsec_get_state and bsec_set_state for (de-)serialization. */
    uint8_t work_buffer[BSEC_MAX_WORKBUFFER_SIZE];

    bool initialized;

    struct bme68x_dev dev;
};

#endif /* DRIVERS_SENSOR_BME68X_BSEC */
