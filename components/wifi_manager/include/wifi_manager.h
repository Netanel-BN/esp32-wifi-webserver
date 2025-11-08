#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// min and max password lengths - WPA2 compliant
#define MIN_PW_LENGTH 8
#define MAX_PW_LENGTH 63
/**
 * Initializes the Wi-Fi Access point
 */
void wifi_ap_init(void);

#endif // WIFI_MANAGER_H