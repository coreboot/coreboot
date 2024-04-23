/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include <superio/smsc/sch555x/sch555x.h>
#include "sch5555_ec.h"

uint8_t sch5555_mbox_read(uint8_t addr1, uint16_t addr2)
{
	// clear ec-to-host mailbox
	uint8_t tmp = inb(SCH555x_EMI_IOBASE + 1);
	outb(tmp, SCH555x_EMI_IOBASE + 1);

	// send address
	outw(0 | 0x8001, SCH555x_EMI_IOBASE + 2);
	outw((addr1 * 2) | 0x100, SCH555x_EMI_IOBASE + 4);

	outw(4 | 0x8002, SCH555x_EMI_IOBASE + 2);
	outl(addr2 << 16, SCH555x_EMI_IOBASE + 4);

	// send message to ec
	outb(1, SCH555x_EMI_IOBASE);

	// wait for ack
	for (size_t retry = 0; retry < 0xfff; ++retry)
		if (inb(SCH555x_EMI_IOBASE + 1) & 1)
			break;

	// read result
	outw(4 | 0x8000, SCH555x_EMI_IOBASE + 2);
	return inb(SCH555x_EMI_IOBASE + 4);
}

void sch5555_mbox_write(uint8_t addr1, uint16_t addr2, uint8_t val)
{
	// clear ec-to-host mailbox
	uint8_t tmp = inb(SCH555x_EMI_IOBASE + 1);
	outb(tmp, SCH555x_EMI_IOBASE + 1);

	// send address and value
	outw(0 | 0x8001, SCH555x_EMI_IOBASE + 2);
	outw((addr1 * 2) | 0x101, SCH555x_EMI_IOBASE + 4);

	outw(4 | 0x8002, SCH555x_EMI_IOBASE + 2);
	outl(val | (addr2 << 16), SCH555x_EMI_IOBASE + 4);

	// send message to ec
	outb(1, SCH555x_EMI_IOBASE);

	// wait for ack
	for (size_t retry = 0; retry < 0xfff; ++retry)
		if (inb(SCH555x_EMI_IOBASE + 1) & 1)
			break;
}
