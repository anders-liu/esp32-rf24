#include <string.h>

#include "esp32-rf24.h"

#include "esp_log.h"

static const char *TAG = "RF24";

struct rf24_dev_t {
    rf24_bus_cfg_t bus_cfg;
    bool bus_initialized;
    spi_device_handle_t spi_dev;
};

#define RF24_CHECK(a, str, ret_val, ...)                                      \
    if (!(a)) {                                                               \
        ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
        return (ret_val);                                                     \
    }

/**
 * Public functions.
 */

esp_err_t rf24_init(rf24_bus_cfg_t *bus_cfg, rf24_dev_handle_t *handle) {
    ESP_LOGV(TAG, "rf24_init");

    RF24_CHECK(bus_cfg != NULL, "invalid bus config", ESP_ERR_INVALID_ARG);
    RF24_CHECK(GPIO_IS_VALID_OUTPUT_GPIO(bus_cfg->ce_io_num), "invalid CE pin", ESP_ERR_INVALID_ARG);
    RF24_CHECK(handle != NULL, "invalid output handle", ESP_ERR_INVALID_ARG);

    esp_err_t ret = ESP_OK;

    struct rf24_dev_t *rf_dev = malloc(sizeof(struct rf24_dev_t));
    if (rf_dev == NULL) {
        ESP_LOGD(TAG, "malloc failed");
        ret = ESP_ERR_NO_MEM;
        goto cleanup;
    }

    memset(rf_dev, 0, sizeof(struct rf24_dev_t));
    rf_dev->bus_cfg = *bus_cfg;

    // Initialize bus if needed.
    if (bus_cfg->init_host) {
        spi_bus_config_t spi_buscfg = {
            .mosi_io_num = bus_cfg->mosi_io_num,
            .miso_io_num = bus_cfg->miso_io_num,
            .sclk_io_num = bus_cfg->sclk_io_num,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1
        };

        ret = spi_bus_initialize(bus_cfg->spi_host, &spi_buscfg, 1);

        if (ret != ESP_OK) {
            ESP_LOGD(TAG, "spi_bus_initialize failed: %s", esp_err_to_name(ret));
            goto cleanup;
        }
        rf_dev->bus_initialized = true;
    }

    // Initialize device.
    spi_device_interface_config_t spi_devcfg={
        .clock_speed_hz = 8000000,
        .mode = 0,
        .spics_io_num = bus_cfg->cs_io_num,
        .queue_size = 7
    };

    ESP_LOGV(TAG, "rf24_init: spi_bus_add_device");
    spi_device_handle_t spi_dev;
    ret = spi_bus_add_device(bus_cfg->spi_host, &spi_devcfg, &spi_dev);

    if (ret != ESP_OK) {
        ESP_LOGD(TAG, "spi_bus_add_device failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }
    rf_dev->spi_dev = spi_dev;

    *handle = rf_dev;
    return ret;

cleanup:
    ESP_LOGV(TAG, "rf24_init: cleanup");

    if (rf_dev) {
        if (rf_dev->bus_initialized) {
            spi_bus_free(bus_cfg->spi_host);
        }
        if (rf_dev->spi_dev) {
            spi_bus_remove_device(rf_dev->spi_dev);
        }
        free(rf_dev);
    }
    return ret;
}

esp_err_t rf24_free(rf24_dev_handle_t handle)
{
    ESP_LOGV(TAG, "rf24_free");

    RF24_CHECK(handle != NULL, "invalid handle", ESP_ERR_INVALID_ARG);

    ESP_LOGV(TAG, "rf24_free: spi_bus_remove_device");
    spi_bus_remove_device(handle->spi_dev);

    if (handle->bus_initialized) {
        ESP_LOGV(TAG, "rf24_free: spi_bus_free");
        spi_bus_free(handle->bus_cfg.spi_host);
    }

    free(handle);
    return ESP_OK;
}
