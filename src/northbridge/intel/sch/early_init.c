/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
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

#include "sch.h"
#include <southbridge/intel/sch/sch.h>

#if 0
static void sch_set_mtrr(void)
{
	msr_t msr;
	printk(BIOS_DEBUG, "1");
	msr.hi = 0x06060606;
	msr.lo = 0x06060606;
	wrmsr(0x250, msr);
	printk(BIOS_DEBUG, "2");
	msr.hi = 0x06060606;
	msr.lo = 0x06060606;
	wrmsr(0x258, msr);
	printk(BIOS_DEBUG, "3");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x259, msr);
	printk(BIOS_DEBUG, "4");
	msr.hi = 0x04040404;
	msr.lo = 0x04040404;
	wrmsr(0x268, msr);
	printk(BIOS_DEBUG, "5");
	msr.hi = 0x04040404;
	msr.lo = 0x04040404;
	wrmsr(0x269, msr);
	printk(BIOS_DEBUG, "6");
	msr.hi = 0x04040404;
	msr.lo = 0x04040404;
	wrmsr(0x26A, msr);
	printk(BIOS_DEBUG, "7");
	msr.hi = 0x04040404;
	msr.lo = 0x04040404;
	wrmsr(0x26B, msr);
	printk(BIOS_DEBUG, "8");
	msr.hi = 0x04040404;
	msr.lo = 0x04040404;
	wrmsr(0x26C, msr);
	printk(BIOS_DEBUG, "9");
	msr.hi = 0x05050505;
	msr.lo = 0x05050505;
	wrmsr(0x26D, msr);
	printk(BIOS_DEBUG, "10");
	msr.hi = 0x05050505;
	msr.lo = 0x05050505;
	wrmsr(0x26E, msr);
	printk(BIOS_DEBUG, "11");
	msr.hi = 0x05050505;
	msr.lo = 0x05050505;
	wrmsr(0x26f, msr);
	printk(BIOS_DEBUG, "12");
	msr.hi = 0x0;
	msr.lo = 0x6;
	wrmsr(0x202, msr);
	printk(BIOS_DEBUG, "13");
	msr.hi = 0x0;
	msr.lo = 0xC0000800;
	wrmsr(0x203, msr);
	printk(BIOS_DEBUG, "14");
	msr.hi = 0x0;
	msr.lo = 0x3FAF0000;
	wrmsr(0x204, msr);
	printk(BIOS_DEBUG, "15");
	msr.hi = 0x0;
	msr.lo = 0xFFFF0800;
	wrmsr(0x205, msr);
	printk(BIOS_DEBUG, "16");
	msr.hi = 0x0;
	msr.lo = 0x3FB00000;
	wrmsr(0x206, msr);
	printk(BIOS_DEBUG, "16");
	msr.hi = 0x0;
	msr.lo = 0xFFF00800;
	wrmsr(0x207, msr);
	printk(BIOS_DEBUG, "17");
	msr.hi = 0x0;
	msr.lo = 0x3FC00000;
	wrmsr(0x208, msr);
	printk(BIOS_DEBUG, "18");
	msr.hi = 0x0;
	msr.lo = 0xFFC00800;
	wrmsr(0x209, msr);
	printk(BIOS_DEBUG, "19");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20A, msr);
	printk(BIOS_DEBUG, "20");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20B, msr);
	printk(BIOS_DEBUG, "21");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20a, msr);
	printk(BIOS_DEBUG, "22");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20B, msr);
	printk(BIOS_DEBUG, "23");
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20c, msr);
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20d, msr);
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20E, msr);
	msr.hi = 0x0;
	msr.lo = 0x0;
	wrmsr(0x20F, msr);
	msr.hi = 0x0;
	msr.lo = 0XC00;
	wrmsr(0x2FF, msr);
	printk(BIOS_DEBUG, "end");
}
#endif

static void sch_detect_chipset(void)
{
	u16 reg16;
	u8 reg8;
	printk(BIOS_INFO, "\n");
	reg16 = pci_read_config16(PCI_DEV(0, 0x00, 0), 0x2);
	switch (reg16) {
	case 0x8101:
		printk(BIOS_INFO, "UL11L/US15L");
		break;
	case 0x8100:
		printk(BIOS_INFO, "US15W");
		break;
	default:
		/* Others reserved. */
		printk(BIOS_INFO, "Unknown (%02x)", reg16);
	}
	printk(BIOS_INFO, " Chipset ");

	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0x8);
	switch (reg8) {
	case 3:
		printk(BIOS_INFO, "Qual. Sample ES1, Stepping B1");
		break;
	case 4:
		printk(BIOS_INFO, "Qual. Sample ES2, Stepping C0");
		break;
	case 5:
		printk(BIOS_INFO, "Qual. Sample ES2-Prime, Stepping D0");
		break;
	case 6:
		printk(BIOS_INFO, "Qual. Sample QS, Stepping D1");
		break;
	default:
		/* Others reserved. */
		printk(BIOS_INFO, "Unknown (%02x)", reg8);
	}
	printk(BIOS_INFO, "\n");
}

static void sch_setup_non_standard_bars(void)
{
	printk(BIOS_DEBUG, "Setting up ACPI PM1 block ");
	/* Address 0x1000 size 16B */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x48,
			   (0x80000000 | DEFAULT_PMBASE));

	printk(BIOS_DEBUG, "Setting up ACPI P block ");
	/* Address 0x1010 size 16B */
	sch_port_access_write(4, 0x70, 4, 0x80001010);

	/* SMBus address 0x1040 size 64B */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x40, 0x80001040);

	/* GPIO address 0x1080 size 64B */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x44, 0x80001080);

	/* GPE0 address 0x10C0 size 64B */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x4C, 0x800010C0);

	sch_port_access_write(2, 4, 4, 0x3F703F76); /* FIXME: SMM Control */

	/* Base of Stolen Memory Address 0x1080 size 64B */
	pci_write_config32(PCI_DEV(0, 0x02, 0), 0x5C, 0x3F800000);

	sch_port_access_write(0, 0, 4, DEFAULT_PCIEXBAR | 1); /* pre-b1 */
	sch_port_access_write(2, 9, 4, DEFAULT_PCIEXBAR | 1); /* b1+ */

	/* RCBA */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xF0, (DEFAULT_RCBABASE | 1));

	printk(BIOS_DEBUG, " done.\n");
}

static void sch_early_initialization(void)
{
	/* Print some chipset specific information. */
	sch_detect_chipset();

	/* Setup all non standard BARs. */
	sch_setup_non_standard_bars();
}
