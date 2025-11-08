
#include "wifi_manager.h"
#include "httpd_manager.h"

static const char *TAG = "main";

void app_main(void)
{
    wifi_ap_init();
    httpd_init();
}