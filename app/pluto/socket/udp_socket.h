/*
 * mac_socket.h
 *
 *  Created on: Jul 22, 2016
 *      Author: lort
 */

#ifndef UDP_SOCKET_H_
#define UDP_SOCKET_H_

#include "osCore.h"

#define defUdpSocketBufferSize	1500

/****************************************************************
 * port: the udp listen port
 * input_cb : the callback funtion of received message
 * return: 0 is succeed, other's faild
 */
int udp_socket_init(uint16 port);
int udp_socket_deinit(void);
/********************************************************************
 * ip: data to be send target
 * port:data to be send target
 * pdata :the data to be send
 * len : the length of the data to be send
 * return: 0 is succeed, other's faild
 */
int udp_socket_send(uint32 ip, uint16 port,uint8 *pdata, uint16 len);

#endif /* LSL_UDP_SOCKET_H_ */
