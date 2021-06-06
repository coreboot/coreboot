/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/pnp_ops.h>
#include "sch555x.h"

uint8_t sch555x_emi_read8(uint16_t addr)
{
	outw(addr | 0x8000, SCH555x_EMI_IOBASE + 2);
	return inb(SCH555x_EMI_IOBASE + 4);
}

uint16_t sch555x_emi_read16(uint16_t addr)
{
	outw(addr | 0x8001, SCH555x_EMI_IOBASE + 2);
	return inw(SCH555x_EMI_IOBASE + 4);
}

uint32_t sch555x_emi_read32(uint16_t addr)
{
	outw(addr | 0x8002, SCH555x_EMI_IOBASE + 2);
	return inl(SCH555x_EMI_IOBASE + 4);
}

void sch555x_emi_write8(uint16_t addr, uint8_t val)
{
	outw(addr | 0x8000, SCH555x_EMI_IOBASE + 2);
	outb(val, SCH555x_EMI_IOBASE + 4);
}

void sch555x_emi_write16(uint16_t addr, uint16_t val)
{
	outw(addr | 0x8001, SCH555x_EMI_IOBASE + 2);
	outw(val, SCH555x_EMI_IOBASE + 4);
}

void sch555x_emi_write32(uint16_t addr, uint32_t val)
{
	outw(addr | 0x8002, SCH555x_EMI_IOBASE + 2);
	outl(val, SCH555x_EMI_IOBASE + 4);
}
