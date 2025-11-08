#include <stdio.h>
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_log.h"

#define M_CFG_WIFI_SSID "MyESP32AP"
#define M_CFG_WIFI_PASSWORD "MyESP32"

static const char *TAG = "HELLO_HTTP";

/**
 * Initializes the Wifi AP
 */
static void wifi_init_ap(void)
{

    // initialize network stack and global event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create AP and initialize resources for Wifi
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // wifi Configuration
    wifi_config_t wifi_cfg = {
        .ap = {
            .ssid = M_CFG_WIFI_SSID,
            .ssid_len = 0,
            .channel = 1,
            .password = M_CFG_WIFI_PASSWORD,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK}};

    if (strlen(M_CFG_WIFI_PASSWORD) == 0)
    {
        wifi_cfg.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi AP started. SSID: %s, password: %s",
             wifi_cfg.ap.ssid, wifi_cfg.ap.password);
}

void app_main(void)
{
}