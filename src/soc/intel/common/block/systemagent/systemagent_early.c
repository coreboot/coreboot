/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/systemagent.h>
#include <security/intel/txt/txt_platform.h>
#include <security/intel/txt/txt_register.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>

#include "systemagent_def.h"

void bootblock_systemagent_early_init(void)
{
	uint32_t reg;
	uint8_t pciexbar_length;

	/*
	 * The PCIEXBAR is assumed to live in the memory mapped IO space under
	 * 4GiB.
	 */
	reg = 0;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR + 4, reg);

	/* Get PCI Express Region Length */
	switch (CONFIG_ECAM_MMCONF_BUS_NUMBER) {
	case 256:
		pciexbar_length = PCIEXBAR_LENGTH_256MB;
		break;
	case 128:
		pciexbar_length = PCIEXBAR_LENGTH_128MB;
		break;
	case 64:
		pciexbar_length = PCIEXBAR_LENGTH_64MB;
		break;
	default:
		dead_code();
	}
	reg = CONFIG_ECAM_MMCONF_BASE_ADDRESS | (pciexbar_length << 1)
				| PCIEXBAR_PCIEXBAREN;
	pci_io_write_config32(SA_DEV_ROOT, PCIEXBAR, reg);

	/*
	 * TSEG defines the base of SMM range. BIOS determines the base
	 * of TSEG memory which must be at or below Graphics base of GTT
	 * Stolen memory, hence its better to clear TSEG register early
	 * to avoid power on default non-zero value (if any).
	 */
	pci_write_config32(SA_DEV_ROOT, TSEG, 0);
}

void sa_set_pci_bar(const struct sa_mmio_descriptor *fixed_set_resources,
		size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		uint64_t base;
		unsigned int index;

		index = fixed_set_resources[i].index;
		/* Check if PCI BAR already enabled */
		base = pci_read_config32(SA_DEV_ROOT, index);

		/* If enabled don't program it. */
		if (base & PCIEXBAR_PCIEXBAREN)
			return;

		base = fixed_set_resources[i].base;
		if (base >> 32)
			pci_write_config32(SA_DEV_ROOT, index + 4, base >> 32);
		pci_write_config32(SA_DEV_ROOT, index,
			(base & 0xffffffff) | PCIEXBAR_PCIEXBAREN);
	}
}

/*
 * There are special BARs that actually are programmed in the MCHBAR. These
 * Intel special features, but they do consume resources that need to be
 * accounted for.
 */
void sa_set_mch_bar(const struct sa_mmio_descriptor *fixed_set_resources,
		size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		uint64_t base;
		unsigned int index;

		base = fixed_set_resources[i].base;
		index = fixed_set_resources[i].index;
		if (base >> 32)
			write32p((uintptr_t)(MCH_BASE_ADDRESS + index + 4), base >> 32);
		write32p((uintptr_t)(MCH_BASE_ADDRESS + index),
			(base & 0xffffffff) | PCIEXBAR_PCIEXBAREN);
	}
}

void enable_pam_region(void)
{
	/* All read and writes in this region are serviced by DRAM */
	pci_write_config8(SA_DEV_ROOT, PAM0, 0x30);
	pci_write_config8(SA_DEV_ROOT, PAM1, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM2, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM3, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM4, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM5, 0x33);
	pci_write_config8(SA_DEV_ROOT, PAM6, 0x33);
}

void enable_bios_reset_cpl(void)
{
	u8 bios_reset_cpl;

	/*
	 * Set bits 0+1 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	bios_reset_cpl = MCHBAR8(BIOS_RESET_CPL);
	bios_reset_cpl |= 3;
	MCHBAR8(BIOS_RESET_CPL) = bios_reset_cpl;
}

uintptr_t sa_get_tolud_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, TOLUD), 1*MiB);
}

uintptr_t sa_get_gsm_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, BGSM), 1*MiB);
}

uintptr_t sa_get_tseg_base(void)
{
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(SA_DEV_ROOT, TSEG), 1*MiB);
}

size_t sa_get_tseg_size(void)
{
	return sa_get_gsm_base() - sa_get_tseg_base();
}

union dpr_register txt_get_chipset_dpr(void)
{
	return (union dpr_register) { .raw = pci_read_config32(SA_DEV_ROOT, DPR) };
}
