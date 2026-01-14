#include "sdkconfig.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "i2c_manager.h"

static const char *TAG = "i2c_mgr";
static bool initialized = false;
static i2c_master_bus_handle_t i2c_bus = NULL;
static i2c_master_dev_handle_t i2c_device = NULL;

void i2c_manager_init(void)
{
    if (initialized)
    {
        ESP_LOGE(TAG, "I2C already initialized.");
        return;
    }
    // Initialize I2C peripheral here

    i2c_master_bus_config_t cfg = {
        .i2c_port = CONFIG_I2C_PORT_NUM,
        .sda_io_num = CONFIG_I2C_SDA_PIN,
        .scl_io_num = CONFIG_I2C_SCL_PIN,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = CONFIG_I2C_GLITCH_IGNORE_CNT,
        .flags.enable_internal_pullup = true

    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&cfg, &i2c_bus));

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = CONFIG_I2C_SLAVE_ADDRESS,
        .scl_speed_hz = CONFIG_I2C_FREQUENCY_HZ};

    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &dev_config, &i2c_device));

    initialized = true;
    ESP_LOGI(TAG, "I2C initialized on port %d (SDA: GPIO%d, SCL: GPIO%d, Addr: 0x%02X, %dHz)",
             CONFIG_I2C_PORT_NUM, CONFIG_I2C_SDA_PIN, CONFIG_I2C_SCL_PIN,
             CONFIG_I2C_SLAVE_ADDRESS, CONFIG_I2C_FREQUENCY_HZ);
}

int i2c_send_msg(const char *msg, char *resp_buf, size_t buf_size,
                 uint32_t timeout_ms)
{
    if (!initialized || i2c_device == NULL)
    {
        ESP_LOGE(TAG, "I2C not initialized");
        return -1;
    }

    if (buf_size == 0)
    {
        ESP_LOGE(TAG, "Buffer size is 0");
        return -1;
    }

    // Clear the response buffer
    memset(resp_buf, 0, buf_size);

    // Use very short timeout to prevent watchdog/blocking issues
    uint32_t safe_timeout = (timeout_ms > 100) ? 100 : timeout_ms;

    esp_err_t err;

    /*
    // First, probe if device is present (safer than transmit_receive)
    ESP_LOGI(TAG, "Probing I2C device at 0x%02X...", CONFIG_I2C_SLAVE_ADDRESS);
    esp_err_t err = i2c_master_probe(i2c_bus, CONFIG_I2C_SLAVE_ADDRESS, pdMS_TO_TICKS(safe_timeout));

    ESP_LOGI(TAG, "i2c_master_probe result: %s", esp_err_to_name(err));

    if (err != ESP_OK)
    {
        ESP_LOGW(TAG, "Device not found or not responding: %s (0x%x)", esp_err_to_name(err), err);
        snprintf(resp_buf, buf_size, "I2C device not found at 0x%02X", CONFIG_I2C_SLAVE_ADDRESS);
        return -1;
    }

    // Device present, try transmit
    ESP_LOGI(TAG, "Device found! Attempting transmit...");
    */
    err = i2c_master_transmit(
        i2c_device,
        (const uint8_t *)msg, strlen(msg),
        pdMS_TO_TICKS(safe_timeout));

    ESP_LOGI(TAG, "i2c_master_transmit completed with: %s", esp_err_to_name(err));

    if (err != ESP_OK)
    {
        // ESP_ERR_INVALID_STATE (0x103) - Device not responding (NACK on address)
        // ESP_ERR_TIMEOUT (0x107) - Timeout waiting for response
        if (err == ESP_ERR_INVALID_STATE || err == ESP_ERR_TIMEOUT)
        {
            ESP_LOGW(TAG, "No I2C device at address 0x%02X: %s",
                     CONFIG_I2C_SLAVE_ADDRESS, esp_err_to_name(err));
        }
        else
        {
            ESP_LOGE(TAG, "I2C transmit failed: %s (0x%x)", esp_err_to_name(err), err);
        }
        return -1;
    }

    // Now try to receive
    ESP_LOGI(TAG, "Calling i2c_master_receive...");
    err = i2c_master_receive(
        i2c_device,
        (uint8_t *)resp_buf, buf_size - 1,
        pdMS_TO_TICKS(safe_timeout));

    ESP_LOGI(TAG, "i2c_master_receive completed with: %s", esp_err_to_name(err));

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "I2C transmit_receive failed: %s (0x%x)", esp_err_to_name(err), err);

        // Common I2C errors:
        // ESP_ERR_TIMEOUT (0x107) - No device response
        // ESP_FAIL (0xffffffff) - Communication error
        // ESP_ERR_INVALID_STATE - Bus not initialized

        return -1;
    }

    // Find actual length of response (null-terminated or full buffer)
    int len = strnlen(resp_buf, buf_size - 1);
    ESP_LOGI(TAG, "I2C received %d bytes", len);

    return len;
}