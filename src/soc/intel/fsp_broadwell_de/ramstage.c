/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <stdlib.h>
#include <arch/cpu.h>
#include <arch/acpi.h>
#include <console/console.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <romstage_handoff.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/pattrs.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

/* Global PATTRS */
DEFINE_PATTRS;

#define SHOW_PATTRS 1

static void detect_num_cpus(struct pattrs *attrs)
{
	msr_t core_thread_count = rdmsr(MSR_CORE_THREAD_COUNT);
	attrs->num_cpus = core_thread_count.lo & 0xffff;
}

static inline void fill_in_msr(msr_t *msr, int idx)
{
	*msr = rdmsr(idx);
	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "msr(%x) = %08x%08x\n",
		       idx, msr->hi, msr->lo);
	}
}

static const char *stepping_str[] = {
	"U0", "V1", "V2", "Y0"
};

static void fill_in_pattrs(void)
{
	struct device *dev;
	struct pattrs *attrs = (struct pattrs *)pattrs_get();

	attrs->cpuid = cpuid_eax(1);
	attrs->stepping = (attrs->cpuid & 0x0F) - 1;
	dev = dev_find_slot(0, PCI_DEVFN(LPC_DEV, LPC_FUNC));
	attrs->revid = pci_read_config8(dev, REVID);
	attrs->microcode_patch = intel_microcode_find();
	attrs->address_bits = cpuid_eax(0x80000008) & 0xff;
	detect_num_cpus(attrs);

	if (SHOW_PATTRS) {
		printk(BIOS_DEBUG, "CPUID: %08x\n", attrs->cpuid);
		printk(BIOS_DEBUG, "Cores: %d\n", attrs->num_cpus);
		printk(BIOS_DEBUG, "Stepping: %s\n", (attrs->stepping >= ARRAY_SIZE(stepping_str))
							? "??" : stepping_str[attrs->stepping]);
		printk(BIOS_DEBUG, "Revision ID: %02x\n", attrs->revid);
	}

	fill_in_msr(&attrs->platform_id, MSR_IA32_PLATFORM_ID);
	fill_in_msr(&attrs->platform_info, MSR_PLATFORM_INFO);
}

void broadwell_de_init_pre_device(void)
{
	fill_in_pattrs();
}
