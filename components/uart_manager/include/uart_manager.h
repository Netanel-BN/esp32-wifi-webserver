#pragma once

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * Initialize UART with settings from Kconfig
     */
    esp_err_t uart_manager_init(void);

    /**
     * Send data via UART
     * @param data Pointer to data to send
     * @param len Length of data
     * @return Number of bytes written, or -1 on error
     */
    int uart_manager_send(const char *data, size_t len);

    /**
     * Receive data from UART (blocking with timeout)
     * @param buffer Buffer to receive data into
     * @param max_len Maximum bytes to read
     * @param timeout_ms Timeout in milliseconds
     * @return Number of bytes read, or -1 on error/timeout
     */
    int uart_manager_receive(char *buffer, size_t max_len, uint32_t timeout_ms);

    /**
     * Send a string via UART
     * @param str Null-terminated string
     * @return Number of bytes written, or -1 on error
     */
    int uart_manager_send_string(const char *str);

    /**
     * Flush UART TX buffer
     */
    void uart_manager_flush(void);

#ifdef __cplusplus
}
#endif