/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <arch/io.h>
#include "sio1007.h"

void sio1007_setreg(u16 lpc_port, u8 reg, u8 value, u8 mask)
{
	u8 reg_value;

	outb(reg, lpc_port);
	reg_value = inb(lpc_port + 1);
	reg_value &= ~mask;
	reg_value |= (value & mask);
	outb(reg_value, lpc_port + 1);
}

int sio1007_enable_uart_at(u16 port)
{
	/* Enable config mode. */
	outb(0x55, port);
	if (inb(port) != 0x55)
		return 0; /* There is no LPC device at this address. */

	/* Registers 12 and 13 hold config address, look for a match. */
	outb(0x12, port);
	if (inb(port + 1) != (port & 0xff))
		return 0;

	outb(0x13, port);
	if (inb(port + 1) != (port >> 8))
		return 0;

	/* This must be the sio1007, enable the UART. */
	/* turn on power */
	sio1007_setreg(port, 0x2, 1 << 3, 1 << 3);
	/* enable high speed */
	sio1007_setreg(port, 0xc, 1 << 6, 1 << 6);
	/* set the base address */
	sio1007_setreg(port, 0x24, CONFIG_TTYS0_BASE >> 2, 0xff);

	/* Disable config mode. */
	outb(0xaa, port);
	return 1;
}
