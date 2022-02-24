#ifndef _NETWORK_H
#define _NETWORK_H

#include "protocol.h"

#define DEFAULTPORT 7865

// return constants
#define AC_CONNECTED		0
#define AC_ALREADY_CONNECTED	1
#define AC_CONNECTION_ERROR	2
#define AC_NOT_CONNECTED	4

// function prototypes
int alexei_connect(const char *server, int port);
int alexei_disconnect();
int alexei_send(botstate *bs);
int alexei_sendto(const char *hostname, int port, botstate *bs);
int alexei_bind(int port);
int alexei_recv(botpacket *bp);
int alexei_packet_waiting();
void alexei_heartbeat();
void *heartbeat(void *);

#endif //_NETWORK_H
