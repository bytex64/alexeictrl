#include <stdio.h>
#include "protocol.h"
#include "debug.h"

int main() {
	botstate *bsptr;
	botpacket *bpptr;

	// Let's convert a botstate to a botpacket
	printf("Testing conversion from botstate to botpacket...\n");

	printf("Allocating botstate: %d bytes\n", sizeof(botstate));
	bsptr = (botstate *) malloc(sizeof(botstate));

	printf("Allocating botpacket: %d bytes\n", sizeof(botpacket));
	bpptr = (botpacket *) malloc(sizeof(botpacket));

	printf("Loading botstate with info\n");
	bsptr->lTracMotor = 0.7;
	bsptr->rTracMotor = -0.4;
	bsptr->arm = 0;
	bsptr->grabOrFire = 1;
	bsptr->dump = 0;
	bsptr->weaponArm = 0;

	print_botstate(bsptr);

	printf("Converting botstate to botpacket\n");
	botstate_to_packet(bsptr,bpptr);

	print_botpacket(bpptr);

	printf("Deallocating botpacket and botstate\n");
	free(bsptr);
	free(bpptr);


	// Now, the reverse
	printf("Testing conversion from botpacket to botstate...\n");

	printf("Allocating botpacket: %d bytes\n", sizeof(botpacket));
	bpptr = (botpacket *) malloc(sizeof(botpacket));

	printf("Allocating botstate: %d bytes\n", sizeof(botstate));
	bsptr = (botstate *) malloc(sizeof(botstate));

	printf("Loading botpacket with info\n");
	bpptr->lTracMotor = 144;
	bpptr->rTracMotor = 45;
	bpptr->arm = 97;
	bpptr->grabOrFire = 0;
	bpptr->dump = 0;

	print_botpacket(bpptr);

	printf("Converting botpacket to botstate\n");
	botpacket_to_state(bpptr,bsptr);

	print_botstate(bsptr);

	printf("Deallocating botpacket and botstate\n");
	free(bsptr);
	free(bpptr);
	printf("All done.\n");
	return 0;
}
