idf_component_register(SRCS "httpd_manager.c"
                       INCLUDE_DIRS "include"
                       REQUIRES esp_http_server uart_manager)
