#include "esp32-rf24.h"

#include "esp_log.h"

static const char *TAG = "RF24";

esp_err_t rf24_init()
{
    ESP_LOGD(TAG, "rf24_init");
    return ESP_OK;
}
