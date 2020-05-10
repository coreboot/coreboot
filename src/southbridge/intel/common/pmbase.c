/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <acpi/acpi.h>
#include <arch/io.h>
#include <bootmode.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <assert.h>

#include "pmbase.h"
#include "pmutil.h"

/* LPC PM Base Address Register */
#define PMBASE		0x40
#define PMSIZE		0x80

/* PCI Configuration Space (D31:F0): LPC */
#if defined(__SIMPLE_DEVICE__)
#define PCH_LPC_DEV	PCI_DEV(0, 0x1f, 0)
#else
#define PCH_LPC_DEV	pcidev_on_root(0x1f, 0)
#endif

u16 lpc_get_pmbase(void)
{
#ifdef __SIMPLE_DEVICE__
	/* Don't assume PMBASE is still the same */
	return pci_read_config16(PCH_LPC_DEV, PMBASE) & 0xfffc;
#else
	static u16 pmbase;

	if (pmbase)
		return pmbase;

	pmbase = pci_read_config16(PCH_LPC_DEV, PMBASE) & 0xfffc;

	return pmbase;
#endif
}

void write_pmbase32(const u8 addr, const u32 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u32)));

	outl(val, lpc_get_pmbase() + addr);
}

void write_pmbase16(const u8 addr, const u16 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u16)));

	outw(val, lpc_get_pmbase() + addr);
}

void write_pmbase8(const u8 addr, const u8 val)
{
	ASSERT(addr <= (PMSIZE - sizeof(u8)));

	outb(val, lpc_get_pmbase() + addr);
}

u32 read_pmbase32(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u32)));

	return inl(lpc_get_pmbase() + addr);
}

u16 read_pmbase16(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u16)));

	return inw(lpc_get_pmbase() + addr);
}

u8 read_pmbase8(const u8 addr)
{
	ASSERT(addr <= (PMSIZE - sizeof(u8)));

	return inb(lpc_get_pmbase() + addr);
}

int platform_is_resuming(void)
{
	u16 reg16 = read_pmbase16(PM1_STS);

	if (!(reg16 & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(reg16) == ACPI_S3;
}
