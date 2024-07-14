/**
 * @file bme68x_bsec.h
 *
 * @copyright Copyright (c) 2023 Nordic Semiconductor ASA - 2024 Stavros Avramidis
 */

#ifndef __BME68X_BSEC_H__
#define __BME68X_BSEC_H__
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

struct bme_sample_result {
    int64_t timestamp;

    uint16_t iaq;
    float co2Equivalent;
    float breathVocEquivalent;

    float pressure;
    float temperature;
    float humidity;

    bool stabStatus;
    bool runInStatus;

    uint8_t iaqAccuracy;
    uint8_t co2Accuracy;
    uint8_t breathVocAccuracy;
};

#ifdef __cplusplus
}
#endif

#endif /* __BME68X_BSEC_H__ */
