/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <arch/io.h>
#include <device/chip.h>
#include <console/console.h>
#include "chip.h"

void pnp_output(char address, char data)
{
	outb(address, PNP_CFGADDR);
	outb(data, PNP_CFGDATA);
}

void sio_enable(struct chip *chip, enum chip_pass pass)
{

	unsigned char reg;
	struct superio_NSC_pc97307_config *conf = (struct superio_NSC_pc97307_config *)chip->chip_info;

	switch (pass) {
	case CONF_PASS_PRE_PCI:
		printk_info("Configuring PC97307...\n");

		/* Enable keyboard */
		pnp_output(0x07, 0x00);
		pnp_output(0x30, 0x00); /* Disable keyboard */
		pnp_output(0xf0, 0x40); /* Set KBC clock to 8 Mhz */
		pnp_output(0x30, 0x01); /* Enable keyboard */

		/* Enable mouse */
		pnp_output(0x07, 0x01);
		pnp_output(0x30, 0x01);

		/* Enable rtc */
		pnp_output(0x07, 0x02);
		pnp_output(0x30, 0x01);

		/* Enable fdc */
		pnp_output(0x07, 0x03);
		pnp_output(0x30, 0x01);

		/* Enable parallel port */
		pnp_output(0x07, 0x04);
		pnp_output(0x30, 0x01);

		/* Set up floppy in PS/2 mode */
		outb(0x09, SIO_CONFIG_RA);
		reg = inb(SIO_CONFIG_RD);
		reg = (reg & 0x3F) | 0x40;
		outb(reg, SIO_CONFIG_RD);
		outb(reg, SIO_CONFIG_RD);       /* Have to write twice to change! */
		break;
	default:
		/* nothing yet */
		break;
	}
}

struct chip_control superio_NSC_pc97307_control = {
	enable: sio_enable,
	name:   "NSC 97307"
};
