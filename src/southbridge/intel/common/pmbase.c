/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <arch/io.h>
#include <assert.h>
#include <bootmode.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <stdint.h>

#include "pmbase.h"
#include "pmutil.h"

/* LPC PM Base Address Register */
#define PMBASE		0x40
#define PMSIZE		0x80

u16 lpc_get_pmbase(void)
{
#ifdef __SIMPLE_DEVICE__
	/* Don't assume PMBASE is still the same */
	return pci_read_config16(PCI_DEV(0, 0x1f, 0), PMBASE) & 0xfffc;
#else
	static u16 pmbase;

	if (pmbase)
		return pmbase;

	pmbase = pci_read_config16(pcidev_on_root(0x1f, 0), PMBASE) & 0xfffc;

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

int acpi_get_sleep_type(void)
{
	return acpi_sleep_from_pm1(read_pmbase32(PM1_CNT));
}

/*
 * Note that southbridge_detect_s3_resume clears the sleep state,
 * so this may not be used reliable throughout romstage.
 */
int platform_is_resuming(void)
{
	u16 reg16 = read_pmbase16(PM1_STS);

	if (!(reg16 & WAK_STS))
		return 0;

	return acpi_get_sleep_type() == ACPI_S3;
}
