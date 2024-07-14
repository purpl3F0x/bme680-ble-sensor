/**
 * @file ess.h
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @copyright Copyright (c) 2024
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int ess_set_temperature(const float temp);

int ess_set_humidity(const float hum);

int ess_set_pressure(const float press);

#ifdef __cplusplus
}
#endif