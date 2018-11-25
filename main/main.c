#include "esp_log.h"

#include "esp32-rf24.h"

static const char *TAG = "[MAIN]";

void app_main() {
    ESP_LOGV(TAG, "app_main");

    esp_err_t ret;

    rf24_bus_cfg_t rf24_bus_cfg = {
        .spi_host = HSPI_HOST,
        .init_host = true,
        .mosi_io_num = GPIO_NUM_22,
        .miso_io_num = GPIO_NUM_36,
        .sclk_io_num = GPIO_NUM_23,
        .cs_io_num = GPIO_NUM_26,
        .ce_io_num = GPIO_NUM_25
    };
    rf24_dev_handle_t rf24_dev;

    ret = rf24_init(&rf24_bus_cfg, &rf24_dev);

    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "app_main: RF24 initialized failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ESP_LOGI(TAG, "app_main: RF24 initialized");

    // Get status.
    rf24_status status;
    ret = rf24_get_status(rf24_dev, &status);

    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "app_main: rf24_get_status failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ESP_LOGI(TAG, "app_main: RF24 status: "
        "rx_data_ready=%u, tx_data_sent=%u, "
        "tx_max_retried=%u, rx_pipe_no=%u, "
        "tx_full=%u",
        status.rx_data_ready, status.tx_data_sent,
        status.tx_max_retried, status.rx_pipe_no,
        status.tx_full);

    // Check chip connection.
    bool is_chip_connected;
    ret = rf24_is_chip_connected(rf24_dev, &is_chip_connected);

    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "app_main: rf24_is_chip_connected failed: %s", esp_err_to_name(ret));
        goto cleanup;
    }

    ESP_LOGI(TAG, "app_main: RF24 connected: %s", is_chip_connected ? "yes" : "no");

cleanup:
    rf24_free(rf24_dev);
    ESP_LOGI(TAG, "app_main: bye!");
}
