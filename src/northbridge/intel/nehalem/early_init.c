/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc
 * Copyright (C) 2013 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <elog.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <arch/cpu.h>

#include "nehalem.h"
#include "pcie_config.c"

static void nehalem_setup_bars(void)
{
	/* Setting up Southbridge. In the northbridge code. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA, DEFAULT_RCBA | 1);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44 /* ACPI_CNTL */ , 0x80);	/* Enable ACPI BAR */

	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	outw(inw(DEFAULT_PMBASE | 0x60 | 0x06) | 2, DEFAULT_PMBASE | 0x60 | 0x06);	/* halt timer */
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR + 4,
			   (0LL + DEFAULT_EPBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR, DEFAULT_MCHBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR + 4,
			   (0LL + DEFAULT_MCHBAR) >> 32);
	pci_write_config32(PCI_DEV(0, 0x00, 0), PCIEXBAR, DEFAULT_PCIEXBAR | 5);	/* 64MB - busses 0-63 */
	pci_write_config32(PCI_DEV(0, 0x00, 0), PCIEXBAR + 4,
			   (0LL + DEFAULT_PCIEXBAR) >> 32);
	pci_write_config32(PCI_DEV(0xff, 0x00, 1), 0x50, DEFAULT_PCIEXBAR | 1);

	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR, DEFAULT_DMIBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR + 4,
			   (0LL + DEFAULT_DMIBAR) >> 32);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(0), 0x30);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(1), 0x33);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(2), 0x33);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(3), 0x33);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(4), 0x33);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(5), 0x33);
	pci_write_config8(PCI_DEV(0xff, 0x00, 1), QPD0F1_PAM(6), 0x33);

#if CONFIG_ELOG_BOOT_COUNT
	/* Increment Boot Counter for non-S3 resume */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) != SLP_TYP_S3)
		boot_count_increment();
#endif

	printk(BIOS_DEBUG, " done.\n");

#if CONFIG_ELOG_BOOT_COUNT
	/* Increment Boot Counter except when resuming from S3 */
	if ((inw(DEFAULT_PMBASE + PM1_STS) & WAK_STS) &&
	    ((inl(DEFAULT_PMBASE + PM1_CNT) >> 10) & 7) == SLP_TYP_S3)
		return;
	boot_count_increment();
#endif
}

static void nehalem_setup_graphics(void)
{
	u32 reg32;
	u16 reg16;
	u8 reg8;

	reg16 = pci_read_config16(PCI_DEV(0, 2, 0), PCI_DEVICE_ID);
	switch (reg16) {
	case 0x0102:		/* GT1 Desktop */
	case 0x0106:		/* GT1 Mobile */
	case 0x010a:		/* GT1 Server */
	case 0x0112:		/* GT2 Desktop */
	case 0x0116:		/* GT2 Mobile */
	case 0x0122:		/* GT2 Desktop >=1.3GHz */
	case 0x0126:		/* GT2 Mobile >=1.3GHz */
	case 0x0156:		/* IvyBridge */
	case 0x0166:		/* IvyBridge */
		break;
	default:
		printk(BIOS_DEBUG,
		       "Graphics not supported by this CPU/chipset.\n");
		return;
	}

	return;

	printk(BIOS_DEBUG, "Initializing Graphics...\n");

	/* Setup IGD memory by setting GGC[7:3] = 1 for 32MB */
	reg16 = pci_read_config16(PCI_DEV(0, 0, 0), D0F0_GGC);
	reg16 &= ~0x00f8;
	reg16 |= 1 << 3;
	/* Program GTT memory by setting GGC[9:8] = 2MB */
	reg16 &= ~0x0300;
	reg16 |= 2 << 8;
	/* Enable VGA decode */
	reg16 &= ~0x0002;
	reg16 |= 2;
	pci_write_config16(PCI_DEV(0, 0, 0), D0F0_GGC, reg16);

	/* Enable 256MB aperture */
	reg8 = pci_read_config8(PCI_DEV(0, 2, 0), MSAC);
	reg8 &= ~0x06;
	reg8 |= 0x02;
	pci_write_config8(PCI_DEV(0, 2, 0), MSAC, reg8);

	/* Erratum workarounds */
	reg32 = MCHBAR32(0x5f00);
	reg32 |= (1 << 9) | (1 << 10);
	MCHBAR32(0x5f00) = reg32;

	/* Enable SA Clock Gating */
	reg32 = MCHBAR32(0x5f00);
	MCHBAR32(0x5f00) = reg32 | 1;

	/* GPU RC6 workaround for sighting 366252 */
	reg32 = MCHBAR32(0x5d14);
	reg32 |= (1 << 31);
	MCHBAR32(0x5d14) = reg32;

	/* VLW */
	reg32 = MCHBAR32(0x6120);
	reg32 &= ~(1 << 0);
	MCHBAR32(0x6120) = reg32;

	reg32 = MCHBAR32(0x5418);
	reg32 |= (1 << 4) | (1 << 5);
	MCHBAR32(0x5418) = reg32;
}

static void early_cpu_init (void)
{
	msr_t m;

	/* bit 0 = disable multicore,
	   bit 1 = disable quadcore,
	   bit 8 = disable hyperthreading.  */
	pci_write_config32(PCI_DEV(0xff, 0x00, 0), 0x80,
			   (pci_read_config32(PCI_DEV(0xff, 0x0, 0x0), 0x80) & 0xfffffefc) | 0x10000);

	u8 reg8;
	struct cpuid_result result;
	result = cpuid_ext(0x6, 0x8b);
	if (!(result.eax & 0x2)) {
		m = rdmsr(MSR_FSB_CLOCK_VCC);
		reg8 = ((m.lo & 0xff00) >> 8) + 1;
		m = rdmsr (IA32_PERF_CTL);
		m.lo = (m.lo & ~0xff) | reg8;
		wrmsr(IA32_PERF_CTL, m);

		m = rdmsr(MSR_IA32_MISC_ENABLES);
		m.hi &= ~0x00000040;
		m.lo |= 0x10000;

		wrmsr(MSR_IA32_MISC_ENABLES, m);
	}

	m = rdmsr(MSR_FSB_CLOCK_VCC);
	reg8 = ((m.lo & 0xff00) >> 8) + 1;

	m = rdmsr (IA32_PERF_CTL);
	m.lo = (m.lo & ~0xff) | reg8;
	wrmsr(IA32_PERF_CTL, m);

	m = rdmsr(MSR_IA32_MISC_ENABLES);
	m.lo |= 0x10000;
	wrmsr(MSR_IA32_MISC_ENABLES, m);

	m = rdmsr(0x1f1);
	m.lo |= 1;
	wrmsr(0x1f1, m);

}

void nehalem_early_initialization(int chipset_type)
{
	u32 capid0_a;
	u8 reg8;

	/* Device ID Override Enable should be done very early */
	capid0_a = pci_read_config32(PCI_DEV(0, 0, 0), 0xe4);
	if (capid0_a & (1 << 10)) {
		reg8 = pci_read_config8(PCI_DEV(0, 0, 0), 0xf3);
		reg8 &= ~7;	/* Clear 2:0 */

		if (chipset_type == NEHALEM_MOBILE)
			reg8 |= 1;	/* Set bit 0 */

		pci_write_config8(PCI_DEV(0, 0, 0), 0xf3, reg8);
	}

	/* Setup all BARs required for early PCIe and raminit */
	nehalem_setup_bars();

	/* Device Enable */
	pci_write_config32(PCI_DEV(0, 0, 0), D0F0_DEVEN, 9 | 2);

	nehalem_setup_graphics();

	early_cpu_init();

	pci_write_config32(PCI_DEV(0, 0x16, 0), 0x10, DEFAULT_HECIBAR);
	pci_write_config32(PCI_DEV(0, 0x16, 0), PCI_COMMAND,
			   PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}
