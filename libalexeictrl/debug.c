#include "protocol.h"

void print_botstate(botstate *bs) {
	printf("***DEBUG Botstate:\n");
	printf("lTracMotor: %f\trTracMotor: %f\n",bs->lTracMotor,bs->rTracMotor);
	printf("arm: %f\tgrabRotate: %d\tgrabOrFire: %d\tdump: %d\n",
			bs->arm, bs->grabRotate, bs->grabOrFire, bs->dump);
	printf("weaponArm: %d\n",
			bs->weaponArm);
	printf("***\n");
}

void print_botpacket(botpacket *bp) {
	printf("***DEBUG Botpacket:\n");
	printf("lTracMotor: %d\trTracMotor: %d\n",bp->lTracMotor,bp->rTracMotor);
	printf("arm: %d\tgrabRotate: %d\tgrabOrFire: %d\tdump: %d\n",
			bp->arm, bp->grabRotate, bp->grabOrFire, bp->dump);
	printf("hex dump: %08X\n",(*bp));
	printf("***\n");
}
