#pragma once

#include "esp_err.h"
#include <stddef.h>

void i2c_manager_init(void);

int i2c_send_msg(const char *msg, char *resp_buf, size_t buf_size,
                 uint32_t timeout_ms);