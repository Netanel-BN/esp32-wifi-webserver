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
     * Send a message
     * @param msg Message contents
     * @param buf Pointer to buffer
     * @param buf_size size of buffer
     * @param timeout_ms timeout in milliseconds
     * @return -1 on error
     */
    int uart_send_msg(const char *msg, char *buf, size_t buf_size,
                      uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif