#ifndef _conversion_h
#define _conversion_h

#include "protocol.h"

void botstate_to_packet(botstate *in, botpacket *out);
void botpacket_to_state(botpacket *in, botstate *out);

#endif //_conversion_h
