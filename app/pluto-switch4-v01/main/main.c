/* ethernet Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include <esp_ota_ops.h>
#include "external_include.h"
#include "osCore.h"
#include "app_task.h"
#include "hal_led.h"
#include "hal_key.h"
#include "pluto_nvs.h"
#include "pluto.h"
#include "pluto_file_system.h"

#define DEFAULT_ETHERNET_PHY_CONFIG phy_lan8720_default_ethernet_config
#define PIN_SMI_MDC   5
#define PIN_SMI_MDIO  18

#define DBG_START 1

static void init_task(int sig, void *arg, int len);
static void eth_gpio_config_rmii(void);
static void eth_enable_clk(void);
static esp_err_t eth_init(void);

void app_main()
{
	const esp_partition_t *pboot = esp_ota_get_boot_partition();
	const esp_partition_t *prun = esp_ota_get_running_partition();
	osLogI(DBG_UPGRADE, "app_main boot partition		:type:%08x subtype:%08x run lable:%s addr:%08x size:%08x \r\n",
			pboot->type,pboot->subtype,pboot->label,pboot->address,pboot->size);
	osLogI(DBG_UPGRADE, "app_main runnung partition	:type:%08x subtype:%08x run lable:%s addr:%08x size:%08x \r\n",
			prun->type,prun->subtype,prun->label,prun->address,prun->size);
	osInit();
	osStartReloadTimer(1000,init_task,1,NULL);
}
extern esp_err_t initialise_wifi(void);
static void init_task(int sig, void *arg, int len)
{
	tcpip_adapter_ip_info_t 	ip;
	static  uint8				init_flag = 0;
	pluto_nvs_init();
	osLogI(0,"test_mem: free heap: %d\r\n",esp_get_free_heap_size());
	if(init_flag==0x00)
	{
		init_flag = 0x01;
		tcpip_adapter_init();
		ESP_ERROR_CHECK(esp_event_loop_init(NULL, NULL));
		if(initialise_wifi()==ESP_OK)
			app_task_init(osTrue);
		else
			app_task_init(osFalse);
	}
	else
	{
       /* if((tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA, &ip)==ESP_OK)){
            osLogI(DBG_START, "ETH-IP: %d.%d.%d.%d ", IP2STR(&ip.ip));
            osLogI(DBG_START, "ETH-MASK: %d.%d.%d.%d  ", IP2STR(&ip.netmask));
            osLogI(DBG_START, "ETH-GW: %d.%d.%d.%d \r\n", IP2STR(&ip.gw));
            osStartReloadTimer(60000,init_task,1,NULL);
        }*/
	}
}
static void eth_gpio_config_rmii(void)
{
    // RMII data pins are fixed:
    // TXD0 = GPIO19
    // TXD1 = GPIO22
    // TX_EN = GPIO21
    // RXD0 = GPIO25
    // RXD1 = GPIO26
    // CLK == GPIO0
    phy_rmii_configure_data_interface_pins();
    // MDC is GPIO 23, MDIO is GPIO 18
    phy_rmii_smi_configure_pins(PIN_SMI_MDC, PIN_SMI_MDIO);
}
#define GPIO_CLKEN_NUM 	2
#define GPIO_CLKEN_MASK	(1<<GPIO_CLKEN_NUM)
static void eth_enable_clk(void)
{
    gpio_config_t io_conf;
    memset(&io_conf,0,sizeof(io_conf));
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_CLKEN_MASK;
    gpio_config(&io_conf);
	gpio_set_level(GPIO_CLKEN_NUM,1);
}
static esp_err_t eth_init(void)
{
	eth_config_t 	config = DEFAULT_ETHERNET_PHY_CONFIG;
	esp_err_t 		ret = ESP_FAIL;
    /* Set the PHY address in the example configuration */
	eth_enable_clk();
    config.phy_addr = 0x00;
    config.gpio_config = eth_gpio_config_rmii;
    config.tcpip_input = tcpip_adapter_eth_input;
    ret = esp_eth_init(&config);
    if(ret == ESP_OK) {
       ret =  esp_eth_enable();
    }
    return ret;
}
