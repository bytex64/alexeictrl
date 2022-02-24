/*  alexeictrl_client.c
 *
 *  AlexeiBOT Control Client
 *  Version 0.7.0, (c) 2003 Matt Erickson and Chip Black
 *  
 *  Joystick input functions borrow heavily from jstest.c by Vojtech Pavlik
 *
 *  Network functionality has been added and works.
 *  
 *  Currently brought up to libalexeictrl standards.  Streamlining of the code will
 *  be accomplished soon (for 0.7.1).
 */

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <linux/joystick.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

#include "protocol.h"
#include "conversion.h"
#include "network.h"
#include "debug.h"
#include "disp.h"

#define VER 0x000700

#define NAME_LENGTH 128
#define NONET 0
#define NODISP 1
#define NORMAL 2

botstate botstruct;
int mode, result;

void modeSet(char *chmode[]) {

    if (!strcmp(*chmode, "nonet"))
	mode = NONET;
    else if (!strcmp(*chmode, "nodisp"))
	mode = NODISP;
    else if (!strcmp(*chmode, "normal"))
	mode = NORMAL;
}

int joystart(int *fd, char *jsdevice[], unsigned char *numaxes, unsigned char *numbuttons) {

/*
 *  Initializes the joystick, and grabs basic perameters from the joystick.
 */    
    int version = 0x000800;
    char name[NAME_LENGTH] = "Unknown";

    if (((*fd) = open(*jsdevice, O_RDONLY, O_NONBLOCK)) < 0) {
	if (mode != NODISP)
	    endwin();
	perror("\nAlexeiCTRL Error: Joystick device could not open properly.");
	return (-1);
    }

    ioctl(*fd, JSIOCGVERSION, &version);
    ioctl(*fd, JSIOCGAXES, &(*numaxes));
    ioctl(*fd, JSIOCGBUTTONS, &(*numbuttons));
    ioctl(*fd, JSIOCGNAME(NAME_LENGTH), name);

/*
 *  Displays peramaters reported from joystick
 */

    if (mode != NODISP) {
	dispJsStart(jsdevice, &name, &version, numaxes, numbuttons);
    }

    return 0;
};

int input(int fd, struct js_event *js, int axes, int buttons, int *axis, char *button) {
/*
 *  Reads data from the joystick, and puts the data in two arrays.
 */

    if(read(fd, &(*js), sizeof(struct js_event)) != sizeof(struct js_event)) {
	if (mode != NODISP)
            endwin();
	perror("\nAlexeiCTRL Error: Could not read joystick device.");
	return(-1);
    }

    switch((*js).type & ~JS_EVENT_INIT) {
	case JS_EVENT_BUTTON:
	    button[((*js).number)] = (*js).value;
	    break;
	case JS_EVENT_AXIS:
	    axis[(*js).number] = (*js).value;
	    break;
    }
    return(1);
};

void axisconvert(int axes, int *axis, float *humanaxis) {
/*
 *  Converts the raw axis data into percentages, for my own sanity.  "Sanity?  What sanity???"
 */

    int i;

    for (i = 0; i < axes; i++)
	humanaxis[i] = (-1) * ((float)axis[i] / 32767) * 100;
};

void makecmd(float *humanaxis, char *button) {
/*
 *  Creates a struct that holds all the information anyone could ever want!  Yay!
 */
    int tmplmotor, tmprmotor;

    tmplmotor = humanaxis[1] - humanaxis[0];
    tmprmotor = humanaxis[1] + humanaxis[0];

    if (tmplmotor > 100)
	tmplmotor = 100;
    else if (tmplmotor < -100)
	tmplmotor = -100;

    if (tmprmotor > 100)
	tmprmotor = 100;
    else if (tmprmotor < -100)
	tmprmotor = -100;

    botstruct.lTracMotor = tmplmotor;
    botstruct.rTracMotor = tmprmotor;

    if (humanaxis[5] > 0)
	botstruct.arm = 1;
    else if (humanaxis[5] < 0)
	botstruct.arm = -1;
    else
	botstruct.arm = 0;

    botstruct.grabOrFire = button[0];

    if (humanaxis[4] > 0)
	botstruct.grabRotate = -1;
    else if (humanaxis[4] < 0)
	botstruct.grabRotate = 1;
    else
	botstruct.grabRotate = 0;

    botstruct.dump = button[6];
    botstruct.weaponArm = button[7];
};

int runBot(int fd, int axes, int buttons) {
/*
 *  Does most of the robot running.
 */
    float *humanaxis;
    int i, *axis;
    char *button;

    struct js_event js;
    
    axis = calloc(axes, sizeof(int));
    button = calloc(buttons, sizeof(char));
    humanaxis = calloc(axes, sizeof(float));

    while(getch() != 'q') {

	if((input(fd, &js, axes, buttons, axis, button)) < 0) {
	    if (mode != NODISP)
	        endwin();
	    perror("\nAlexeiCTRL Fatal Error");
	    exit(1);
	}

	axisconvert(axes, axis, humanaxis);
	makecmd(humanaxis, button);
	
	if (mode != NONET)
	    result = alexei_send(&botstruct);
	
	if (mode != NODISP) {
		jsDispBot(&botstruct, axes, buttons, axis, button);
		refresh();
	}
    }

    return (0);
}

int main(int argc, char *argv[]){

    int fd;
    unsigned char axes = 2;
    unsigned char buttons = 2;

    if (argc != 3) {
	printf("AlexeiBOT Control Client Version 0.7\n");
	printf("Useage: alexeictrl_client <device> <mode>\n");
	printf("<device> is the name of the joystick port\n");
	printf("<mode> can be:\n");
	printf("	'nonet'- Network-less joystick test mode\n");
	printf("	'nodisp'- Display-less network test mode\n");
	printf("	'normal'- Normal display & network enabled mode\n");
	exit(1);
      }

    modeSet(&argv[2]);

    if (mode != NODISP)   
	startDisp(CLIENT);

    if(joystart(&fd, &argv[1],&axes, &buttons) < 0) {
	if (mode != NODISP)
	    endwin();
	perror("\nAlexeiCTRL Fatal Error.  Program exit.");
	exit(1);
    }

    if (mode != NODISP) {
	printw("AlexeiCTRL Event: Joystick device open.  Attempting device read.\n");
    	printw("                  Press 'q' to exit.\n");
    }

    if (mode != NONET)
	result = alexei_connect();

    runBot(fd, axes, buttons);

    if (mode != NONET)
	result = alexei_disconnect();

    if(mode != NODISP)
	endwin();

    return 0;
}
