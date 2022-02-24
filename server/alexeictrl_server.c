#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sched.h>
#include <errno.h>
#include <sys/time.h>

#include "protocol.h"
#include "network.h"

#include "alexeictrl_server.h"
#include "parport.h"

// For 400Hz, each cycle is 2.5ms (2500 us)
#define CYCLE		2500

// These are on the data byte (output)
#define LMOTOR		0x01
#define LREV		0x02
#define RMOTOR		0x04
#define RREV		0x08
#define ARMENABLE	0x10
#define ARMREV		0x20
#define GRENABLE	0x40
#define GRREV		0x80

// These are on the control byte (output again)
#define GRABORFIRE	0x01	// INVERTED
#define WEAPONARM	0x02	// INVERTED
#define DUMP		0x04

// These are on the status byte (input)
#define TEMPERATURE	0x04

// These are just constants so I don't have to hold out my hand like this:
//
//      ()
//      ||
//      '`^^^)
//   @~~     |
//    ~~\   /
//
// in order to figure out which way is left. :)
#define LEFT		0
#define RIGHT		1

struct timeval t1,t2;
botstate *bs;
botpacket *bp;
uint8_t databyte;
uint8_t controlbyte;
uint8_t statusbyte;
#define PWM_CHANNELS 3
pwm_output *pwm_times[PWM_CHANNELS];

int main() {
	pthread_t pwm_thread;
	pthread_attr_t rt_attr;
	struct sched_param sp;

	printf("alexeictrl_server starting\n");

	// first things first
	bs = (botstate *) malloc(sizeof(botstate));
	bp = (botpacket *) malloc(sizeof(botpacket));
	databyte = controlbyte = statusbyte = 0;

#ifdef DEBUG
	//fprintf(stderr, "Setting realtime priority...\n");
#endif
	// Set up realtime scheduling
	/* memset(&sp, 0, sizeof(struct sched_param));
	sp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	if (sched_setscheduler(0, SCHED_FIFO, &sp)) {
		perror("sched_setscheduler");
	} */

#ifdef DEBUG
	fprintf(stderr, "Binding to socket...\n");
#endif
	alexei_bind(DEFAULTPORT);

#ifndef NOPP
	if (parport_init())
		exit(1);
#endif

#ifdef DEBUG
	fprintf(stderr, "Starting PWM...\n");
#endif
	control_loop(NULL);
}

inline void busymark() {
	gettimeofday(&t1, NULL);
}

inline void busywait_until(int us) {
	int diff;

	do {
		// WHAT THE HELL WAS THIS FOR?
		for (diff = 0; diff < 1000; diff++);
		gettimeofday(&t2, NULL);
		diff = (t2.tv_sec - t1.tv_sec) ? t2.tv_usec - t1.tv_usec + 1000000 : t2.tv_usec - t1.tv_usec;
	} while (diff < us);
}

// be warned, busywait only works for intervals less than a second.
inline void busywait(int us) {
	int diff;

	if (!us) return;

	gettimeofday(&t1, NULL);
	do {
		for (diff = 0; diff < 1000; diff++);
		gettimeofday(&t2, NULL);
		diff = (t2.tv_sec - t1.tv_sec) ? t2.tv_usec - t1.tv_usec + 1000000 : t2.tv_usec - t1.tv_usec;
	} while (diff < us);
}

void *control_loop(void *data) {
	int n;
	int temp_status = 0;
	int temp_up, temp_down;
	struct timeval tv1, tv2;
	double temperature;
	parport_map output;
	pwm_output lmotor = {0, DATA0};
	pwm_output rmotor = {0, DATA2};
	pwm_output amotor = {0, DATA4};
	pwm_times[0] = &lmotor;
	pwm_times[1] = &rmotor;
	pwm_times[2] = &amotor;

	while(1) {
		// do PWM via the "bus stop" algorithm.
		parport_map_clear(output);
		for (n = 0; n < PWM_CHANNELS; n++)
			if (pwm_times[n]->time > 0)
				parport_map_set(output, pwm_times[n]->pwm);
		parport_out(output);
		busymark();
		for (n = 0; n < PWM_CHANNELS; n++) {
			if (pwm_times[n]->time == 0) continue;
			busywait_until(pwm_times[n]->time);
			//printf("at %d\n", t2.tv_usec - t1.tv_usec);
			parport_map_off(output, pwm_times[n]->pwm);
			parport_out(output);
		}

		/* busywait(down);
		pwm_up(LMOTOR|RMOTOR);
		busywait(up1);
		pwm_down(bias);
		busywait(up2);
		pwm_down(LMOTOR|RMOTOR); */

		// read sensors
		//read_status();
		// Our temperature sensor by varying the length of a
		// square wave. It's like our PWM, but it varies the
		// down time instead of the duty cycle. The temperature
		// in celsius is (time up/time down) * 400.
		if (statusbyte & TEMPERATURE) {
			if (temp_status == 0) {
				gettimeofday(&tv2, NULL);
				temp_down = tv1.tv_usec - tv2.tv_usec;
			}
			temp_status = 1;
		} else {
			if (temp_status) {
				gettimeofday(&tv1, NULL);
				temp_up = tv2.tv_usec - tv1.tv_usec;
				// Calculate the temp
				temperature = temp_up / temp_down * 400.0;
#ifdef DEBUG
				printf("Temperature: %f\n", temperature);
#endif
			}
			temp_status = 0;
		}

		if (alexei_packet_waiting()) {
			alexei_recv(bp);
			print_botpacket(bp);
			botpacket_to_state(bp,bs);
#ifdef DEBUG
			print_botstate(bs);
#endif
			lmotor.time = CYCLE * (bs->lTracMotor >= 0.0 ? bs->lTracMotor : -bs->lTracMotor);
			rmotor.time = CYCLE * (bs->rTracMotor >= 0.0 ? bs->rTracMotor : -bs->rTracMotor);
			amotor.time = CYCLE * (bs->arm >= 0.0 ? bs->arm : -bs->arm);
			qsort(pwm_times, PWM_CHANNELS, sizeof(pwm_output *), pwm_sorter);
			/* old PWM code
			if (ltime_up > rtime_up) {
				bias = RMOTOR;
				up1 = rtime_up;
				up2 = ltime_up - rtime_up;
				down = CYCLE - ltime_up;
			} else {
				bias = LMOTOR;
				up1 = ltime_up;
				up2 = rtime_up - ltime_up;
				down = CYCLE - rtime_up;
			}
			printf("bias: %d up1: %d up2: %d down: %d\n",bias, up1, up2, down); */

			databyte = 0;
			// GRABORFIRE and WEAPONARM are inverted
			controlbyte = GRABORFIRE | WEAPONARM;
			if (bs->lTracMotor < 0.0)
				databyte = databyte | LREV;
			if (bs->rTracMotor < 0.0)
				databyte = databyte | RREV;
			if (bs->arm) {
				databyte = databyte | ARMENABLE;
				if (bs->arm == -1)
					databyte = databyte | ARMREV;
			}
			if (bs->grabRotate) {
				databyte = databyte | GRENABLE;
				if (bs->grabRotate = -1)
					databyte = databyte | GRREV;
			}
			if (bs->grabOrFire)
				// GRABORFIRE is inverted logic
				controlbyte = controlbyte & ~GRABORFIRE;
			if (bs->weaponArm)
				// WEAPONARM is inverted logic
				controlbyte = controlbyte & ~WEAPONARM;
			if (bs->dump)
				controlbyte = controlbyte | DUMP;
			//outb(controlbyte, PPCONTROL);
		}
		// Wait the remainder of the time of our cycle. I put
		// this after the above, so the network stuff will get
		// "absorbed" into the down-time.
		busywait_until(CYCLE);
	}
}

int pwm_sorter(const void *v1, const void *v2) {
	pwm_output *t1, *t2;
	t1 = (pwm_output *) v1;
	t2 = (pwm_output *) v2;
	if (t1->time < t2->time)
		return -1;
	else if (t1->time > t2->time)
		return 1;
	else
		return 0;
}
