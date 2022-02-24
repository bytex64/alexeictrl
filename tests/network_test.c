#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "network.h"
#include "protocol.h"
#include "conversion.h"

#define SERVER "10.0.0.5"

botstate *bs;
botpacket *bp;

int main(int argc, char *argv[]) {
	int result = 0;

	if (argc == 2 && !strcmp(argv[1],"server")) {
		result = alexei_bind(DEFAULTPORT);
		printf("alexei_bind: %d\n", result);

		printf("allocating botpacket\n");
		bp = (botpacket *) malloc(sizeof(botpacket));
		printf("allocating botstate\n");
		bs = (botstate *) malloc(sizeof(botstate));

		printf("attempting to recieve a single packet...\n");
		result = alexei_recv(bp);
		printf("alexei_recv: %d\n",result);
		print_botpacket(bp);
		printf("converting botpacket to botstate\n");
		botpacket_to_state(bp,bs);
		print_botstate(bs);

		printf("recieving packets, dot for packet, E for error: ");
		fflush(stdout);
		while (1) {
			if (alexei_recv(bp))
				putchar('.');
			else
				putchar('E');
			fflush(stdout);
		}
	} else if (argc == 2 && !strcmp(argv[1],"stress")) {
		printf("sending packets as fast as I can!\n");
		bs = (botstate *) malloc(sizeof(botstate));
		alexei_connect(SERVER,DEFAULTPORT);
		while (1) {
			alexei_send(bs);
		}
	} else if (argc == 4 && !strcmp(argv[1],"motor")) {
		printf("Setting motor values\n");
		bs = (botstate *) malloc(sizeof(botstate));
		memset(bs,0,sizeof(botstate));
		alexei_connect(SERVER,DEFAULTPORT);
		bs->lTracMotor = ((float) atoi(argv[2])) / 100.0;
		bs->rTracMotor = ((float) atoi(argv[3])) / 100.0;
		print_botstate(bs);

		alexei_send(bs);
		alexei_disconnect();
	} else if (argc == 2 && !strcmp(argv[1],"fader")) {
		struct timespec delay;
		printf("Doing fader\n");

		bs = (botstate *) malloc(sizeof(botstate));
		memset(bs, 0, sizeof(botstate));
		alexei_connect(SERVER,DEFAULTPORT);

		delay.tv_sec = 0;
		delay.tv_nsec = 10000000;

		while (1) {
			for (bs->lTracMotor=-1.0; bs->lTracMotor <= 1.0; bs->lTracMotor += 0.05) {
				bs->rTracMotor = bs->lTracMotor;
				alexei_send(bs);
				nanosleep(&delay,NULL);
			}
		}
	} else if (argc == 2 && !strcmp(argv[1],"deadman")) {
		alexei_connect(SERVER,DEFAULTPORT);

		bs = (botstate *) malloc(sizeof(botstate));
		bs->lTracMotor = 0.7;
		bs->rTracMotor = -0.33;
		bs->grabRotate = 0;
		bs->grabOrFire = 1;
		bs->dump = 0;
		bs->weaponArm = 0;
		
		alexei_heartbeat();
		printf("Kickstarted my heart\n");
		alexei_send(bs);
		printf("Sent a packet. Waiting 15 seconds.\n");
		sleep(15);

		alexei_disconnect();
	} else {

		result = alexei_connect(SERVER,DEFAULTPORT);
		printf("alexei_connect: %d\n",result);

		printf("allocating botstate\n");
		bs = (botstate *) malloc(sizeof(botstate));
		bs->lTracMotor = 0.7;
		bs->rTracMotor = -0.33;
		bs->grabRotate = 0;
		bs->grabOrFire = 1;
		bs->dump = 0;
		bs->weaponArm = 0;
		print_botstate(bs);
		
		printf("sending botstate\n");
		result = alexei_send(bs);
		printf("alexei_send: %d\n",result);

		result = alexei_disconnect();
		printf("alexei_disconnect: %d\n",result);
	}
	return 0;
}
