/* disp.h */
#ifndef _disp_h
#define _disp_h

void startDisp(int prog);

void dispJsStart(char *jsdevice[], char *jsname, int *jsversion, int *numaxes, int *numbuttons);

void dispBot(botstate *botstruct); // Will show only botstate data.
void jsDispBot(botstate *botstruct, int axes, int buttons, int *axis, char *button);

#endif
