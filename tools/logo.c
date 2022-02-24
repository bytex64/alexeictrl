#include <stdio.h>
#include <network.h>
#include <protocol.h>

#define DELIM " \t"

float speed;
botstate bs;

void bot_move(int,int);
void bot_stop();

int main(int argc, char *argv[]) {
	char *buf = NULL;
	char *cmd, *arg;
	int bufsize = 80;
	int running = 1;
	int n, c;

	buf = (char *) malloc(bufsize);
	memset(&bs, 0, sizeof(botstate));

	// connect to our robot (assumed to be first argument)
	if (argc < 2) {
		printf("Usage: %s server\n", argv[0]);
		exit(1);
	} else {
		alexei_connect(argv[1]);
	}

	while (running) {
		n = 0;
		do {
			c = getchar();
			if (c == '\n') {
				buf[n] = 0;
				break;
			} else if (c == -1) {
				buf[n] = 0;
				running = 0;
				break;
			}
			buf[n++] = c;
			if (n == bufsize) {
				bufsize *= 2;
				buf = (char *) realloc(buf, bufsize);
			}
		} while (1);

		cmd = strtok(buf, DELIM);
		do {
			if (arg == NULL) break;
			arg = strtok(NULL, DELIM);
			n = atoi(arg);
			if (!strcmp(cmd,"forward")) {
				bot_move(1,1);
				sleep(n);
				bot_stop();
			} else if (!strcmp(cmd, "backward")) {
				bot_move(-1,-1);
				sleep(n);
				bot_stop();
			} else if (!strcmp(cmd, "left")) {
				bot_move(-1,1);
				sleep(n);
				bot_stop();
			} else if (!strcmp(cmd, "right")) {
				bot_move(1,-1);
				sleep(n);
				bot_stop();
			} else if (!strcmp(cmd, "speed")) {
				speed = (float) n / 100.0;
			}
		} while (cmd = strtok(NULL, DELIM));
	}
}

void bot_move(int l, int r) {
	bs.lTracMotor = speed * l;
	bs.rTracMotor = speed * r;
	alexei_send(&bs);
}

void bot_stop() {
	bot_move(0,0);
}
