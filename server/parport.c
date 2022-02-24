#include <sys/io.h>
#include "parport.h"

int parport_init() {
#ifdef DEBUG
	fprintf(stderr, "Getting port access...\n");
#endif
	if (ioperm(PPDATA,1,1)) {
		fprintf(stderr,"Couldn't get IO access. Dying.\n");
		return 1;
	}
	ioperm(PPSTATUS,1,1);
	ioperm(PPCONTROL,1,1);
	return 0;
}

void parport_out(parport_map p) {
#ifdef NOPP
	printf("data: ");
	print_byte_binary(parport_map_data(p));
	printf("  control: ");
	print_byte_binary(parport_map_control(p) ^ CONTROL_INVERT_MASK);
	printf("\n");
#else
	outb(p.data, PPDATA);
	outb(p.control ^ CONTROL_INVERT_MASK, PPCONTROL);
#endif
}

#ifdef NOPP
void print_byte_binary(uint8_t byte) {
	int n;
	char bits[9];

	bits[8] = 0;
	for (n=7; n >= 0; n--) {
		bits[n] = byte & 1 ? '1' : '0';
		byte = byte >> 1;
	}
	printf("%s",bits);
}
#endif

inline uint8_t read_status() {
	return inb(PPSTATUS);
}
