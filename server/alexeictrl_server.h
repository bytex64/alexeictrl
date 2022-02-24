#ifndef _ALEXEICTRL_SERVER_H
#define _ALEXEICTRL_SERVER_H

#include "parport.h"

// structures
typedef struct {
	int time;
	parport_map pwm;
} pwm_output;

// function prototypes
int main();
inline void busymark();
inline void busywait_until(int us);
inline void busywait(int us);
void *control_loop(void *);
int pwm_sorter(const void *v1, const void *v2);

#endif //_ALEXEICTRL_SERVER_H
