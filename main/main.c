
#include "wifi_manager.h"
#include "httpd_manager.h"
#include "uart_manager.h"
#include "i2c_manager.h"
static const char *TAG = "main";

void app_main(void)
{
    uart_manager_init();
    i2c_manager_init();
    wifi_ap_init();
    httpd_init();
}