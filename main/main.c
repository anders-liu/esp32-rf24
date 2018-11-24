#include "esp_log.h"

#include "esp32-rf24.h"

static const char *TAG = "MAIN";

void app_main() {
    ESP_LOGV(TAG, "app_main");

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

    rf24_init(&rf24_bus_cfg, &rf24_dev);
    ESP_LOGI(TAG, "app_main: yeah!");

    rf24_free(rf24_dev);
    ESP_LOGI(TAG, "app_main: bye!");
}
