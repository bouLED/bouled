#include <esp_wifi.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <sys/param.h>
#include <driver/spi_master.h>

#include <esp_http_server.h>

#define SSID "bouLED"
#define PASS "password"

static const char *TAG="bouLED";

extern const uint8_t index_html_str[] asm("_binary_index_html_start");

static spi_device_handle_t spi;

/* An HTTP GET handler */
esp_err_t hello_get_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "GET handler");
	httpd_resp_send(req, (const char*) index_html_str, strlen((const char*) index_html_str));
	return ESP_OK;
}

httpd_uri_t index_uri = {
	.uri       = "/index.html",
	.method    = HTTP_GET,
	.handler   = hello_get_handler,
};

/* An HTTP POST handler */
esp_err_t state_post_handler(httpd_req_t *req)
{
	ESP_LOGI(TAG, "POST handler");
	int buffer_size = 100;
	char buffer[buffer_size];

	/* Redirect to index */
	httpd_resp_set_status(req, "303");
	httpd_resp_set_hdr(req, "Location", "/index.html");

	if (req->content_len > buffer_size) {
		ESP_LOGI(TAG, "Too long POST");
		httpd_resp_send(req, (const char*) index_html_str, strlen((const char*) index_html_str));
		return ESP_OK;
	}
	if (httpd_req_recv(req, buffer, req->content_len) < 0) {
		ESP_LOGI(TAG, "Problem receiving POST");
		return ESP_FAIL;
	}

	int value;
	if (sscanf(buffer, "state=%d", &value) != 1) {
		ESP_LOGI(TAG, "Couldn't parse POST");
	} else {
		ESP_LOGI(TAG, "Value = %d", value);
		spi_transaction_t transaction = {
			.tx_data = {(uint8_t) value, (uint8_t) 127, (uint8_t) 8, (uint8_t) 255}, 
			.length = 32,
			.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA
		};
		ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &transaction));
	}
	httpd_resp_send(req, (const char*) index_html_str, strlen((const char*) index_html_str));

	return ESP_OK;
}

httpd_uri_t state_uri = {
	.uri       = "/state",
	.method    = HTTP_POST,
	.handler   = state_post_handler,
	.user_ctx  = NULL
};

httpd_handle_t start_webserver(void)
{
	httpd_handle_t server = NULL;
	httpd_config_t config = HTTPD_DEFAULT_CONFIG();

	// Start the httpd server
	ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
	if (httpd_start(&server, &config) == ESP_OK) {
		// Set URI handlers
		ESP_LOGI(TAG, "Registering URI handlers");
		httpd_register_uri_handler(server, &index_uri);
		httpd_register_uri_handler(server, &state_uri);
		return server;
	}

	ESP_LOGI(TAG, "Error starting server!");
	return NULL;
}

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
	httpd_handle_t *server = (httpd_handle_t *) ctx;

	switch(event->event_id) {
		case SYSTEM_EVENT_AP_STACONNECTED:
			ESP_LOGI(TAG, "station:"MACSTR" join, AID=%d",
					MAC2STR(event->event_info.sta_connected.mac),
					event->event_info.sta_connected.aid);
			break;
		case SYSTEM_EVENT_AP_STADISCONNECTED:
			ESP_LOGI(TAG, "station:"MACSTR"leave, AID=%d",
					MAC2STR(event->event_info.sta_disconnected.mac),
					event->event_info.sta_disconnected.aid);
			break;
		case SYSTEM_EVENT_AP_START:
			ESP_LOGI(TAG, "SYSTEM_EVENT_AP_START");
			// Start the web server
			if (*server == NULL) {
				*server = start_webserver();
			}
			break;
		default:
			break;
	}
	return ESP_OK;
}

void init_softap(void *arg) {
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, arg));

	// Set the IP to 192.168.0.1
	tcpip_adapter_ip_info_t ip_info;
	IP4_ADDR(&(ip_info.ip), 192, 168, 0, 1);
	IP4_ADDR(&(ip_info.netmask), 255, 255, 255, 0);
	IP4_ADDR(&(ip_info.gw), 192, 168, 0, 1);
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
	ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP, &ip_info));
	ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	wifi_config_t wifi_config = {
		.ap = {
			.ssid = SSID,
			.password = PASS,
			.ssid_len = strlen(SSID),
			.channel = 0,
			.authmode = WIFI_AUTH_WPA_WPA2_PSK,
			.max_connection = 1,
		},
	};

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "softAP inited");
}

void spi_pre_transfer_callback(spi_transaction_t *t) {
	(void) t;
	ESP_LOGI(TAG, "SPI about to transfer");
}

void spi_post_transfer_callback(spi_transaction_t *t) {
	(void) t;
	ESP_LOGI(TAG, "SPI just transfered");
}

void app_main()
{
	static httpd_handle_t server = NULL;
	ESP_ERROR_CHECK(nvs_flash_init());

	spi_bus_config_t bus_config = {
		.miso_io_num = 12,
		.mosi_io_num = 13,
		.sclk_io_num = 14,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 4
	};
	spi_device_interface_config_t device_config = {
		.clock_speed_hz = SPI_MASTER_FREQ_10M,
		.mode = 0,
		.spics_io_num = 15,
		.queue_size = 1,
		.pre_cb = spi_pre_transfer_callback,
		.post_cb = spi_post_transfer_callback
	};
	ESP_ERROR_CHECK(spi_bus_initialize(HSPI_HOST, &bus_config, 1));
	ESP_ERROR_CHECK(spi_bus_add_device(HSPI_HOST, &device_config, &spi));

	init_softap(&server);
}
