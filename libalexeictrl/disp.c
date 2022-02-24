#include <ncurses.h>
#include <string.h>
#include <errno.h>

#include "protocol.h"
#include "disp.h"

#define CLIENT 0
#define SERVER 1

#define MSG_SIZE 128

void startDisp(int prog) {

    initscr();
    nodelay(stdscr, TRUE);
    noecho();
    keypad(stdscr, TRUE);

    printw("AlexeiBOT Control ");
    if (prog == CLIENT)
	printw("Client");
    else if (prog == SERVER)
	printw("Server");
    /* printw(" version %s\n", VERSION); */
}

void dispJsStart(char *jsdevice[], char *jsname, int *jsversion, int *numaxes, int *numbuttons) {

	printw("AlexeiCTRL Event: Joystick device opened properly.\n");
	printw("AlexeiCTRL Event: Joystick device returns following peramaters:\n\n");
	printw("     device name:    %s\n", *jsdevice);
	printw("     joystick name:  %s\n", *jsname);
	printw("     driver version: %d.%d.%d\n", *jsversion >> 16, (*jsversion >> 8) & 0xff, *jsversion & 0xff);
	printw("     axes:           %d\n", *numaxes);
	printw("     buttons:        %d\n\n", *numbuttons);
}

void dispBot(botstate *botstruct) {

    printw("\r");
    printw("L Traction Motor: %4d%% | R Traction Motor: %4d%%\n", (int)(*botstruct).lTracMotor, (int)(*botstruct).rTracMotor);
    printw("Arm movement: %2d  |  Grabber Status: %s  |  Grabber Rotation: %2d\n", (*botstruct).arm, (*botstruct).grabOrFire ? "CLOSED" : "OPEN  ", (*botstruct).grabRotate);
    printw("Dumper: %d  |  Weapon Arm Status: %s\n", (*botstruct).dump, (*botstruct).weaponArm ? ">>>> ARMED <<<<" : "---- SAFED ----");
    printw("Weapon Status: %s\n", (*botstruct).grabOrFire ? ">>>> ACTIVATED <<<<" : "---- STOPPED ----");
};

void jsDispBot(botstate *botstruct, int axes, int buttons, int *axis, char *button) {

    int i;

    move(14,0);

    refresh();

    printw("\r");
    printw("L Traction Motor: %4d%% | R Traction Motor: %4d%%\n", (int)(*botstruct).lTracMotor, (int)(*botstruct).rTracMotor);
    printw("Arm movement: %2d  |  Grabber Status: %s  |  Grabber Rotation: %2d\n", (*botstruct).arm, (*botstruct).grabOrFire ? "CLOSED" : "OPEN  ", (*botstruct).grabRotate);
    printw("Dumper: %d  |  Weapon Arm Status: %s\n", (*botstruct).dump, (*botstruct).weaponArm ? ">>>> ARMED <<<<" : "---- SAFED ----");
    printw("Weapon Status: %s\n", (*botstruct).grabOrFire ? ">>>> ACTIVATED <<<<" : "---- STOPPED ----");
    printw("\nController Status:\n");
    printw("Axes: ");
    for(i = 0; i < axes; i++)
	printw("%2d: %6d ", i, axis[i]);
    printw("\nButtons: ");
    for(i = 0; i < buttons; i++)
	printw("%2d: %d  ", i, button[i]);
};
