/*
 * extern_include.h
 *
 *  Created on: May 30, 2018
 *      Author: lort
 */

#ifndef EXTERNAL_INCLUDE_H_
#define EXTERNAL_INCLUDE_H_

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
#include "freertos/portmacro.h"
#include "freertos/event_groups.h"

#include "stdint.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_attr.h"
#include "esp_eth.h"
#include "esp_smartconfig.h"
#include "esp_event_loop.h"
#include "esp_wps.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_ota_ops.h"

#include "eth_phy/phy_lan8720.h"
#include "tcpip_adapter.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip4.h"
#include "lwip/sockets.h"
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

#include "rom/ets_sys.h"
#include "rom/gpio.h"

#include "soc/dport_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"


#include "wear_levelling.h"
#include "nvs.h"
#include "nvs_flash.h"

#endif /* EXTERNAL_INCLUDE_H_ */
