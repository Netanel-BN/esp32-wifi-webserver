#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"

#include "httpd_manager.h"

#define M_HEADER_CT_TEXT "text/plain"

static const char *TAG = "httpd_manager";

static esp_err_t hello_handler(httpd_req_t *req)
{
    const char *resp_str = "Welcome!";
    httpd_resp_set_type(req, M_HEADER_CT_TEXT);
    httpd_resp_send(req, resp_str, strlen(resp_str));
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

    ESP_LOGI(TAG, "Server started");
}