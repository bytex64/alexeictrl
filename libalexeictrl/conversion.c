#include <stdint.h>
#include <assert.h>

#include "conversion.h"
#include "protocol.h"

void botstate_to_packet(botstate *in, botpacket *out) {
#ifdef DEBUG
	assert(in->arm >= -1 && in->arm <= 1);
	assert(in->grabRotate >= -1 && in->grabRotate <= 1);
	assert(in->grabOrFire == 0 || in->grabOrFire == 1);
	assert(in->dump == 0 || in->dump == 1);
#endif
	out->lTracMotor = ((int) (in->lTracMotor * 126.0)) + 126;
	out->rTracMotor = ((int) (in->rTracMotor * 126.0)) + 126;
	out->arm = ((int) (in->arm * 126.0)) + 126;
	out->grabRotate = in->grabRotate + 1;
	out->grabOrFire = in->grabOrFire;
	out->dump = in->dump;

	// This a stub right now. It could concievably be
	// better placed in network.c... but whatever.
	// FIXME
	out->seqnum = 0;
	out->checksum = 0;
}

void botpacket_to_state(botpacket *in, botstate *out) {
#ifdef DEBUG
	assert(in->arm >= -1 && in->arm <= 1);
	assert(in->grabRotate >= -1 && in->grabRotate <= 1);
	assert(in->grabOrFire == 0 || in->grabOrFire == 1);
	assert(in->dump == 0 || in->dump == 1);
#endif
	out->lTracMotor = ((float) in->lTracMotor) / 126.0 - 1.0;
	out->rTracMotor = ((float) in->rTracMotor) / 126.0 - 1.0;
	out->arm = ((float) in->arm) / 126.0 - 1.0;
	out->grabRotate = in->grabRotate - 1;
	out->grabOrFire = in->grabOrFire;
	out->dump = in->dump;
}
