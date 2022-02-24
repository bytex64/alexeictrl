/* parport_test
 *
 * If this works correctly, the data pins on the parallel port should move back
 * and forth Knight Rider style.
 */

#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <sys/io.h>

#define LEFT -1
#define RIGHT 1

// PPBASE is 0x3BC on my laptop. Also, try 0x3F8.
#define PPBASE 0x3BC

// DELAY is the number of nanoseconds to delay between iterations
#define DELAY 500000000

int main() {
	struct timespec delay;
	uint8_t ppbyte = 0x01;
	int direction = LEFT;
	char c;

	if (ioperm(PPBASE,1,1)) {
		printf("Could not get IO access permission for 0x%x. Are you root?\n",PPBASE);
		exit(1);
	}

	delay.tv_sec = 0;
	delay.tv_nsec = DELAY;

	while (1) {
		c = getchar();
		outb(c, PPBASE);
		nanosleep(&delay, NULL);
	}
}
