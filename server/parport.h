#ifndef _PARPORT_H
#define _PARPORT_H

#include <stdio.h>
#include <stdint.h>

// We're down with SPP.
//  (yeah, you know me!)
// We're down with SPP.
//  (yeah, you know me!)
// We're down with SPP.
//  (yeah, you know me!)
// Who's down with SPP?
//  (every last homie!)
#define PPDATA		0x3BC
#define PPSTATUS	0x3BD
#define PPCONTROL	0x3BE

// data!
// The parport map is a uint16_t, because we (currently) have only the
// data port and control port to play with. Two bytes.
typedef uint16_t parport_map;

// Handy defines
#define CONTROL_INVERT_MASK 0x0B
#define parport_map_clear(p) (p = 0)
#define parport_map_set(p, x) (p = x)
#define parport_map_on(p, x) (p = p | x)
#define parport_map_off(p, x) (p = p & ~(x))
#define parport_map_flip(p, x) (p = p ^ x)
#define parport_map_data(p) (uint8_t) (p & 0x00FF)
#define parport_map_control(p) (uint8_t) (p >> 8)

// parport bits
// The first byte in our parport_map is the data port. It's really
// straightforward
#define DATA0 0x0001
#define DATA1 0x0002
#define DATA2 0x0004
#define DATA3 0x0008
#define DATA4 0x0010
#define DATA5 0x0020
#define DATA6 0x0040
#define DATA7 0x0080
// The second byte is the control port. It's a little more fucked up.
// Note that we specify them both by pin and function.
// Also: There is no CONTROL4-7
#define CONTROL0   0x0100
#define CONTROL1   0x0200
#define CONTROL2   0x0400
#define CONTROL3   0x0800
#define STROBE     0x0100
#define AUTOFEED   0x0200
#define INITIALIZE 0x0400
#define SELECT     0x0800

// Function prototypes
int parport_init();
void parport_out(parport_map);
void print_byte_binary(uint8_t byte);
inline uint8_t read_status();

#endif //_PARPORT_H
