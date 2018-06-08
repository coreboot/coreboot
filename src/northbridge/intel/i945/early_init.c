/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
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

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <cbmem.h>
#include <halt.h>
#include <romstage_handoff.h>
#include <string.h>
#include "i945.h"
#include <pc80/mc146818rtc.h>
#include <southbridge/intel/common/gpio.h>

int i945_silicon_revision(void)
{
	return pci_read_config8(PCI_DEV(0, 0x00, 0), PCI_CLASS_REVISION);
}

static void i945m_detect_chipset(void)
{
	u8 reg8;

	printk(BIOS_INFO, "\n");
	reg8 = (pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe7) & 0x70) >> 4;
	switch (reg8) {
	case 1:
		printk(BIOS_INFO, "Mobile Intel(R) 82945GM/GME Express");
		break;
	case 2:
		printk(BIOS_INFO, "Mobile Intel(R) 82945GMS/GU/GSE Express");
		break;
	case 3:
		printk(BIOS_INFO, "Mobile Intel(R) 82945PM Express");
		break;
	case 5:
		printk(BIOS_INFO, "Intel(R) 82945GT Express");
		break;
	case 6:
		printk(BIOS_INFO, "Mobile Intel(R) 82943/82940GML Express");
		break;
	default:
		printk(BIOS_INFO, "Unknown (%02x)", reg8);	/* Others reserved. */
	}
	printk(BIOS_INFO, " Chipset\n");

	printk(BIOS_DEBUG, "(G)MCH capable of up to FSB ");
	reg8 = (pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe3) & 0xe0) >> 5;
	switch (reg8) {
	case 2:
		printk(BIOS_DEBUG, "800 MHz"); /* According to 965 spec */
		break;
	case 3:
		printk(BIOS_DEBUG, "667 MHz");
		break;
	case 4:
		printk(BIOS_DEBUG, "533 MHz");
		break;
	default:
		printk(BIOS_DEBUG, "N/A MHz (%02x)", reg8);
	}
	printk(BIOS_DEBUG, "\n");

	printk(BIOS_DEBUG, "(G)MCH capable of ");
	reg8 = (pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe4) & 0x07);
	switch (reg8) {
	case 2:
		printk(BIOS_DEBUG, "up to DDR2-667");
		break;
	case 3:
		printk(BIOS_DEBUG, "up to DDR2-533");
		break;
	case 4:
		printk(BIOS_DEBUG, "DDR2-400");
		break;
	default:
		printk(BIOS_INFO, "unknown max. RAM clock (%02x).", reg8);	/* Others reserved. */
	}
	printk(BIOS_DEBUG, "\n");

	if (IS_ENABLED(CONFIG_NORTHBRIDGE_INTEL_SUBTYPE_I945GC))
		printk(BIOS_ERR, "coreboot is compiled for the wrong chipset.\n");
}

static void i945_detect_chipset(void)
{
	u8 reg8;

	printk(BIOS_INFO, "\nIntel(R) ");

	reg8 = ((pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe7) >> 5) & 4) | ((pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe4) >> 4) & 3);
	switch (reg8) {
	case 0:
	case 1:
		printk(BIOS_INFO, "82945G");
		break;
	case 2:
	case 3:
		printk(BIOS_INFO, "82945P");
		break;
	case 4:
		printk(BIOS_INFO, "82945GC");
		break;
	case 5:
		printk(BIOS_INFO, "82945GZ");
		break;
	case 6:
	case 7:
		printk(BIOS_INFO, "82945PL");
		break;
	default:
		break;
	}
	printk(BIOS_INFO, " Chipset\n");

	printk(BIOS_DEBUG, "(G)MCH capable of ");
	reg8 = (pci_read_config8(PCI_DEV(0, 0x00, 0), 0xe4) & 0x07);
	switch (reg8) {
	case 0:
	case 2:
		printk(BIOS_DEBUG, "up to DDR2-667");
		break;
	case 3:
		printk(BIOS_DEBUG, "up to DDR2-533");
		break;
	default:
		printk(BIOS_INFO, "unknown max. RAM clock (%02x).", reg8);	/* Others reserved. */
	}
	printk(BIOS_DEBUG, "\n");

	if (IS_ENABLED(CONFIG_NORTHBRIDGE_INTEL_SUBTYPE_I945GM))
		printk(BIOS_ERR, "coreboot is compiled for the wrong chipset.\n");
}

static void i945_setup_bars(void)
{
	u8 reg8, gfxsize;

	/* As of now, we don't have all the A0 workarounds implemented */
	if (i945_silicon_revision() == 0)
		printk(BIOS_INFO, "Warning: i945 silicon revision A0 might not work correctly.\n");

	/* Setting up Southbridge. In the northbridge code. */
	printk(BIOS_DEBUG, "Setting up static southbridge registers...");
	pci_write_config32(PCI_DEV(0, 0x1f, 0), RCBA, (uintptr_t)DEFAULT_RCBA | 1);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), PMBASE, DEFAULT_PMBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x44, 0x80); /* ACPI_CNTL: Enable ACPI BAR */

	pci_write_config32(PCI_DEV(0, 0x1f, 0), GPIOBASE, DEFAULT_GPIOBASE | 1);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x4c, 0x10);	/* GC: Enable GPIOs */
	setup_pch_gpios(&mainboard_gpio_map);
	printk(BIOS_DEBUG, " done.\n");

	printk(BIOS_DEBUG, "Disabling Watchdog reboot...");
	RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
	outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	outw((1 <<  3), DEFAULT_PMBASE | 0x60 | 0x04);	/* clear timeout */
	outw((1 <<  1), DEFAULT_PMBASE | 0x60 | 0x06);	/* clear 2nd timeout */
	printk(BIOS_DEBUG, " done.\n");

	/* Enable upper 128bytes of CMOS */
	RCBA32(RC) = (1 << 2);

	printk(BIOS_DEBUG, "Setting up static northbridge registers...");
	/* Set up all hardcoded northbridge BARs */
	pci_write_config32(PCI_DEV(0, 0x00, 0), EPBAR, DEFAULT_EPBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), MCHBAR, (uintptr_t)DEFAULT_MCHBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), DMIBAR, (uintptr_t)DEFAULT_DMIBAR | 1);
	pci_write_config32(PCI_DEV(0, 0x00, 0), X60BAR, DEFAULT_X60BAR | 1);

	/* vram size from cmos option */
	if (get_option(&gfxsize, "gfx_uma_size") != CB_SUCCESS)
		gfxsize = 2;	/* 2 for 8MB */
	/* make sure no invalid setting is used */
	if (gfxsize > 6)
		gfxsize = 2;
	pci_write_config16(PCI_DEV(0, 0x00, 0), GGC, ((gfxsize + 1) << 4));

	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), ESMRAMC);
	reg8 &= ~0x7;
	reg8 |= (2 << 1) | (1 << 0); /* 8M and TSEG_Enable */
	pci_write_config8(PCI_DEV(0, 0, 0), ESMRAMC, reg8);

	/* Set C0000-FFFFF to access RAM on both reads and writes */
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM0, 0x30);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM1, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM2, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM3, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM4, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM5, 0x33);
	pci_write_config8(PCI_DEV(0, 0x00, 0), PAM6, 0x33);

	printk(BIOS_DEBUG, " done.\n");

	/* Wait for MCH BAR to come up */
	printk(BIOS_DEBUG, "Waiting for MCHBAR to come up...");
	if ((pci_read_config32(PCI_DEV(0, 0x00, 0), 0xe4) & 0x20000) == 0x00) { /* Bit 49 of CAPID0 */
		do {
			reg8 = *(volatile u8 *)0xfed40000;
		} while (!(reg8 & 0x80));
	}
	printk(BIOS_DEBUG, "ok\n");
}

static void i945_setup_egress_port(void)
{
	u32 reg32;
	u32 timeout;

	printk(BIOS_DEBUG, "Setting up Egress Port RCRB\n");

	/* Egress Port Virtual Channel 0 Configuration */

	/* map only TC0 to VC0 */
	reg32 = EPBAR32(EPVC0RCTL);
	reg32 &= 0xffffff01;
	EPBAR32(EPVC0RCTL) = reg32;

	reg32 = EPBAR32(EPPVCCAP1);
	reg32 &= ~(7 << 0);
	reg32 |= 1;
	EPBAR32(EPPVCCAP1) = reg32;

	/* Egress Port Virtual Channel 1 Configuration */
	reg32 = EPBAR32(0x2c);
	reg32 &= 0xffffff00;
	if (IS_ENABLED(CONFIG_NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
		if ((MCHBAR32(CLKCFG) & 7) == 0)
			reg32 |= 0x1a;	/* 1067MHz */
	}
	if ((MCHBAR32(CLKCFG) & 7) == 1)
		reg32 |= 0x0d;	/* 533MHz */
	if ((MCHBAR32(CLKCFG) & 7) == 2)
		reg32 |= 0x14;	/* 800MHz */
	if ((MCHBAR32(CLKCFG) & 7) == 3)
		reg32 |= 0x10;	/* 667MHz */
	EPBAR32(0x2c) = reg32;

	EPBAR32(EPVC1MTS) = 0x0a0a0a0a;

	reg32 = EPBAR32(EPVC1RCAP);
	reg32 &= ~(0x7f << 16);
	reg32 |= (0x0a << 16);
	EPBAR32(EPVC1RCAP) = reg32;

	if (IS_ENABLED(CONFIG_NORTHBRIDGE_INTEL_SUBTYPE_I945GC)) {
		if ((MCHBAR32(CLKCFG) & 7) == 0) {	/* 1067MHz */
			EPBAR32(EPVC1IST + 0) = 0x01380138;
			EPBAR32(EPVC1IST + 4) = 0x01380138;
		}
	}

	if ((MCHBAR32(CLKCFG) & 7) == 1) {	/* 533MHz */
		EPBAR32(EPVC1IST + 0) = 0x009c009c;
		EPBAR32(EPVC1IST + 4) = 0x009c009c;
	}

	if ((MCHBAR32(CLKCFG) & 7) == 2) {	/* 800MHz */
		EPBAR32(EPVC1IST + 0) = 0x00f000f0;
		EPBAR32(EPVC1IST + 4) = 0x00f000f0;
	}

	if ((MCHBAR32(CLKCFG) & 7) == 3) {	/* 667MHz */
		EPBAR32(EPVC1IST + 0) = 0x00c000c0;
		EPBAR32(EPVC1IST + 4) = 0x00c000c0;
	}

	/* Is internal graphics enabled? */
	if (pci_read_config8(PCI_DEV(0, 0x0, 0), DEVEN) & (DEVEN_D2F0 | DEVEN_D2F1))
		MCHBAR32(MMARB1) |= (1 << 17);

	/* Assign Virtual Channel ID 1 to VC1 */
	reg32 = EPBAR32(EPVC1RCTL);
	reg32 &= ~(7 << 24);
	reg32 |= (1 << 24);
	EPBAR32(EPVC1RCTL) = reg32;

	reg32 = EPBAR32(EPVC1RCTL);
	reg32 &= 0xffffff01;
	reg32 |= (1 << 7);
	EPBAR32(EPVC1RCTL) = reg32;

	EPBAR32(PORTARB + 0x00) = 0x01000001;
	EPBAR32(PORTARB + 0x04) = 0x00040000;
	EPBAR32(PORTARB + 0x08) = 0x00001000;
	EPBAR32(PORTARB + 0x0c) = 0x00000040;
	EPBAR32(PORTARB + 0x10) = 0x01000001;
	EPBAR32(PORTARB + 0x14) = 0x00040000;
	EPBAR32(PORTARB + 0x18) = 0x00001000;
	EPBAR32(PORTARB + 0x1c) = 0x00000040;

	EPBAR32(EPVC1RCTL) |= (1 << 16);
	EPBAR32(EPVC1RCTL) |= (1 << 16);

	printk(BIOS_DEBUG, "Loading port arbitration table ...");
	/* Loop until bit 0 becomes 0 */
	timeout = 0x7fffff;
	while ((EPBAR16(EPVC1RSTS) & 1) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "ok\n");

	/* Now enable VC1 */
	EPBAR32(EPVC1RCTL) |= (1 << 31);

	printk(BIOS_DEBUG, "Wait for VC1 negotiation ...");
	/* Wait for VC1 negotiation pending */
	timeout = 0x7fff;
	while ((EPBAR16(EPVC1RSTS) & (1 << 1)) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "ok\n");

}

static void ich7_setup_dmi_rcrb(void)
{
	u16 reg16;
	u32 reg32;

	reg16 = RCBA16(LCTL);
	reg16 &= ~(3 << 0);
	reg16 |= 3;
	RCBA16(LCTL) = reg16;

	RCBA32(V0CTL) = 0x80000001;
	RCBA32(V1CAP) = 0x03128010;
	RCBA32(ESD) = 0x00000810;
	RCBA32(RP1D) = 0x01000003;
	RCBA32(RP2D) = 0x02000002;
	RCBA32(RP3D) = 0x03000002;
	RCBA32(RP4D) = 0x04000002;
	RCBA32(HDD) = 0x0f000003;
	RCBA32(RP5D) = 0x05000002;

	RCBA32(RPFN) = 0x00543210;

	pci_write_config16(PCI_DEV(0, 0x1c, 0), 0x42, 0x0141);
	pci_write_config16(PCI_DEV(0, 0x1c, 4), 0x42, 0x0141);
	pci_write_config16(PCI_DEV(0, 0x1c, 5), 0x42, 0x0141);

	pci_write_config32(PCI_DEV(0, 0x1c, 4), 0x54, 0x00480ce0);
	pci_write_config32(PCI_DEV(0, 0x1c, 5), 0x54, 0x00500ce0);

	reg32 = RCBA32(V1CTL);
	reg32 &= ~((0x7f << 1) | (7 << 17) | (7 << 24));
	reg32 |= (0x40 << 1) | (4 << 17) | (1 << 24) | (1 << 31);
	RCBA32(V1CTL) = reg32;

	RCBA32(ESD) |= (2 << 16);

	RCBA32(ULD) |= (1 << 24) | (1 << 16);

	RCBA32(ULBA) = (uintptr_t)DEFAULT_DMIBAR;

	RCBA32(RP1D) |= (2 << 16);
	RCBA32(RP2D) |= (2 << 16);
	RCBA32(RP3D) |= (2 << 16);
	RCBA32(RP4D) |= (2 << 16);
	RCBA32(HDD)  |= (2 << 16);
	RCBA32(RP5D) |= (2 << 16);
	RCBA32(RP6D) |= (2 << 16);

	RCBA32(LCAP) |= (3 << 10);
}

static void i945_setup_dmi_rcrb(void)
{
	u32 reg32;
	u32 timeout;
	int activate_aspm = 1; /* hardcode ASPM for now */

	printk(BIOS_DEBUG, "Setting up DMI RCRB\n");

	/* Virtual Channel 0 Configuration */
	reg32 = DMIBAR32(DMIVC0RCTL0);
	reg32 &= 0xffffff01;
	DMIBAR32(DMIVC0RCTL0) = reg32;

	reg32 = DMIBAR32(DMIPVCCAP1);
	reg32 &= ~(7 << 0);
	reg32 |= 1;
	DMIBAR32(DMIPVCCAP1) = reg32;

	reg32 = DMIBAR32(DMIVC1RCTL);
	reg32 &= ~(7 << 24);
	reg32 |= (1 << 24);	/* NOTE: This ID must match ICH7 side */
	DMIBAR32(DMIVC1RCTL) = reg32;

	reg32 = DMIBAR32(DMIVC1RCTL);
	reg32 &= 0xffffff01;
	reg32 |= (1 << 7);
	DMIBAR32(DMIVC1RCTL) = reg32;

	/* Now enable VC1 */
	DMIBAR32(DMIVC1RCTL) |= (1 << 31);

	printk(BIOS_DEBUG, "Wait for VC1 negotiation ...");
	/* Wait for VC1 negotiation pending */
	timeout = 0x7ffff;
	while ((DMIBAR16(DMIVC1RSTS) & (1 << 1)) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "done..\n");
#if 1
	/* Enable Active State Power Management (ASPM) L0 state */

	reg32 = DMIBAR32(DMILCAP);
	reg32 &= ~(7 << 12);
	reg32 |= (2 << 12);

	reg32 &= ~(7 << 15);

	reg32 |= (2 << 15);
	DMIBAR32(DMILCAP) = reg32;

	reg32 = DMIBAR32(DMICC);
	reg32 &= 0x00ffffff;
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	reg32 &= ~(3 << 20);
	reg32 |= (1 << 20);

	DMIBAR32(DMICC) = reg32;

	if (activate_aspm)
		DMIBAR32(DMILCTL) |= (3 << 0);
#endif

	/* Last but not least, some additional steps */
	reg32 = MCHBAR32(FSBSNPCTL);
	reg32 &= ~(0xff << 2);
	reg32 |= (0xaa << 2);
	MCHBAR32(FSBSNPCTL) = reg32;

	DMIBAR32(0x2c) = 0x86000040;

	reg32 = DMIBAR32(0x204);
	reg32 &= ~0x3ff;
#if 1
	reg32 |= 0x13f;		/* for x4 DMI only */
#else
	reg32 |= 0x1e4; /* for x2 DMI only */
#endif
	DMIBAR32(0x204) = reg32;

	if (pci_read_config8(PCI_DEV(0, 0x0, 0), DEVEN) & (DEVEN_D2F0 | DEVEN_D2F1)) {
		printk(BIOS_DEBUG, "Internal graphics: enabled\n");
		DMIBAR32(0x200) |= (1 << 21);
	} else {
		printk(BIOS_DEBUG, "Internal graphics: disabled\n");
		DMIBAR32(0x200) &= ~(1 << 21);
	}

	reg32 = DMIBAR32(0x204);
	reg32 &= ~((1 << 11) | (1 << 10));
	DMIBAR32(0x204) = reg32;

	reg32 = DMIBAR32(0x204);
	reg32 &= ~(0xff << 12);
	reg32 |= (0x0d << 12);
	DMIBAR32(0x204) = reg32;

	DMIBAR32(DMICTL1) |= (3 << 24);

	reg32 = DMIBAR32(0x200);
	reg32 &= ~(0x3 << 26);
	reg32 |= (0x02 << 26);
	DMIBAR32(0x200) = reg32;

	DMIBAR32(DMIDRCCFG) &= ~(1 << 31);
	DMIBAR32(DMICTL2) |= (1 << 31);

	if (i945_silicon_revision() >= 3) {
		reg32 = DMIBAR32(0xec0);
		reg32 &= 0x0fffffff;
		reg32 |= (2 << 28);
		DMIBAR32(0xec0) = reg32;

		reg32 = DMIBAR32(0xed4);
		reg32 &= 0x0fffffff;
		reg32 |= (2 << 28);
		DMIBAR32(0xed4) = reg32;

		reg32 = DMIBAR32(0xee8);
		reg32 &= 0x0fffffff;
		reg32 |= (2 << 28);
		DMIBAR32(0xee8) = reg32;

		reg32 = DMIBAR32(0xefc);
		reg32 &= 0x0fffffff;
		reg32 |= (2 << 28);
		DMIBAR32(0xefc) = reg32;
	}

	/* wait for bit toggle to 0 */
	printk(BIOS_DEBUG, "Waiting for DMI hardware...");
	timeout = 0x7fffff;
	while ((DMIBAR8(0x32) & (1 << 1)) && --timeout)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "ok\n");

	/* Clear Error Status Bits! */
	DMIBAR32(0x1c4) = 0xffffffff;
	DMIBAR32(0x1d0) = 0xffffffff;
	DMIBAR32(0x228) = 0xffffffff;

	/* Program Read-Only Write-Once Registers */
	DMIBAR32(0x308) = DMIBAR32(0x308);
	DMIBAR32(0x314) = DMIBAR32(0x314);
	DMIBAR32(0x324) = DMIBAR32(0x324);
	DMIBAR32(0x328) = DMIBAR32(0x328);
	DMIBAR32(0x338) = DMIBAR32(0x334);
	DMIBAR32(0x338) = DMIBAR32(0x338);

	if (i945_silicon_revision() == 1 && (MCHBAR8(DFT_STRAP1) & (1 << 5))) {
		if ((MCHBAR32(0x214) & 0xf) != 0x3) {
			printk(BIOS_INFO, "DMI link requires A1 stepping workaround. Rebooting.\n");
			reg32 = DMIBAR32(0x224);
			reg32 &= ~(7 << 0);
			reg32 |= (3 << 0);
			DMIBAR32(0x224) = reg32;
			outb(0x06, 0xcf9);
			halt(); /* wait for reset */
		}
	}
}

static void i945_setup_pci_express_x16(void)
{
	u32 timeout;
	u32 reg32;
	u16 reg16;

	u8 reg8;

	printk(BIOS_DEBUG, "Enabling PCI Express x16 Link\n");

	reg16 = pci_read_config16(PCI_DEV(0, 0x00, 0), DEVEN);
	reg16 |= DEVEN_D1F0;
	pci_write_config16(PCI_DEV(0, 0x00, 0), DEVEN, reg16);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x208);
	reg32 &= ~(1 << 8);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x208, reg32);

	/* We have no success with querying the usual PCIe registers
	 * for link setup success on the i945. Hence we assign a temporary
	 * PCI bus 0x0a and check whether we find a device on 0:a.0
	 */

	/* First we reset the secondary bus */
	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0x3e);
	reg16 |= (1 << 6); /* SRESET */
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0x3e, reg16);
	/* Read back and clear reset bit. */
	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0x3e);
	reg16 &= ~(1 << 6); /* SRESET */
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0x3e, reg16);

	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0xba);
	printk(BIOS_DEBUG, "SLOTSTS: %04x\n", reg16);
	if (!(reg16 & 0x48))
		goto disable_pciexpress_x16_link;
	reg16 |= (1 << 4) | (1 << 0);
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0xba, reg16);

	pci_write_config8(PCI_DEV(0, 0x01, 0), 0x19, 0x00);
	pci_write_config8(PCI_DEV(0, 0x01, 0), 0x1a, 0x00);
	pci_write_config8(PCI_DEV(0, 0x01, 0), 0x19, 0x0a);
	pci_write_config8(PCI_DEV(0, 0x01, 0), 0x1a, 0x0a);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x224);
	reg32 &= ~(1 << 8);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x224, reg32);

	MCHBAR16(UPMC1) &= ~((1 << 5) | (1 << 0));

	/* Initialize PEG_CAP */
	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0xa2);
	reg16 |= (1 << 8);
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0xa2, reg16);

	/* Setup SLOTCAP */
	/* TODO: These values are mainboard dependent and should
	 * be set from devicetree.cb.
	 */
	/* NOTE: SLOTCAP becomes RO after the first write! */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xb4);
	reg32 &= 0x0007ffff;

	reg32 &= 0xfffe007f;

	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xb4, reg32);

	/* Wait for training to succeed */
	printk(BIOS_DEBUG, "PCIe link training ...");
	timeout = 0x7ffff;
	while ((((pci_read_config32(PCI_DEV(0, 0x01, 0), PEGSTS) >> 16) & 3) != 3)
		&& --timeout)
		;

	reg32 = pci_read_config32(PCI_DEV(0x0a, 0x0, 0), 0);
	if (reg32 != 0x00000000 && reg32 != 0xffffffff) {
		printk(BIOS_DEBUG, " Detected PCIe device %04x:%04x\n",
				reg32 & 0xffff, reg32 >> 16);
	} else {
		printk(BIOS_DEBUG, " timeout!\n");

		printk(BIOS_DEBUG, "Restrain PCIe port to x1\n");

		reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), PEGSTS);
		reg32 &= ~(0xf << 1);
		reg32 |= 1;
		pci_write_config32(PCI_DEV(0, 0x01, 0), PEGSTS, reg32);

		reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0x3e);

		reg16 |= (1 << 6);
		pci_write_config16(PCI_DEV(0, 0x01, 0), 0x3e, reg16);
		reg16 &= ~(1 << 6);
		pci_write_config16(PCI_DEV(0, 0x01, 0), 0x3e, reg16);

		printk(BIOS_DEBUG, "PCIe link training ...");
		timeout = 0x7ffff;
		while ((((pci_read_config32(PCI_DEV(0, 0x01, 0), PEGSTS) >> 16) & 3) != 3)
			&& --timeout)
			;

		reg32 = pci_read_config32(PCI_DEV(0xa, 0x00, 0), 0);
		if (reg32 != 0x00000000 && reg32 != 0xffffffff) {
			printk(BIOS_DEBUG, " Detected PCIe x1 device %04x:%04x\n",
				reg32 & 0xffff, reg32 >> 16);
		} else {
			printk(BIOS_DEBUG, " timeout!\n");
			printk(BIOS_DEBUG, "Disabling PCIe x16 port completely.\n");
			goto disable_pciexpress_x16_link;
		}
	}

	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), 0xb2);
	reg16 >>= 4;
	reg16 &= 0x3f;
	/* reg16 == 1 -> x1; reg16 == 16 -> x16 */
	printk(BIOS_DEBUG, "PCIe x%d link training succeeded.\n", reg16);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x204);
	reg32 &= 0xfffffc00; /* clear [9:0] */
	if (reg16 == 1)
		reg32 |= 0x32b;
		// TODO
		/* pci_write_config32(PCI_DEV(0, 0x01, 0), 0x204, reg32); */
	else if (reg16 == 16)
		reg32 |= 0x0f4;
		// TODO
		/* pci_write_config32(PCI_DEV(0, 0x01, 0), 0x204, reg32); */

	reg32 = (pci_read_config32(PCI_DEV(0xa, 0, 0), 0x8) >> 8);
	printk(BIOS_DEBUG, "PCIe device class: %06x\n", reg32);
	if (reg32 == 0x030000) {
		printk(BIOS_DEBUG, "PCIe device is VGA. Disabling IGD.\n");
		reg16 = (1 << 1);
		pci_write_config16(PCI_DEV(0, 0x0, 0), 0x52, reg16);

		reg32 = pci_read_config32(PCI_DEV(0, 0x0, 0), DEVEN);
		reg32 &= ~(DEVEN_D2F0 | DEVEN_D2F1);
		pci_write_config32(PCI_DEV(0, 0x0, 0), DEVEN, reg32);

		/* Set VGA enable bit in PCIe bridge */
		reg16 = pci_read_config16(PCI_DEV(0, 0x1, 0), 0x3e);
		reg16 |= (1 << 3);
		pci_write_config16(PCI_DEV(0, 0x1, 0), 0x3e, reg16);
	}

	/* Enable GPEs */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xec);
	reg32 |= (1 << 2) | (1 << 1) | (1 << 0); /* PMEGPE, HPGPE, GENGPE */
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x114, reg32);

	/* Virtual Channel Configuration: Only VC0 on PCIe x16 */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x114);
	reg32 &= 0xffffff01;
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x114, reg32);

	/* Extended VC count */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x104);
	reg32 &= ~(7 << 0);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x104, reg32);

	/* Active State Power Management ASPM */

	/* TODO */

	/* Clear error bits */
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0x06, 0xffff);
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0x1e, 0xffff);
	pci_write_config16(PCI_DEV(0, 0x01, 0), 0xaa, 0xffff);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x1c4, 0xffffffff);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x1d0, 0xffffffff);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x1f0, 0xffffffff);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x228, 0xffffffff);

	/* Program R/WO registers */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x308);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x308, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x314);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x314, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x324);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x324, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x328);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x328, reg32);

	reg8 = pci_read_config8(PCI_DEV(0, 0x01, 0), 0xb4);
	pci_write_config8(PCI_DEV(0, 0x01, 0), 0xb4, reg8);

	/* Additional PCIe graphics setup */
	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xf0);
	reg32 |= (3 << 26);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xf0, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xf0);
	reg32 |= (3 << 24);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xf0, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xf0);
	reg32 |= (1 << 5);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xf0, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x200);
	reg32 &= ~(3 << 26);
	reg32 |= (2 << 26);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x200, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xe80);
	if (i945_silicon_revision() >= 2)
		reg32 |= (1 << 12);
	else
		reg32 &= ~(1 << 12);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xe80, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xeb4);
	reg32 &= ~(1 << 31);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xeb4, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xfc);
	reg32 |= (1 << 31);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0xfc, reg32);

	if (i945_silicon_revision() >= 3) {
		static const u32 reglist[] = {
			0xec0, 0xed4, 0xee8, 0xefc, 0xf10, 0xf24,
			0xf38, 0xf4c, 0xf60, 0xf74, 0xf88, 0xf9c,
			0xfb0, 0xfc4, 0xfd8, 0xfec
		};

		int i;
		for (i = 0; i < ARRAY_SIZE(reglist); i++) {
			reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), reglist[i]);
			reg32 &= 0x0fffffff;
			reg32 |= (2 << 28);
			pci_write_config32(PCI_DEV(0, 0x01, 0), reglist[i], reg32);
		}
	}

	if (i945_silicon_revision() <= 2) {
		/* Set voltage specific parameters */
		reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0xe80);
		reg32 &= (0xf << 4);	/* Default case 1.05V */
		if ((MCHBAR32(DFT_STRAP1) & (1 << 20)) == 0) {	/* 1.50V */
			reg32 |= (7 << 4);
		}
		pci_write_config32(PCI_DEV(0, 0x01, 0), 0xe80, reg32);
	}

	return;

disable_pciexpress_x16_link:
	/* For now we just disable the x16 link */
	printk(BIOS_DEBUG, "Disabling PCI Express x16 Link\n");

	MCHBAR16(UPMC1) |= (1 << 5) | (1 << 0);

	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), BCTRL1);
	reg16 |= (1 << 6);
	pci_write_config16(PCI_DEV(0, 0x01, 0), BCTRL1, reg16);

	reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x224);
	reg32 |= (1 << 8);
	pci_write_config32(PCI_DEV(0, 0x01, 0), 0x224, reg32);

	reg16 = pci_read_config16(PCI_DEV(0, 0x01, 0), BCTRL1);
	reg16 &= ~(1 << 6);
	pci_write_config16(PCI_DEV(0, 0x01, 0), BCTRL1, reg16);

	printk(BIOS_DEBUG, "Wait for link to enter detect state... ");
	timeout = 0x7fffff;
	for (reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), PEGSTS);
	     (reg32 & 0x000f0000) && --timeout;)
		;
	if (!timeout)
		printk(BIOS_DEBUG, "timeout!\n");
	else
		printk(BIOS_DEBUG, "ok\n");

	/* Finally: Disable the PCI config header */
	reg16 = pci_read_config16(PCI_DEV(0, 0x00, 0), DEVEN);
	reg16 &= ~DEVEN_D1F0;
	pci_write_config16(PCI_DEV(0, 0x00, 0), DEVEN, reg16);
}

static void i945_setup_root_complex_topology(void)
{
	u32 reg32;

	printk(BIOS_DEBUG, "Setting up Root Complex Topology\n");
	/* Egress Port Root Topology */

	reg32 = EPBAR32(EPESD);
	reg32 &= 0xff00ffff;
	reg32 |= (1 << 16);
	EPBAR32(EPESD) = reg32;

	EPBAR32(EPLE1D) |= (1 << 16) | (1 << 0);

	EPBAR32(EPLE1A) = (uintptr_t)DEFAULT_DMIBAR;

	EPBAR32(EPLE2D) |= (1 << 16) | (1 << 0);

	/* DMI Port Root Topology */

	reg32 = DMIBAR32(DMILE1D);
	reg32 &= 0x00ffffff;

	reg32 &= 0xff00ffff;
	reg32 |= (2 << 16);

	reg32 |= (1 << 0);
	DMIBAR32(DMILE1D) = reg32;

	DMIBAR32(DMILE1A) = (uintptr_t)DEFAULT_RCBA;

	DMIBAR32(DMILE2D) |= (1 << 16) | (1 << 0);

	DMIBAR32(DMILE2A) = DEFAULT_EPBAR;

	/* PCI Express x16 Port Root Topology */
	if (pci_read_config8(PCI_DEV(0, 0x00, 0), DEVEN) & DEVEN_D1F0) {
		pci_write_config32(PCI_DEV(0, 0x01, 0), 0x158, DEFAULT_EPBAR);
		reg32 = pci_read_config32(PCI_DEV(0, 0x01, 0), 0x150);
		reg32 |= (1 << 0);
		pci_write_config32(PCI_DEV(0, 0x01, 0), 0x150, reg32);
	}
}

static void ich7_setup_root_complex_topology(void)
{
	RCBA32(0x104) = 0x00000802;
	RCBA32(0x110) = 0x00000001;
	RCBA32(0x114) = 0x00000000;
	RCBA32(0x118) = 0x00000000;
}

static void ich7_setup_pci_express(void)
{
	RCBA32(CG) |= (1 << 0);

	/* Initialize slot power limit for root ports */
	pci_write_config32(PCI_DEV(0, 0x1c, 0), 0x54, 0x00000060);
#if 0
	pci_write_config32(PCI_DEV(0, 0x1c, 4), 0x54, 0x00480ce0);
	pci_write_config32(PCI_DEV(0, 0x1c, 5), 0x54, 0x00500ce0);
#endif

	pci_write_config32(PCI_DEV(0, 0x1c, 0), 0xd8, 0x00110000);
}

void i945_early_initialization(void)
{
	/* Print some chipset specific information */
	switch (pci_read_config32(PCI_DEV(0, 0x00, 0), 0)) {
	case 0x27708086: /* 82945G/GZ/GC/P/PL */
		i945_detect_chipset();
		break;
	case 0x27a08086: /* 945GME/GSE */
	case 0x27ac8086: /* 945GM/PM/GMS/GU/GT, 943/940GML */
		i945m_detect_chipset();
		break;
	}

	/* Setup all BARs required for early PCIe and raminit */
	i945_setup_bars();

	/* Change port80 to LPC */
	RCBA32(GCS) &= (~0x04);

	/* Just do it that way */
	RCBA32(0x2010) |= (1 << 10);
}

static void i945_prepare_resume(int s3resume)
{
	int cbmem_was_initted;

	cbmem_was_initted = !cbmem_recovery(s3resume);

	romstage_handoff_init(cbmem_was_initted && s3resume);
}

void i945_late_initialization(int s3resume)
{
	i945_setup_egress_port();

	ich7_setup_root_complex_topology();

	ich7_setup_pci_express();

	ich7_setup_dmi_rcrb();

	i945_setup_dmi_rcrb();

	if (IS_ENABLED(CONFIG_NORTHBRIDGE_INTEL_SUBTYPE_I945GM))
		i945_setup_pci_express_x16();

	i945_setup_root_complex_topology();

#if !IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
#if IS_ENABLED(CONFIG_DEBUG_RAM_SETUP)
	sdram_dump_mchbar_registers();

	{
		/* This will not work if TSEG is in place! */
		u32 tom = pci_read_config32(PCI_DEV(0, 2, 0), BSM);

		printk(BIOS_DEBUG, "TOM: 0x%08x\n", tom);
		ram_check(0x00000000, 0x000a0000);
		ram_check(0x00100000, tom);
	}
#endif
#endif
#endif

	MCHBAR16(SSKPD) = 0xCAFE;

	i945_prepare_resume(s3resume);
}
