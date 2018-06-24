/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define SUPERIO_BASE 0x2e
#define SUPERIO_DEV PNP_DEV(SUPERIO_BASE, 0)
#define SIO_GPIO PNP_DEV(SUPERIO_BASE, IT8728F_GPIO)
#define SERIAL_DEV PNP_DEV(SUPERIO_BASE, 0x01)

void mainboard_rcba_config(void)
{
	/* Enable HECI */
	RCBA32(FD2) &= ~0x2;
}

void pch_enable_lpc(void)
{
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN |
			CNF1_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x3c0a01);
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, ETR3, 0x10000);

	/* Initialize SuperIO */
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	ite_reg_write(SIO_GPIO, 0xEF, 0x7E); // magic SIO disable reboot

	/* FIXME: These values could be configured in ramstage */
	ite_reg_write(SIO_GPIO, 0x25, 0x40); // gpio pin function -> gp16
	ite_reg_write(SIO_GPIO, 0x27, 0x10); // gpio pin function -> gp34
	ite_reg_write(SIO_GPIO, 0x2c, 0x80); // smbus isolation on parallel port
	ite_reg_write(SIO_GPIO, 0x62, 0x0a); // simple iobase 0xa00
	ite_reg_write(SIO_GPIO, 0x72, 0x20); // watchdog timeout clear!
	ite_reg_write(SIO_GPIO, 0x73, 0x00); // watchdog timeout clear!
	ite_reg_write(SIO_GPIO, 0xcb, 0x00); // simple io set4 direction -> in
	ite_reg_write(SIO_GPIO, 0xe9, 0x27); // bus select disable
	ite_reg_write(SIO_GPIO, 0xf0, 0x10); // ?
	ite_reg_write(SIO_GPIO, 0xf1, 0x42); // ?
	ite_reg_write(SIO_GPIO, 0xf6, 0x1c); // hwmon alert beep -> gp36(pin12)

	/* EC SIO settings */
	ite_reg_write(IT8728F_EC, 0xf1, 0xc0);
	ite_reg_write(IT8728F_EC, 0xf6, 0xf0);
	ite_reg_write(IT8728F_EC, 0xf9, 0x48);
	ite_reg_write(IT8728F_EC, 0x60, 0x0a);
	ite_reg_write(IT8728F_EC, 0x61, 0x30);
	ite_reg_write(IT8728F_EC, 0x62, 0x0a);
	ite_reg_write(IT8728F_EC, 0x63, 0x20);
	ite_reg_write(IT8728F_EC, 0x30, 0x01);
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 5, 0 },
	{ 1, 5, 0 },
	{ 1, 5, 1 },
	{ 1, 5, 1 },
	{ 1, 5, 2 },
	{ 1, 5, 2 },
	{ 1, 5, 3 },
	{ 1, 5, 3 },
	{ 1, 5, 4 },
	{ 1, 5, 4 },
	{ 1, 5, 6 },
	{ 1, 5, 5 },
	{ 1, 5, 5 },
	{ 1, 5, 6 },
};

/* FIXME: This board only has two DIMM slots! */
void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	read_spd (&spd[0], 0x50, id_only);
	read_spd (&spd[1], 0x51, id_only);
	read_spd (&spd[2], 0x52, id_only);
	read_spd (&spd[3], 0x53, id_only);
}

void mainboard_early_init(int s3resume) {
}

void mainboard_config_superio(void)
{
}
