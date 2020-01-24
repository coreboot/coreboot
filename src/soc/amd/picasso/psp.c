/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <amdblocks/psp.h>

#define PSP_MAILBOX_OFFSET		0x10570
#define MSR_CU_CBBCFG			0xc00110a2

void *soc_get_mbox_address(void)
{
	uintptr_t psp_mmio;

	psp_mmio = rdmsr(MSR_CU_CBBCFG).lo;
	if (psp_mmio == 0xffffffff) {
		printk(BIOS_WARNING, "PSP: MSR_CU_CBBCFG uninitialized\n");
		return 0;
	}

	return (void *)(psp_mmio + PSP_MAILBOX_OFFSET);
}
