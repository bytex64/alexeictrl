#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include "network.h"
#include "protocol.h"
#include "conversion.h"

#define calctime(tv) ((float) tv.tv_sec + (float) tv.tv_usec * 0.000001)

int alexei_sock = 0;
int our_port;
pthread_t heartbeat_thread;
int heartbeat_signal = 0;
struct timeval lastpackettime, now;
botpacket lastpacket;

int alexei_connect(const char *server, int port) {
	struct sockaddr_in sa;
	struct hostent *host;

	if (alexei_sock) {
#ifdef DEBUG
		fprintf(stderr, "alexei_connect: already connected\n");
#endif
		return AC_ALREADY_CONNECTED;
	} else {
		memset(&sa, 0, sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);

		host = (struct hostent *) gethostbyname(server);
		if (host == NULL)
			return AC_CONNECTION_ERROR;

		sa.sin_addr = *((struct in_addr *) host->h_addr_list[0]);

		alexei_sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (alexei_sock < 0)
			return AC_CONNECTION_ERROR;
		if (connect(alexei_sock, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
			return AC_CONNECTION_ERROR;
		} else {
			return AC_CONNECTED;
		}
	}
}

// The server doesn't need this threaded junk
#ifndef SERVER
void alexei_heartbeat() {
	heartbeat_signal = 1;
	pthread_create(&heartbeat_thread, NULL, heartbeat, NULL);
}

void kill_heartbeat() {
	heartbeat_signal = 0;
	pthread_join(heartbeat_thread, NULL);
}

void *heartbeat(void *data) {
	struct timespec t;
	t.tv_sec = 0;
	t.tv_nsec = 1e8;

	while (heartbeat_signal) {
		nanosleep(&t, NULL);
		gettimeofday(&now, NULL);
		if (calctime(now) - calctime(lastpackettime) > 0.1) {
			alexei_sendpacket(&lastpacket);
		}
	}
}
#endif

int alexei_disconnect() {
#ifndef SERVER
	kill_heartbeat();
#endif
	shutdown(alexei_sock,SHUT_RDWR);
	alexei_sock = 0;
	return 0;
}

int alexei_send(botstate *bs) {
	botpacket *bp;
	int ret;

	if (!alexei_sock)
		return AC_NOT_CONNECTED;

	bp = (botpacket *) malloc(sizeof(botpacket));
	botstate_to_packet(bs,bp);

	ret = alexei_sendpacket(bp);
	free(bp);
	return ret;
}

int alexei_sendpacket(botpacket *bp) {
	ssize_t numbytes;

	numbytes = send(alexei_sock, bp, sizeof(botpacket), 0);
	gettimeofday(&lastpackettime, NULL);
	lastpacket = *bp;
#ifdef DEBUG
	printf("alexei_sendpacket: sent %d bytes\n", numbytes);
#endif
	if (numbytes != sizeof(botpacket))
		return 0;

	return 1;
}

int alexei_sendto(const char *hostname, int port, botstate *bs) {
	struct hostent *host;
	struct sockaddr_in sa;
	botpacket *bp;
	int numbytes;

	bp = (botpacket *) malloc(sizeof(botpacket));
	botstate_to_packet(bs,bp);

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	host = (struct hostent *) gethostbyname(hostname);
	if (host == NULL)
		return AC_CONNECTION_ERROR;

	sa.sin_addr = *((struct in_addr *) host->h_addr_list[0]);

	numbytes = sendto(alexei_sock, bp, sizeof(botpacket), 0, (struct sockaddr *) &sa, sizeof(sa));

	free(bp);
	if (numbytes != sizeof(botpacket))
		return AC_CONNECTION_ERROR;
	return 0;
}

int alexei_bind(int port) {
	struct sockaddr_in sa;

	if (alexei_sock) {
#ifdef DEBUG
		fprintf(stderr, "alexei_bind: already connected\n");
#endif
		return AC_ALREADY_CONNECTED;
	}

	alexei_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (alexei_sock < 0)
		return AC_CONNECTION_ERROR;

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = INADDR_ANY;

	if (bind(alexei_sock, (struct sockaddr *) &sa, sizeof(sa)))
		return AC_CONNECTION_ERROR;

	our_port = port;
	return AC_CONNECTED;
}

int alexei_recv(botpacket *bp) {
	ssize_t bytesrecvd;

	if (!alexei_sock)
		return AC_NOT_CONNECTED;

	bytesrecvd = recv(alexei_sock, bp, sizeof(botpacket), MSG_WAITALL);
	gettimeofday(&lastpackettime, NULL);

	if (bytesrecvd != sizeof(botpacket))
		return 0;

	return 1;
}

int alexei_sensor_recv(sensorpacket *sp) {
	ssize_t bytesrecvd;

	if (!alexei_sock)
		return AC_NOT_CONNECTED;

	bytesrecvd = recv(alexei_sock, sp, sizeof(sensorpacket), MSG_WAITALL);

	if (bytesrecvd != sizeof(sensorpacket))
		return 0;

	return 1;
}

int alexei_sensor_send(sensorpacket *sp) {
	ssize_t numbytes;

	if (!alexei_sock)
		return AC_NOT_CONNECTED;

	numbytes = send(alexei_sock, sp, sizeof(sensorpacket), 0);
#ifdef DEBUG
	printf("alexei_send: sent %d bytes\n", numbytes);
#endif
	if (numbytes != sizeof(botpacket))
		return 0;

	return 1;
}

int alexei_packet_waiting() {
	fd_set fds;
	struct timeval timeout;

	gettimeofday(&now, NULL);
	if (calctime(now) - calctime(lastpackettime) > 0.2) {
		// Something is borked. STOP!
		botstate bs;

		fprintf(stderr, "Something is borked\n");

		memset(&bs, 0, sizeof(bs));

		alexei_sendto("127.0.0.1",our_port, &bs);
		
		gettimeofday(&lastpackettime, NULL);
		return 1;
	}

	memset(&timeout, 0, sizeof(struct timeval));
	FD_ZERO(&fds);
	FD_SET(alexei_sock, &fds);
	select(alexei_sock + 1, &fds, NULL, NULL, &timeout);
	if (FD_ISSET(alexei_sock, &fds))
		return 1;
	else
		return 0;
}
