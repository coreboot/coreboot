/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/chip.h>

#ifndef PNP_INDEX_REG
#define PNP_INDEX_REG	0x15C
#endif
#ifndef PNP_DATA_REG
#define PNP_DATA_REG	0x15D
#endif
#ifndef SIO_COM1
#define SIO_COM1_BASE	0x3F8
#endif
#ifndef SIO_COM2
#define SIO_COM2_BASE	0x2F8
#endif

void pnp_output(char address, char data)
{
	outb(address, PNP_INDEX_REG);
	outb(data, PNP_DATA_REG);
}

void sio_enable(struct chip *chip, enum chip_pass pass)
{
	switch (pass) {
	case CHIP_PRE_CONSOLE:
		/* Enable Super IO Chip */
		pnp_output(0x07, 6); /* LD 6 = UART1 */
		pnp_output(0x30, 0); /* Dectivate */
		pnp_output(0x60, chip->control->defaultport >> 8); /* IO Base */
		pnp_output(0x61, chip->control->defaultport & 0xFF); /* IO Base */
		pnp_output(0x30, 1); /* Activate */
		break;
	default:
		/* nothing yet */
	}
}

struct superio_control superio_NSC_pc97307_control = {
	enable:   	sio_enable,
	defaultport:    SIO_COM1_BASE,
	name:           "NSC 87307"
};
