/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 - 2012 Advanced Micro Devices, Inc.
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

#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam15.h>
#include <soc/pci_devs.h>
#include <soc/pci_devs.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <agesawrapper.h>

/*
 * Enable VGA cycles.  Set memory ranges of the FCH legacy devices (TPM, HPET,
 * BIOS RAM, Watchdog Timer, IOAPIC and ACPI) as non-posted.  Set remaining
 * MMIO to posted.  Route all I/O to the southbridge.
 */
void amd_initcpuio(void)
{
	msr_t topmem = rdmsr(TOP_MEM); /* todo: build bsp_topmem() earlier */
	uintptr_t base, limit;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	pci_write_config32(SOC_ADDR_DEV, D18F1_VGAEN, VGA_ADDR_ENABLE);

	/* Non-posted: range(HPET-LAPIC) or 0xfed00000 through 0xfee00000-1 */
	base = (HPET_BASE_ADDRESS >> 8) | MMIO_WE | MMIO_RE;
	limit = (ALIGN_DOWN(LOCAL_APIC_ADDR - 1, 64 * KiB) >> 8) | MMIO_NP;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_LIMIT_LO(0), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_BASE_LO(0), base);

	/* Remaining PCI hole posted MMIO: TOM-HPET (TOM through 0xfed00000-1 */
	base = (topmem.lo >> 8) | MMIO_WE | MMIO_RE;
	limit = ALIGN_DOWN(HPET_BASE_ADDRESS - 1, 64 * KiB) >> 8;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_LIMIT_LO(1), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_BASE_LO(1), base);

	/* Route all I/O downstream */
	base = 0 | IO_WE | IO_RE;
	limit = ALIGN_DOWN(0xffff, 4 * KiB);
	pci_write_config32(SOC_ADDR_DEV, NB_IO_LIMIT(0), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_IO_BASE(0), base);
}

/* Set the MMIO Configuration Base Address and Bus Range. */
void amd_initmmio(void)
{
	msr_t mmconf;
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int mtrr;

	mmconf.hi = 0;
	mmconf.lo = CONFIG_MMCONF_BASE_ADDRESS | MMIO_RANGE_EN
			| fms(CONFIG_MMCONF_BUS_NUMBER) << MMIO_BUS_RANGE_SHIFT;
	wrmsr(MMIO_CONF_BASE, mmconf);

	/*
	 * todo: AGESA currently writes variable MTRRs.  Once that is
	 *       corrected, un-hardcode this MTRR.
	 */
	mtrr = (mtrr_cap.lo & MTRR_CAP_VCNT) - 2;
	set_var_mtrr(mtrr, FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);
}
