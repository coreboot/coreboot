/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/chip.h>
#include "chip.h"

void pnp_output(char address, char data)
{
	outb(address, PNP_INDEX_REG);
	outb(data, PNP_DATA_REG);
}

void sio_enable(struct chip *chip, enum chip_pass pass)
{

	struct superio_NSC_pc97307_config *conf = (struct superio_NSC_pc97307_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_CONSOLE:
		/* Enable Super IO Chip */
		pnp_output(0x07, 6); /* LD 6 = UART1 */
		pnp_output(0x30, 0); /* Dectivate */
		pnp_output(0x60, conf->port >> 8); /* IO Base */
		pnp_output(0x61, conf->port & 0xFF); /* IO Base */
		pnp_output(0x30, 1); /* Activate */
		break;
	default:
		/* nothing yet */
		break;
	}
}

struct chip_control superio_NSC_pc97307_control = {
	enable: sio_enable,
	name:   "NSC 87307"
};
