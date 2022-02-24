#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include <stdint.h>

typedef struct _botstate {
	// these are for the left and right traction motors, and have a
	// value from -1.0 to 1.0, inclusive.
	float lTracMotor, rTracMotor;
	// arm specifies the direction of the arm, either -1, 0, or 1
	float arm;
	// grabRotate rotates the grabber. Either -1, 0, or 1
	int grabRotate;
	// grabOrFire activates the grabber, or fires the weapon, depending
	// on what's mounted. weaponArm must also be 1 before this will work.
	// It's either 1 (closed) or 0 (open).
	int grabOrFire;
	// dump activates the dumping mechanism. Normally 0. 1 activates.
	int dump;
	// weaponArm arms the weapon. Normally 0. 1 when armed.
	int weaponArm;
} botstate;

typedef struct _botpacket {
	// botpacket is the network representation of the botstate. It
	// contains the same information, condensed into a 96-bit
	// packet, with sequence number and checksum.
	uint32_t
		lTracMotor:8, rTracMotor:8,
		arm:8, grabRotate:2,
		grabOrFire:1, dump:1, :4;
	uint32_t seqnum;
	uint32_t checksum;
} botpacket;

typedef struct _sensorpacket {
	// sensorpacket reads back any sensor on the robot. For us,
	// that's only temperature
	float temp;
} sensorpacket;

#endif // _PROTOCOL_H
