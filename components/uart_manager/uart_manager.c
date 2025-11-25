#include "sdkconfig.h"
#include <string.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "uart_manager.h"

static const char *TAG = "uart_mgr";
static bool initialized = false;

esp_err_t uart_manager_init(void)
{

    if (initialized)
    {
        ESP_LOGW(TAG, "UART already initialized.");
        return ESP_OK;
    }

    uart_config_t uart_config = {
        .baud_rate = CONFIG_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT};

    ESP_ERROR_CHECK(uart_driver_install(CONFIG_UART_PORT_NUM,
                                        CONFIG_UART_RX_BUFFER_SIZE, CONFIG_UART_TX_BUFFER_SIZE,
                                        0, NULL, 0));

    ESP_ERROR_CHECK(uart_param_config(CONFIG_UART_PORT_NUM, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(CONFIG_UART_PORT_NUM,
                                 CONFIG_UART_TX_PIN,
                                 CONFIG_UART_RX_PIN,
                                 UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));

    initialized = true;

    ESP_LOGI(TAG, "UART%d initialized: %d baud, TX=%d, RX=%d",
             CONFIG_UART_PORT_NUM, CONFIG_UART_BAUD_RATE,
             CONFIG_UART_TX_PIN, CONFIG_UART_RX_PIN);

    return ESP_OK;
}

int uart_manager_send(const char *data, size_t len)
{
    if (!initialized)
    {
        ESP_LOGE(TAG, "UART not initialized.");
        return -1;
    }

    int written = uart_write_bytes(CONFIG_UART_PORT_NUM, data, len);
    return written;
}

int uart_manager_receive(char *buffer, size_t max_len, uint32_t timeout_ms)
{

    if (!initialized)
    {
        ESP_LOGE(TAG, "UART not initialized.");
        return -1;
    }

    int len = uart_read_bytes(CONFIG_UART_PORT_NUM, (uint8_t *)buffer, max_len,
                              pdMS_TO_TICKS(timeout_ms));

    return len;
}

int uart_manager_send_string(const char *str)
{

    return uart_manager_send(str, strlen(str));
}

void uart_manager_flush(void)
{

    if (initialized)
    {
        uart_wait_tx_done(CONFIG_UART_PORT_NUM, pdMS_TO_TICKS(1000));
    }
}
