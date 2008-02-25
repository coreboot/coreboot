#include <arch/io.h>

static void write_index(uint16_t port_base, uint8_t reg, uint8_t value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static const struct {
	uint8_t index, value;
} sequence[]= {
	/* Set FAN_CTL control register (0x14) polarity to high, and
	   activate fans 1, 2 and 3. */
	{ 0x14, 0x87},
	/* set the correct sensor types 1,2 thermistor; 3 diode */
	{ 0x51, 0x1c},
	/* set the 'zero' voltage for diode type sensor */
	{ 0x5c, 0x80},
//	{ 0x56, 0xe5},
//	{ 0x57, 0xe5},
	{ 0x59, 0xe5},
	{ 0x5c, 0x00},
	/* fan1 (controlled by temp3) control parameters */
	/* fan off limit */
	{ 0x60, 0xff},
	/* fan start limit */
	{ 0x61, 0x14},
	/* ???? */
//	{ 0x62, 0x00},
	/* start PWM */
	{ 0x63, 0x27},
	/* smooth and slope PWM */
	{ 0x64, 0x90},
	/* direct-down and interval */
	{ 0x65, 0x03},
	/* fan1 auto controlled by temp3 */
	{ 0x15, 0x82},
	/* fan2 soft controlled, max speed */
	{ 0x16, 0x7f},
	/* fan3 soft controlled, 75% speed */
	{ 0x17, 0x60},
	/* all fans enable, fan1 ctl smart */
	{ 0x13, 0x71}
};

#define ARRAYSIZE(x) sizeof x/sizeof *x

/*
 * Called from superio.c
 */
void init_ec(uint16_t base)
{
	int i;
	for (i=0; i<ARRAYSIZE(sequence); i++) {
		write_index(base, sequence[i].index, sequence[i].value);
	}
}
