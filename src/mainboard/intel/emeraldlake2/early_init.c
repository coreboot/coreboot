/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <stdint.h>
#include <arch/io.h>
#include <superio/smsc/sio1007/sio1007.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

#define SIO_PORT 0x164e

void bootblock_mainboard_early_init(void)
{
	const u16 port = SIO_PORT;
	const u16 runtime_port = 0x180;

	sio1007_enable_uart_at(port);

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

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  USB oc pin  */
	{ 1, 0, 0 }, /* P0: Front port  (OC0) */
	{ 1, 0, 1 }, /* P1: Back port   (OC1) */
	{ 1, 0, -1 }, /* P2: MINIPCIE1   (no OC) */
	{ 1, 0, -1 }, /* P3: MMC         (no OC) */
	{ 1, 0, 2 }, /* P4: Front port  (OC2) */
	{ 0, 0, -1 }, /* P5: Empty */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	{ 1, 0, 4 }, /* P8: Back port   (OC4) */
	{ 1, 0, -1 }, /* P9: MINIPCIE3   (no OC) */
	{ 1, 0, -1 }, /* P10: BLUETOOTH  (no OC) */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 1, 0, 6 }, /* P12: Back port  (OC6) */
	{ 1, 0, 5 }, /* P13: Back port  (OC5) */
};
