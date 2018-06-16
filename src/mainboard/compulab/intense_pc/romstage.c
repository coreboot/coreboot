/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Hal Martin <hal.martin@gmail.com>
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

#include <stdint.h>
#include <lib.h>
#include <arch/io.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <superio/smsc/sio1007/chip.h>

#define SIO_PORT 0x164e

void pch_enable_lpc(void)
{
	pci_devfn_t dev = PCH_LPC_DEV;

	/* Set COM1/COM2 decode range */
	pci_write_config16(dev, LPC_IO_DEC, 0x0010);

	/* Enable SuperIO */
	u16 lpc_config = CNF1_LPC_EN | CNF2_LPC_EN;
	pci_write_config16(dev, LPC_EN, lpc_config);

	/* Map 1 byte to the LPC bus. */
	pci_write_config32(dev, LPC_GEN1_DEC, 0x00164d);

	/* Map a range for the runtime_port registers to the LPC bus. */
	pci_write_config32(dev, LPC_GEN2_DEC, 0xc0181);

#if IS_ENABLED(CONFIG_DRIVERS_UART_8250IO)
	/* Enable COM1 */
	if (sio1007_enable_uart_at(SIO_PORT)) {
		pci_write_config16(dev, LPC_EN,
				   lpc_config | COMA_LPC_EN);
	}
#endif
}

void mainboard_rcba_config(void)
{
	RCBA32(0x3414) = 0x00000000;
}
const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 1, 4 },
	{ 1, 1, 4 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
	{ 1, 0, 6 },
};

void mainboard_early_init(int s3resume)
{
}

void mainboard_config_superio(void)
{
	const u16 port = SIO_PORT;
	const u16 runtime_port = 0x180;

	/* Turn on configuration mode. */
	outb(0x55, port);

	/* Set the GPIO direction, polarity, and type. */
	sio1007_setreg(port, 0x31, 1 << 0, 1 << 0);
	sio1007_setreg(port, 0x32, 0 << 0, 1 << 0);
	sio1007_setreg(port, 0x33, 0 << 0, 1 << 0);

	/* Set the base address for the runtime register block. */
	sio1007_setreg(port, 0x30, runtime_port >> 4, 0xff);
	sio1007_setreg(port, 0x21, runtime_port >> 12, 0xff);

	/* Turn on address decoding for it. */
	sio1007_setreg(port, 0x3a, 1 << 1, 1 << 1);

	/* Set the value of GPIO 10 by changing GP1, bit 0. */
	u8 byte;
	byte = inb(runtime_port + 0xc);
	byte |= (1 << 0);
	outb(byte, runtime_port + 0xc);

	/* Turn off address decoding for it. */
	sio1007_setreg(port, 0x3a, 0 << 1, 1 << 1);

	/* Turn off configuration mode. */
	outb(0xaa, port);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
