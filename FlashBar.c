#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "lwip/api.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "ssid_config.h"

#define PORT 80

char* buf;
u16_t buflen;
void server_serve(struct netconn *conn) {
	struct netbuf *inbuf;
	err_t recv_err;

	recv_err = netconn_recv(conn, &inbuf);
	if (recv_err == ERR_OK){
		printf("Receive\r\n");
		netbuf_data(inbuf, (void**)&buf, &buflen);

		printf("%s\n", buf);
		char outBuff[80];
		snprintf(outBuff, sizeof(outBuff), "Uptime %d seconds\r\n",
			xTaskGetTickCount()*portTICK_RATE_MS/1000);
		netconn_write(conn, outBuff, strlen(outBuff), NETCONN_COPY);
		//netconn_write(conn, (const unsigned char*)(start_page), (size_t)get_start_page_size(), NETCONN_NOCOPY);
	}

	netconn_close(conn);
	netbuf_delete(inbuf);
}

void task(void *pvParameters) {
	printf("Open server in 10 seconds.\r\n");
	vTaskDelay(10000 / portTICK_RATE_MS);

	struct netconn *conn, *newconn;
	err_t err, accept_err;

	/* Create a new TCP connection handle */
	conn = netconn_new(NETCONN_TCP);
	if(!conn) {
		printf("Error: Failed to allocate socket.\r\n");
		return;
	}

	/* Bind to port 80 (HTTP) with default IP address */
	err = netconn_bind(conn, IP_ADDR_ANY, PORT);
	if(err != ERR_OK) {
		printf("Error: Failed to bind socket. err=%d\r\n", err);
		return;
	}

	/* Put the connection into LISTEN state */
	netconn_listen(conn);
	printf("Listening for connections.\r\n");

	while(1) {
		printf("Loop\r\n");

		/* accept any icoming connection */
		accept_err = netconn_accept(conn, &newconn);
		printf("Accept incoming\r\n");
		if(accept_err == ERR_OK) {
			server_serve(newconn);
			netconn_delete(newconn);
		}
	}
}

void user_init(void) {
    uart_set_baud(0, 115200);
    printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASS,
    };
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

	xTaskCreate(&task, (signed char *)"task", 256, NULL, 2, NULL);
}

