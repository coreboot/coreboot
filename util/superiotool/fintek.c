/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 coresystems GmbH <info@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include "superiotool.h"

#define DEVICE_ID_BYTE1_REG	0x20
#define DEVICE_ID_BYTE2_REG	0x21
#define VENDOR_ID_BYTE1_REG	0x23
#define VENDOR_ID_BYTE2_REG	0x24

const static struct superio_registers reg_table[] = {
	{0x0604, "F71805", {
		{EOT}}},
	{0x4103, "F71872", {
		{EOT}}},
	{EOT}
};

void dump_fintek(uint16_t port, uint16_t did)
{
	switch (did) {
	case 0x0604:
		printf("Fintek F71805\n");
		break;
	case 0x4103:
		printf("Fintek F71872\n");
		break;
	default:
		printf("Unknown Fintek Super I/O: did=0x%04x\n", did);
		return;
	}

	printf("Flash write is %s.\n",
	       regval(port, 0x28) & 0x80 ? "enabled" : "disabled");
	printf("Flash control is 0x%04x.\n", regval(port, 0x28));
	printf("27=%02x\n", regval(port, 0x27));
	printf("29=%02x\n", regval(port, 0x29));
	printf("2a=%02x\n", regval(port, 0x2a));
	printf("2b=%02x\n", regval(port, 0x2b));

	/* Select UART 1. */
	regwrite(port, 0x07, 0x01);
	printf("UART1 is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART1 base=%02x%02x, irq=%02x, mode=%s\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f,
	       regval(port, 0xf0) & 0x10 ? "RS485" : "RS232");

	/* Select UART 2. */
	regwrite(port, 0x07, 0x02);
	printf("UART2 is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("UART2 base=%02x%02x, irq=%02x, mode=%s\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f,
	       regval(port, 0xf0) & 0x10 ? "RS485" : "RS232");

	/* Select parallel port. */
	regwrite(port, 0x07, 0x03);
	printf("PARPORT is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("PARPORT base=%02x%02x, irq=%02x\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* Select HW monitor. */
	regwrite(port, 0x07, 0x04);
	printf("HW monitor is %s\n",
	       regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf("HW monitor base=%02x%02x, irq=%02x\n", regval(port, 0x60),
	       regval(port, 0x61), regval(port, 0x70) & 0x0f);

	/* Select GPIO. */
	regwrite(port, 0x07, 0x05);
	printf("GPIO is %s\n", regval(port, 0x30) & 1 ? "enabled" : "disabled");
	printf
	    ("GPIO 70=%02x, e0=%02x, e1=%02x, e2=%02x, e3=%02x, e4=%02x, e5=%02x\n",
	     regval(port, 0x70), regval(port, 0xe0), regval(port, 0xe1),
	     regval(port, 0xe2), regval(port, 0xe3), regval(port, 0xe4),
	     regval(port, 0xe5));
	printf
	    ("GPIO e6=%02x, e7=%02x, e8=%02x, e9=%02x, f0=%02x, f1=%02x, f3=%02x, f4=%02x\n",
	     regval(port, 0xe6), regval(port, 0xe7), regval(port, 0xe8),
	     regval(port, 0xe9), regval(port, 0xf0), regval(port, 0xf1),
	     regval(port, 0xf3), regval(port, 0xf4));
	printf("GPIO f5=%02x, f6=%02x, f7=%02x, f8=%02x\n", regval(port, 0xf5),
	       regval(port, 0xf6), regval(port, 0xf7), regval(port, 0xf8));
}

void probe_idregs_fintek(uint16_t port)
{
	uint16_t vid, did;

	enter_conf_mode_winbond_fintek_ite_8787(port);

	did = regval(port, DEVICE_ID_BYTE1_REG);
	did |= (regval(port, DEVICE_ID_BYTE2_REG) << 8);

	vid = regval(port, VENDOR_ID_BYTE1_REG);
	vid |= (regval(port, VENDOR_ID_BYTE2_REG) << 8);

	if (vid != 0x3419) {
		no_superio_found(port);
		exit_conf_mode_winbond_fintek_ite_8787(port);
		return;
	}

        printf("Found Fintek %s (vid=0x%04x, id=0x%04x) at port=0x%x\n",
       	       get_superio_name(reg_table, did), vid, did, port);

	dump_superio("Fintek", reg_table, port, did);

	/* TODO: Revive this as --dump-human-readable output. */
	/* dump_fintek(port, did); */

	exit_conf_mode_winbond_fintek_ite_8787(port);
}

