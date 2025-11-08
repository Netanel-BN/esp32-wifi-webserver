#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"

#include "esp_http_server.h"

#define M_CFG_WIFI_SSID "MyESP32AP"
#define M_CFG_WIFI_PASSWORD "MyESP32PW!"

#define M_HEADER_CT_TEXT "text/plain"

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

static esp_err_t hello_handler(httpd_req_t *req)
{

    const char *resp_str = "Hello!";
    httpd_resp_set_type(req, M_HEADER_CT_TEXT);
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static void webserver_start(void)
{
    // Configure server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    // define handler
    httpd_uri_t hello_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = hello_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server, &hello_uri);

    ESP_LOGI(TAG, "Server started");
}

void app_main(void)
{

    // initialize nvs flash for WiFi
    esp_err_t ret = nvs_flash_init();

    // reflash if needed due to no pages / new version
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    wifi_init_ap();
    webserver_start();
}