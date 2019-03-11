/*
 * tcp_socket.h
 *
 *  Created on: 2017Äê5ÔÂ4ÈÕ
 *      Author: lort
 */

#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#include "osCore.h"
#define defTcpSocketBufferSize	1500

int tcp_socket_init(uint32 ip, uint16 port);
osState tcp_socket_deinit(void);
int tcp_socket_send(uint8 *pdata, uint16 len);

#endif /* USER_TCP_SOCKET_H_ */
