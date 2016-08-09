#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ssid_config.h"

#define WEB_SERVER "chainxor.org"
#define WEB_PORT 80
#define WEB_URL "http://chainxor.org/"

void task(void *pvParameters) {
	while(1) {
		vTaskDelay(1000 / portTICK_RATE_MS);
		printf("Hello...\r\n");
	}
}

void user_init(void) {
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

	xTaskCreate(&task, (signed char *)"task", 256, NULL, 2, NULL);
}

