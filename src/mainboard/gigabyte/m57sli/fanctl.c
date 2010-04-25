#include <arch/io.h>
#include <stdlib.h>
#include <superio/ite/it8716f/it8716f.h>

static void write_index(uint16_t port_base, uint8_t reg, uint8_t value)
{
	outb(reg, port_base);
	outb(value, port_base + 1);
}

static const struct {
	uint8_t index, value;
} sequence[]= {
	/* Make sure we can monitor, and enable SMI# interrupt output */
	{ 0x00, 0x13},
	/* Disable fan interrupt status bits for SMI# */
	{ 0x04, 0x37},
	/* Disable VIN interrupt status bits for SMI# */
	{ 0x05, 0xff},
	/* Disable fan interrupt status bits for IRQ */
	{ 0x07, 0x37},
	/* Disable VIN interrupt status bits for IRQ */
	{ 0x08, 0xff},
	/* Disable external sensor interrupt */
	{ 0x09, 0x87},
	/* Enable 16 bit counter divisors */
	{ 0x0c, 0x07},
	/* Set FAN_CTL control register (0x14) polarity to high, and
	   activate fans 1, 2 and 3. */
	{ 0x14, 0xd7},
	/* set the correct sensor types 1,2 thermistor; 3 diode */
	{ 0x51, 0x1c},
	/* set the 'zero' voltage for diode type sensor 3 */
	{ 0x5c, 0x80},
//	{ 0x56, 0xe5},
//	{ 0x57, 0xe5},
	{ 0x59, 0xec},
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
	/* temperature limit of fan stop for fan3 (automatic) */
	{ 0x70, 0xff},
	/* temperature limit of fan start for fan3 (automatic) */
	{ 0x71, 0x14},
	/* Set PWM start & slope for fan3 */
	{ 0x73, 0x20},
	/* Initialize PWM automatic mode slope values for fan3 */
	{ 0x74, 0x90},
	/* set smartguardian temperature interval for fan3 */
	{ 0x75, 0x03},
	/* fan1 auto controlled by temp3 */
	{ 0x15, 0x82},
	/* fan2 auto controlled by temp3 */
	{ 0x16, 0x82},
	/* fan3 auto controlled by temp3 */
	{ 0x17, 0x82},
	/* all fans enable, fan1 ctl smart */
	{ 0x13, 0x77}
};

/*
 * Called from superio.c
 */
void init_ec(uint16_t base)
{
	int i;
	for (i=0; i<ARRAY_SIZE(sequence); i++) {
		write_index(base, sequence[i].index, sequence[i].value);
	}
}
