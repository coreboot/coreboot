/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_mmconf.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <lib.h>

void enable_pci_mmconf(void)
{
	msr_t mmconf;

	mmconf.hi = (uint64_t)CONFIG_ECAM_MMCONF_BASE_ADDRESS >> 32;
	mmconf.lo = (CONFIG_ECAM_MMCONF_BASE_ADDRESS & 0xfff00000) | MMIO_RANGE_EN
			| __fls(CONFIG_ECAM_MMCONF_BUS_NUMBER) << MMIO_BUS_RANGE_SHIFT;
	wrmsr(MMIO_CONF_BASE, mmconf);
}
