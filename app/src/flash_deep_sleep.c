/**
 * @file flash_deep_sleep.c
 * @author Stavros Avramidis (stavros9899@gmail.com)
 * @brief Sets Deep Sleep state in the external flash of the nrf52840 m2, to reduce power.
 * @copyright Copyright (c) 2024
 */

#if CONFIG_BOARD_NRF52840_M2

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/kernel.h>
#include <zephyr/pm/device.h>

static int flash_deep_sleep_enter(void)
{
    static const struct device* flash_dev = DEVICE_DT_GET(DT_ALIAS(spi_flash0));

    return pm_device_action_run(flash_dev, PM_DEVICE_ACTION_SUSPEND);
}

SYS_INIT(flash_deep_sleep_enter, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // CONFIG_BOARD_NRF52840_M2