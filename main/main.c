#include "esp_log.h"

#include "esp32-rf24.h"

static const char *TAG = "MAIN";

void app_main()
{
    ESP_LOGD(TAG, "app_main");
    ESP_ERROR_CHECK(rf24_init());
}
