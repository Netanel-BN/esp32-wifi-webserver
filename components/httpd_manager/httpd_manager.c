#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"

#include "httpd_manager.h"
#include "uart_manager.h"
#include "i2c_manager.h"

#define M_HEADER_CT_TEXT "text/plain"

static const char *TAG = "httpd_manager";

static esp_err_t hello_handler(httpd_req_t *req)
{
    const char *resp_str = "Welcome!";
    httpd_resp_set_type(req, M_HEADER_CT_TEXT);
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}

static esp_err_t uart_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Handling status request");

    size_t buf_size = 64;
    char resp[buf_size];

    int len = uart_send_msg("STATUS", resp, buf_size, 2000);

    if (len > 0)
    {
        ESP_LOGI(TAG, "Received from UART: %s", resp);
        httpd_resp_send(req, resp, len);
    }
    else
    {
        const char *msg = "No UART response from device";
        ESP_LOGE(TAG, "%s", msg);
        httpd_resp_send(req, msg, strlen(msg));
    }

    return ESP_OK;
}

static esp_err_t i2c_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Handling I2C request - START");

    size_t buf_size = 64;
    char resp[buf_size];

    ESP_LOGI(TAG, "About to call i2c_send_msg...");
    // Use very short timeout to prevent blocking issues
    int len = i2c_send_msg("I2C", resp, buf_size, 100);
    ESP_LOGI(TAG, "i2c_send_msg returned: %d", len);

    if (len > 0)
    {
        ESP_LOGI(TAG, "Received from I2C: %s", resp);
        httpd_resp_send(req, resp, len);
    }
    else
    {
        const char *msg = "Failed to read from I2C device";
        ESP_LOGW(TAG, "%s", msg);
        httpd_resp_send(req, msg, strlen(msg));
    }

    ESP_LOGI(TAG, "Handling I2C request - END");
    return ESP_OK;
}

void httpd_init(void)
{
    // Configure server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_ERROR_CHECK(httpd_start(&server, &config));

    // define handlers
    httpd_uri_t hello_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = hello_handler,
        .user_ctx = NULL};
    httpd_register_uri_handler(server, &hello_uri);

    httpd_uri_t uart_uri = {
        .uri = "/uart",
        .method = HTTP_GET,
        .handler = uart_handler,
        .user_ctx = NULL};

    httpd_register_uri_handler(server, &uart_uri);

    httpd_uri_t i2c_uri = {
        .uri = "/i2c",
        .method = HTTP_GET,
        .handler = i2c_handler,
        .user_ctx = NULL};

    httpd_register_uri_handler(server, &i2c_uri);
    ESP_LOGI(TAG, "Server started");
}