/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "tcpip_adapter.h"
#include "esp_smartconfig.h"
#include "osCore.h"
#include "pluto.h"
#include "hal_led.h"

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int CONNECTED_BIT = BIT0;
static const int ESPTOUCH_DONE_BIT = BIT1;
static const int START_PROCESS_BIT	= BIT2;
static const char *TAG = "sc";
static int airkiss_state = SC_STATUS_LINK_OVER;

void smartconfig_example_task(void * parm);
static void sc_callback(smartconfig_status_t status, void *pdata);
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;
    default:
        break;
    }
    return ESP_OK;
}

esp_err_t initialise_wifi(void)
{
	esp_err_t 		ret = ESP_OK;
	wifi_config_t	sta_conf;
    wifi_event_group = xEventGroupCreate();
    esp_event_loop_init(event_handler, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );

	ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_conf));
	if((osMemchkz(sta_conf.sta.ssid,32)==0)||osMemchkz(sta_conf.sta.password,64)==0)
	{
	    ESP_ERROR_CHECK( esp_smartconfig_set_type(SC_TYPE_AIRKISS) );
	    ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
		ret = ESP_FAIL;
	}
	else
	{
		ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_conf));
		ESP_ERROR_CHECK(esp_wifi_connect());
	}
	xTaskCreate(smartconfig_example_task, "smartconfig_example_task", 2048, NULL, 3, NULL);
	return ret;
}

static void sc_callback(smartconfig_status_t status, void *pdata)
{
    switch (status) {
        case SC_STATUS_WAIT:
        	airkiss_state = status;
            ESP_LOGI(TAG, "SC_STATUS_WAIT");
            break;
        case SC_STATUS_FIND_CHANNEL:
        	airkiss_state = status;
        	xEventGroupSetBits(wifi_event_group, START_PROCESS_BIT);
            ESP_LOGI(TAG, "SC_STATUS_FINDING_CHANNEL");
            break;
        case SC_STATUS_GETTING_SSID_PSWD:
        	airkiss_state = status;
            ESP_LOGI(TAG, "SC_STATUS_GETTING_SSID_PSWD");
            break;
        case SC_STATUS_LINK:
        	airkiss_state = status;
            ESP_LOGI(TAG, "SC_STATUS_LINK");
            wifi_config_t *wifi_config = pdata;
            ESP_LOGI(TAG, "SSID:%s", wifi_config->sta.ssid);
            ESP_LOGI(TAG, "PASSWORD:%s", wifi_config->sta.password);
            ESP_ERROR_CHECK( esp_wifi_disconnect() );
            ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, wifi_config) );
            ESP_ERROR_CHECK( esp_wifi_connect() );
            xEventGroupSetBits(wifi_event_group, START_PROCESS_BIT);
            break;
        case SC_STATUS_LINK_OVER:
        	airkiss_state = status;
            ESP_LOGI(TAG, "SC_STATUS_LINK_OVER");
            if (pdata != NULL) {
                uint8_t phone_ip[4] = { 0 };
                memcpy(phone_ip, (uint8_t* )pdata, 4);
                ESP_LOGI(TAG, "Phone ip: %d.%d.%d.%d\n", phone_ip[0], phone_ip[1], phone_ip[2], phone_ip[3]);
            }
            xEventGroupSetBits(wifi_event_group, ESPTOUCH_DONE_BIT);
            break;
        default:
            break;
    }
}

void smartconfig_example_task(void * parm)
{
	tcpip_adapter_ip_info_t 	ip;
	uint8						got_ip_flag = 0;
	uint32						runtime = portMAX_DELAY;;
    EventBits_t uxBits;
    while (1) {
        uxBits = xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT | ESPTOUCH_DONE_BIT, true, false, runtime);
        if(uxBits & CONNECTED_BIT) {
            ESP_LOGI(TAG, "WiFi Connected to ap");
            airkiss_state = SC_STATUS_LINK_OVER+1;
            runtime = 0;
        }
        if(uxBits & START_PROCESS_BIT)
		{
        	runtime = (60*1000)/portTICK_RATE_MS;
		}
        if(uxBits & ESPTOUCH_DONE_BIT) {
            ESP_LOGI(TAG, "smartconfig over");
            pdo_req_send_device_annouce(NULL);
            hal_led_blink(0x0FFFFFFF,1000,1000);
            esp_smartconfig_stop();
            runtime = (60*1000)/portTICK_RATE_MS;
        }
        else
        {
        	runtime = (60*1000)/portTICK_RATE_MS;
			switch (airkiss_state)
			{
				case SC_STATUS_WAIT:
				case SC_STATUS_FIND_CHANNEL:
				case SC_STATUS_GETTING_SSID_PSWD:
				case SC_STATUS_LINK:
				case SC_STATUS_LINK_OVER:
					esp_smartconfig_stop();
					ESP_ERROR_CHECK( esp_smartconfig_start(sc_callback) );
					break;
				case (SC_STATUS_LINK_OVER+1):
					if(tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip)==ESP_OK)
					{
						osLogI(1, "ETH-IP: %d.%d.%d.%d ", IP2STR(&ip.ip));
						osLogI(1, "ETH-MASK: %d.%d.%d.%d  ", IP2STR(&ip.netmask));
						osLogI(1, "ETH-GW: %d.%d.%d.%d \r\n", IP2STR(&ip.gw));
						if((ip.ip.addr!=0))
						{
							got_ip_flag = 0x01;
						}
						else if(got_ip_flag)
						{
							esp_wifi_connect();
						}
					}
					break;
				default:
					break;
			}
        }
    }
}

