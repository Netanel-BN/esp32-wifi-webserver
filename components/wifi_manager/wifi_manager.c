#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"

#include "wifi_manager.h"

static const char *TAG = "wifi_manager";

void wifi_ap_init()
{
    // init NVS flash
    esp_err_t ret = nvs_flash_init();
    // reflash if needed due to no pages / new version
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // initialize network stack and global event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create AP and initialize resources for Wifi
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    // wifi Configuration
    wifi_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    strncpy((char *)cfg.ap.ssid, CONFIG_WIFI_SSID,
            sizeof(cfg.ap.ssid) - 1);

    cfg.ap.ssid[sizeof(cfg.ap.ssid) - 1] = '\0';

    cfg.ap.ssid_len = 0;
    cfg.ap.channel = 1;
    cfg.ap.max_connection = atoi((char *)(CONFIG_WIFI_MAX_CONNECTIONS));

// no auth
#ifdef CONFIG_WIFI_AUTH_OPEN
    cfg.ap.authmode = WIFI_AUTH_OPEN;
    cfg.ap.password[0] = '\0';
    ESP_LOGI(TAG, "WiFi AP configured for OPEN authentication (no password)");

#else
    // Auth enabled - make sure password is correct

    char *password = CONFIG_WIFI_PASSWORD;
    size_t pw_len = strlen(password);
    if (pw_len < MIN_PW_LENGTH || pw_len > MAX_PW_LENGTH)
    {
        ESP_LOGE(TAG, "Invalid password length: %d (must be 8-63 chars for WPA2)", (int)pw_len);
        ESP_LOGE(TAG, "Please reconfigure via: idf.py menuconfig -> Component config -> WiFi Manager Configuration");
        abort();
    }

    // correct password
    strncpy((char *)cfg.ap.password, password, sizeof(cfg.ap.password));
    cfg.ap.password[sizeof(cfg.ap.password) - 1] = '\0';
    cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_LOGI(TAG, "WiFi AP configured for WPA2-PSK authentication");

#endif

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    const char *mode = cfg.ap.authmode == WIFI_AUTH_OPEN ? "Without" : "With";
    ESP_LOGI(TAG, "Wi-Fi AP started. SSID: %s, %s authentication",
             cfg.ap.ssid, mode);
}
