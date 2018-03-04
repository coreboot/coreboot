/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <pc80/i8259.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <elog.h>
#include <cbmem.h>
#include <string.h>
#include "nvs.h"
#include "pch.h"
#include <arch/acpigen.h>
#include <cbmem.h>
#include <drivers/intel/gma/i915.h>

#define NMI_OFF	0

#define ENABLE_ACPI_MODE_IN_COREBOOT	0

typedef struct southbridge_intel_lynxpoint_config config_t;

/**
 * Set miscellanous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void pch_enable_ioapic(struct device *dev)
{
	u32 reg32;

	/* Assign unique bus/dev/fn for I/O APIC */
	pci_write_config16(dev, LPC_IBDF,
		PCH_IOAPIC_PCI_BUS << 8 | PCH_IOAPIC_PCI_SLOT << 3);

	/* Enable ACPI I/O range decode */
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);

	set_ioapic_id(VIO_APIC_VADDR, 0x02);

	/* affirm full set of redirection table entries ("write once") */
	reg32 = io_apic_read(VIO_APIC_VADDR, 0x01);
	if (pch_is_lp()) {
		/* PCH-LP has 39 redirection entries */
		reg32 &= ~0x00ff0000;
		reg32 |= 0x00270000;
	}
	io_apic_write(VIO_APIC_VADDR, 0x01, reg32);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(VIO_APIC_VADDR, 0x03, 0x01);
}

static void pch_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
#if !IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE)
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
#endif
}

/* PIRQ[n]_ROUT[3:0] - PIRQ Routing Control
 * 0x00 - 0000 = Reserved
 * 0x01 - 0001 = Reserved
 * 0x02 - 0010 = Reserved
 * 0x03 - 0011 = IRQ3
 * 0x04 - 0100 = IRQ4
 * 0x05 - 0101 = IRQ5
 * 0x06 - 0110 = IRQ6
 * 0x07 - 0111 = IRQ7
 * 0x08 - 1000 = Reserved
 * 0x09 - 1001 = IRQ9
 * 0x0A - 1010 = IRQ10
 * 0x0B - 1011 = IRQ11
 * 0x0C - 1100 = IRQ12
 * 0x0D - 1101 = Reserved
 * 0x0E - 1110 = IRQ14
 * 0x0F - 1111 = IRQ15
 * PIRQ[n]_ROUT[7] - PIRQ Routing Control
 * 0x80 - The PIRQ is not routed.
 */

static void pch_pirq_init(device_t dev)
{
	device_t irq_dev;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	pci_write_config8(dev, PIRQA_ROUT, config->pirqa_routing);
	pci_write_config8(dev, PIRQB_ROUT, config->pirqb_routing);
	pci_write_config8(dev, PIRQC_ROUT, config->pirqc_routing);
	pci_write_config8(dev, PIRQD_ROUT, config->pirqd_routing);

	pci_write_config8(dev, PIRQE_ROUT, config->pirqe_routing);
	pci_write_config8(dev, PIRQF_ROUT, config->pirqf_routing);
	pci_write_config8(dev, PIRQG_ROUT, config->pirqg_routing);
	pci_write_config8(dev, PIRQH_ROUT, config->pirqh_routing);

	/* Eric Biederman once said we should let the OS do this.
	 * I am not so sure anymore he was right.
	 */

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin=0, int_line=0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		switch (int_pin) {
		case 1: /* INTA# */ int_line = config->pirqa_routing; break;
		case 2: /* INTB# */ int_line = config->pirqb_routing; break;
		case 3: /* INTC# */ int_line = config->pirqc_routing; break;
		case 4: /* INTD# */ int_line = config->pirqd_routing; break;
		}

		if (!int_line)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, int_line);
	}
}

static void pch_gpi_routing(device_t dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	u32 reg32 = 0;

	/* An array would be much nicer here, or some
	 * other method of doing this.
	 */
	reg32 |= (config->gpi0_routing & 0x03) << 0;
	reg32 |= (config->gpi1_routing & 0x03) << 2;
	reg32 |= (config->gpi2_routing & 0x03) << 4;
	reg32 |= (config->gpi3_routing & 0x03) << 6;
	reg32 |= (config->gpi4_routing & 0x03) << 8;
	reg32 |= (config->gpi5_routing & 0x03) << 10;
	reg32 |= (config->gpi6_routing & 0x03) << 12;
	reg32 |= (config->gpi7_routing & 0x03) << 14;
	reg32 |= (config->gpi8_routing & 0x03) << 16;
	reg32 |= (config->gpi9_routing & 0x03) << 18;
	reg32 |= (config->gpi10_routing & 0x03) << 20;
	reg32 |= (config->gpi11_routing & 0x03) << 22;
	reg32 |= (config->gpi12_routing & 0x03) << 24;
	reg32 |= (config->gpi13_routing & 0x03) << 26;
	reg32 |= (config->gpi14_routing & 0x03) << 28;
	reg32 |= (config->gpi15_routing & 0x03) << 30;

	pci_write_config32(dev, GPIO_ROUT, reg32);
}

static void pch_power_options(device_t dev)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;
	u16 pmbase = get_pmbase();
	int pwr_on=CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use Kconfig setting.
	 */
	get_option(&pwr_on, "power_on_after_fail");
	pwr_on = MAINBOARD_POWER_KEEP;

	reg16 = pci_read_config16(dev, GEN_PMCON_3);
	reg16 &= 0xfffe;
	switch (pwr_on) {
	case MAINBOARD_POWER_OFF:
		reg16 |= 1;
		state = "off";
		break;
	case MAINBOARD_POWER_ON:
		reg16 &= ~1;
		state = "on";
		break;
	case MAINBOARD_POWER_KEEP:
		reg16 &= ~1;
		state = "state keep";
		break;
	default:
		state = "undefined";
	}

	reg16 &= ~(3 << 4);	/* SLP_S4# Assertion Stretch 4s */
	reg16 |= (1 << 3);	/* SLP_S4# Assertion Stretch Enable */

	reg16 &= ~(1 << 10);
	reg16 |= (1 << 11);	/* SLP_S3# Min Assertion Width 50ms */

	reg16 |= (1 << 12);	/* Disable SLP stretch after SUS well */

	pci_write_config16(dev, GEN_PMCON_3, reg16);
	printk(BIOS_INFO, "Set power %s after power failure.\n", state);

	/* Set up NMI on errors. */
	reg8 = inb(0x61);
	reg8 &= 0x0f;		/* Higher Nibble must be 0 */
	reg8 &= ~(1 << 3);	/* IOCHK# NMI Enable */
	// reg8 &= ~(1 << 2);	/* PCI SERR# Enable */
	reg8 |= (1 << 2); /* PCI SERR# Disable for now */
	outb(reg8, 0x61);

	reg8 = inb(0x70);
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		printk(BIOS_INFO, "NMI sources enabled.\n");
		reg8 &= ~(1 << 7);	/* Set NMI. */
	} else {
		printk(BIOS_INFO, "NMI sources disabled.\n");
		reg8 |= ( 1 << 7);	/* Can't mask NMI from PCI-E and NMI_NOW */
	}
	outb(reg8, 0x70);

	/* Enable CPU_SLP# and Intel Speedstep, set SMI# rate down */
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~(3 << 0);	// SMI# rate 1 minute
	reg16 &= ~(1 << 10);	// Disable BIOS_PCI_EXP_EN for native PME
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	/*
	 * Set the board's GPI routing on LynxPoint-H.
	 * This is done as part of GPIO configuration on LynxPoint-LP.
	 */
	if (pch_is_lp())
		pch_gpi_routing(dev);

	/* GPE setup based on device tree configuration */
	enable_all_gpe(config->gpe0_en_1, config->gpe0_en_2,
		       config->gpe0_en_3, config->gpe0_en_4);

	/* SMI setup based on device tree configuration */
	enable_alt_smi(config->alt_gp_smi_en);

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT
	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);

	/* Clear magic status bits to prevent unexpected wake */
	reg32 = RCBA32(0x3310);
	reg32 |= (1 << 4)|(1 << 5)|(1 << 0);
	RCBA32(0x3310) = reg32;

	reg16 = RCBA16(0x3f02);
	reg16 &= ~0xf;
	RCBA16(0x3f02) = reg16;
}

static void pch_rtc_init(struct device *dev)
{
	int rtc_failed = rtc_failure();

	if (rtc_failed) {
		if (IS_ENABLED(CONFIG_ELOG))
			elog_add_event(ELOG_TYPE_RTC_RESET);
		pci_update_config8(dev, GEN_PMCON_3, ~RTC_BATTERY_DEAD, 0);
	}

	printk(BIOS_DEBUG, "rtc_failed = 0x%x\n", rtc_failed);

	cmos_init(rtc_failed);
}

/* LynxPoint PCH Power Management init */
static void lpt_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "LynxPoint PM init\n");
}

const struct rcba_config_instruction lpt_lp_pm_rcba[] = {
	RCBA_RMW_REG_32(0x232c, ~1, 0x00000000),
	RCBA_RMW_REG_32(0x1100, ~0xc000, 0xc000),
	RCBA_RMW_REG_32(0x1100, ~0, 0x00000100),
	RCBA_RMW_REG_32(0x1100, ~0, 0x0000003f),
	RCBA_RMW_REG_32(0x2320, ~0x60, 0x10),
	RCBA_RMW_REG_32(0x3314,  0, 0x00012fff),
	RCBA_RMW_REG_32(0x3318,  0, 0x0dcf0400),
	RCBA_RMW_REG_32(0x3324,  0, 0x04000000),
	RCBA_RMW_REG_32(0x3368,  0, 0x00041400),
	RCBA_RMW_REG_32(0x3388,  0, 0x3f8ddbff),
	RCBA_RMW_REG_32(0x33ac,  0, 0x00007001),
	RCBA_RMW_REG_32(0x33b0,  0, 0x00181900),
	RCBA_RMW_REG_32(0x33c0,  0, 0x00060A00),
	RCBA_RMW_REG_32(0x33d0,  0, 0x06200840),
	RCBA_RMW_REG_32(0x3a28,  0, 0x01010101),
	RCBA_RMW_REG_32(0x3a2c,  0, 0x04040404),
	RCBA_RMW_REG_32(0x2b1c,  0, 0x03808033),
	RCBA_RMW_REG_32(0x2b34,  0, 0x80000009),
	RCBA_RMW_REG_32(0x3348,  0, 0x022ddfff),
	RCBA_RMW_REG_32(0x334c,  0, 0x00000001),
	RCBA_RMW_REG_32(0x3358,  0, 0x0001c000),
	RCBA_RMW_REG_32(0x3380,  0, 0x3f8ddbff),
	RCBA_RMW_REG_32(0x3384,  0, 0x0001c7e1),
	RCBA_RMW_REG_32(0x338c,  0, 0x0001c7e1),
	RCBA_RMW_REG_32(0x3398,  0, 0x0001c000),
	RCBA_RMW_REG_32(0x33a8,  0, 0x00181900),
	RCBA_RMW_REG_32(0x33dc,  0, 0x00080000),
	RCBA_RMW_REG_32(0x33e0,  0, 0x00000001),
	RCBA_RMW_REG_32(0x3a20,  0, 0x00000404),
	RCBA_RMW_REG_32(0x3a24,  0, 0x01010101),
	RCBA_RMW_REG_32(0x3a30,  0, 0x01010101),
	RCBA_RMW_REG_32(0x0410, ~0, 0x00000003),
	RCBA_RMW_REG_32(0x2618, ~0, 0x08000000),
	RCBA_RMW_REG_32(0x2300, ~0, 0x00000002),
	RCBA_RMW_REG_32(0x2600, ~0, 0x00000008),
	RCBA_RMW_REG_32(0x33b4,  0, 0x00007001),
	RCBA_RMW_REG_32(0x3350,  0, 0x022ddfff),
	RCBA_RMW_REG_32(0x3354,  0, 0x00000001),
	RCBA_RMW_REG_32(0x33d4, ~0, 0x08000000),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x33c8, ~0, 0x00000080),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x2b10,  0, 0x0000883c),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x2b14,  0, 0x1e0a4616),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x2b24,  0, 0x40000005),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x2b20,  0, 0x0005db01),  /* Power Optimizer */
	RCBA_RMW_REG_32(0x3a80,  0, 0x05145005),
	RCBA_END_CONFIG
};

/* LynxPoint LP PCH Power Management init */
static void lpt_lp_pm_init(struct device *dev)
{
	struct southbridge_intel_lynxpoint_config *config = dev->chip_info;
	u32 data;

	printk(BIOS_DEBUG, "LynxPoint LP PM init\n");

	pci_write_config8(dev, 0xa9, 0x46);

	pch_config_rcba(lpt_lp_pm_rcba);

	pci_write_config32(dev, 0xac,
		pci_read_config32(dev, 0xac) | (1 << 21));

	pch_iobp_update(0xED00015C, ~(1 << 11), 0x00003700);
	pch_iobp_update(0xED000118, ~0UL, 0x00c00000);
	pch_iobp_update(0xED000120, ~0UL, 0x00240000);
	pch_iobp_update(0xCA000000, ~0UL, 0x00000009);

	/* Set RCBA CIR28 0x3A84 based on SATA port enables */
	data = 0x00001005;
	/* Port 3 and 2 disabled */
	if ((config->sata_port_map & ((1 << 3)|(1 << 2))) == 0)
		data |= (1 << 24) | (1 << 26);
	/* Port 1 and 0 disabled */
	if ((config->sata_port_map & ((1 << 1)|(1 << 0))) == 0)
		data |= (1 << 20) | (1 << 18);
	RCBA32(0x3a84) = data;

	/* Set RCBA 0x2b1c[29]=1 if DSP disabled */
	if (RCBA32(FD) & PCH_DISABLE_ADSPD)
		RCBA32_OR(0x2b1c, (1 << 29));

	/* Lock */
	RCBA32_OR(0x3a6c, 0x00000001);

	/* Set RCBA 0x33D4 after other setup */
	RCBA32_OR(0x33d4, 0x2fff2fb1);

	/* Set RCBA 0x33C8[15]=1 as last step */
	RCBA32_OR(0x33c8, (1 << 15));
}

static void enable_hpet(struct device *const dev)
{
	u32 reg32;
	size_t i;

	/* Assign unique bus/dev/fn for each HPET */
	for (i = 0; i < 8; ++i)
		pci_write_config16(dev, LPC_HnBDF(i),
			PCH_HPET_PCI_BUS << 8 | PCH_HPET_PCI_SLOT << 3 | i);

	/* Move HPET to default address 0xfed00000 and enable it */
	reg32 = RCBA32(HPTC);
	reg32 |= (1 << 7); // HPET Address Enable
	reg32 &= ~(3 << 0);
	RCBA32(HPTC) = reg32;
	/* Read it back to stick. It's affected by posted write syndrome. */
	reg32 = RCBA32(HPTC);
}

static void enable_clock_gating(device_t dev)
{
	/* LynxPoint Mobile */
	u32 reg32;
	u16 reg16;

	/* DMI */
	RCBA32_AND_OR(0x2234, ~0UL, 0xf);
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 |= (1 << 11) | (1 << 12) | (1 << 14);
	reg16 |= (1 << 2); // PCI CLKRUN# Enable
	pci_write_config16(dev, GEN_PMCON_1, reg16);
	RCBA32_OR(0x900, (1 << 14));

	reg32 = RCBA32(CG);
	reg32 |= (1 << 22); // HDA Dynamic
	reg32 |= (1UL << 31); // LPC Dynamic
	reg32 |= (1 << 16); // PCIe Dynamic
	reg32 |= (1 << 27); // HPET Dynamic
	reg32 |= (1 << 28); // GPIO Dynamic
	RCBA32(CG) = reg32;

	RCBA32_OR(0x38c0, 0x7); // SPI Dynamic
}

static void enable_lp_clock_gating(device_t dev)
{
	/* LynxPoint LP */
	u32 reg32;
	u16 reg16;

	/* DMI */
	RCBA32_AND_OR(0x2234, ~0UL, 0xf);
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~((1 << 11) | (1 << 14));
	reg16 |= (1 << 5) | (1 << 6) | (1 << 7) | (1 << 12) | (1 << 13);
	reg16 |= (1 << 2); // PCI CLKRUN# Enable
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	reg32 = pci_read_config32(dev, 0x64);
	reg32 |= (1 << 6);
	pci_write_config32(dev, 0x64, reg32);

	/*
	 * RCBA + 0x2614[27:25,14:13,10,8] = 101,11,1,1
	 * RCBA + 0x2614[23:16] = 0x20
	 * RCBA + 0x2614[30:28] = 0x0
	 * RCBA + 0x2614[26] = 1 (IF 0:2.0@0x08 >= 0x0b)
	 */
	RCBA32_AND_OR(0x2614, 0x8bffffff, 0x0a206500);

	/* Check for LPT-LP B2 stepping and 0:31.0@0xFA > 4 */
	if (pci_read_config8(dev_find_slot(0, PCI_DEVFN(2, 0)), 0x8) >= 0x0b)
		RCBA32_OR(0x2614, (1 << 26));

	RCBA32_OR(0x900, 0x0000031f);

	reg32 = RCBA32(CG);
	if (RCBA32(0x3454) & (1 << 4))
		reg32 &= ~(1 << 29); // LPC Dynamic
	else
		reg32 |= (1 << 29); // LPC Dynamic
	reg32 |= (1UL << 31); // LP LPC
	reg32 |= (1 << 30); // LP BLA
	reg32 |= (1 << 28); // GPIO Dynamic
	reg32 |= (1 << 27); // HPET Dynamic
	reg32 |= (1 << 26); // Generic Platform Event Clock
	if (RCBA32(BUC) & PCH_DISABLE_GBE)
		reg32 |= (1 << 23); // GbE Static
	reg32 |= (1 << 22); // HDA Dynamic
	reg32 |= (1 << 16); // PCI Dynamic
	RCBA32(CG) = reg32;

	RCBA32_OR(0x3434, 0x7); // LP LPC

	RCBA32_AND_OR(0x333c, 0xffcfffff, 0x00c00000); // SATA

	RCBA32_OR(0x38c0, 0x3c07); // SPI Dynamic

	pch_iobp_update(0xCF000000, ~0UL, 0x00007001);
	pch_iobp_update(0xCE00C000, ~1UL, 0x00000000); // bit0=0 in BWG 1.4.0
}

static void pch_set_acpi_mode(void)
{
#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	if (!acpi_is_wakeup_s3()) {
#if ENABLE_ACPI_MODE_IN_COREBOOT
		printk(BIOS_DEBUG, "Enabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_ENABLE, APM_CNT);
		printk(BIOS_DEBUG, "done.\n");
#else
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT);
		printk(BIOS_DEBUG, "done.\n");
#endif
	}
#endif /* CONFIG_HAVE_SMI_HANDLER */
}

static void pch_disable_smm_only_flashing(struct device *dev)
{
	u8 reg8;

	printk(BIOS_SPEW, "Enabling BIOS updates outside of SMM... ");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	reg8 &= ~(1 << 5);
	pci_write_config8(dev, 0xdc, reg8);
}

static void pch_fixups(struct device *dev)
{
	u8 gen_pmcon_2;

	/* Indicate DRAM init done for MRC S3 to know it can resume */
	gen_pmcon_2 = pci_read_config8(dev, GEN_PMCON_2);
	gen_pmcon_2 |= (1 << 7);
	pci_write_config8(dev, GEN_PMCON_2, gen_pmcon_2);

	/*
	 * Enable DMI ASPM in the PCH
	 */
	RCBA32_AND_OR(0x2304, ~(1 << 10), 0);
	RCBA32_OR(0x21a4, (1 << 11)|(1 << 10));
	RCBA32_OR(0x21a8, 0x3);
}

static void lpc_init(struct device *dev)
{
	printk(BIOS_DEBUG, "pch: lpc_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	pch_enable_ioapic(dev);

	pch_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	pch_pirq_init(dev);

	/* Setup power options. */
	pch_power_options(dev);

	/* Initialize power management */
	if (pch_is_lp()) {
		lpt_lp_pm_init(dev);
		enable_lp_clock_gating(dev);
	} else {
		lpt_pm_init(dev);
		enable_clock_gating(dev);
	}

	/* Initialize the real time clock. */
	pch_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);

	setup_i8259();

	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	pch_disable_smm_only_flashing(dev);

	pch_set_acpi_mode();

	pch_fixups(dev);
}

static void pch_lpc_add_mmio_resources(device_t dev)
{
	u32 reg;
	struct resource *res;
	const u32 default_decode_base = IO_APIC_ADDR;

	/*
	 * Just report all resources from IO-APIC base to 4GiB. Don't mark
	 * them reserved as that may upset the OS if this range is marked
	 * as reserved in the e820.
	 */
	res = new_resource(dev, OIC);
	res->base = default_decode_base;
	res->size = 0 - default_decode_base;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* RCBA */
	if ((uintptr_t)DEFAULT_RCBA < default_decode_base) {
		res = new_resource(dev, RCBA);
		res->base = (resource_t)(uintptr_t)DEFAULT_RCBA;
		res->size = 16 * 1024;
		res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
		             IORESOURCE_FIXED | IORESOURCE_RESERVE;
	}

	/* Check LPC Memory Decode register. */
	reg = pci_read_config32(dev, LGMR);
	if (reg & 1) {
		reg &= ~0xffff;
		if (reg < default_decode_base) {
			res = new_resource(dev, LGMR);
			res->base = reg;
			res->size = 16 * 1024;
			res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED |
			             IORESOURCE_FIXED | IORESOURCE_RESERVE;
		}
	}
}

/* Default IO range claimed by the LPC device. The upper bound is exclusive. */
#define LPC_DEFAULT_IO_RANGE_LOWER 0
#define LPC_DEFAULT_IO_RANGE_UPPER 0x1000

static inline int pch_io_range_in_default(u16 base, u16 size)
{
	/* Does it start above the range? */
	if (base >= LPC_DEFAULT_IO_RANGE_UPPER)
		return 0;

	/* Is it entirely contained? */
	if (base >= LPC_DEFAULT_IO_RANGE_LOWER &&
	    (base + size) < LPC_DEFAULT_IO_RANGE_UPPER)
		return 1;

	/* This will return not in range for partial overlaps. */
	return 0;
}

/*
 * Note: this function assumes there is no overlap with the default LPC device's
 * claimed range: LPC_DEFAULT_IO_RANGE_LOWER -> LPC_DEFAULT_IO_RANGE_UPPER.
 */
static void pch_lpc_add_io_resource(device_t dev, u16 base, u16 size, int index)
{
	struct resource *res;

	if (pch_io_range_in_default(base, size))
		return;

	res = new_resource(dev, index);
	res->base = base;
	res->size = size;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void pch_lpc_add_gen_io_resources(device_t dev, int reg_value, int index)
{
	/*
	 * Check if the register is enabled. If so and the base exceeds the
	 * device's deafult claim range add the resoure.
	 */
	if (reg_value & 1) {
		u16 base = reg_value & 0xfffc;
		u16 size = (0x3 | ((reg_value >> 16) & 0xfc)) + 1;
		pch_lpc_add_io_resource(dev, base, size, index);
	}
}

static void pch_lpc_add_io_resources(device_t dev)
{
	struct resource *res;
	config_t *config = dev->chip_info;

	/* Add the default claimed IO range for the LPC device. */
	res = new_resource(dev, 0);
	res->base = LPC_DEFAULT_IO_RANGE_LOWER;
	res->size = LPC_DEFAULT_IO_RANGE_UPPER - LPC_DEFAULT_IO_RANGE_LOWER;
	res->flags = IORESOURCE_IO | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* GPIOBASE */
	pch_lpc_add_io_resource(dev, get_gpiobase(), DEFAULT_GPIOSIZE,
	                        GPIO_BASE);

	/* PMBASE */
	pch_lpc_add_io_resource(dev, get_pmbase(), 256, PMBASE);

	/* LPC Generic IO Decode range. */
	pch_lpc_add_gen_io_resources(dev, config->gen1_dec, LPC_GEN1_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen2_dec, LPC_GEN2_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen3_dec, LPC_GEN3_DEC);
	pch_lpc_add_gen_io_resources(dev, config->gen4_dec, LPC_GEN4_DEC);
}

static void pch_lpc_read_resources(device_t dev)
{
	global_nvs_t *gnvs;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add non-standard MMIO resources. */
	pch_lpc_add_mmio_resources(dev);

	/* Add IO resources. */
	pch_lpc_add_io_resources(dev);

	/* Allocate ACPI NVS in CBMEM */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));
	if (!acpi_is_wakeup_s3() && gnvs)
		memset(gnvs, 0, sizeof(global_nvs_t));
}

static void pch_lpc_enable(device_t dev)
{
	/* Enable PCH Display Port */
	RCBA16(DISPBDF) = 0x0010;
	RCBA32_OR(FD2, PCH_ENABLE_DBDF);

	pch_enable(dev);
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static void southbridge_inject_dsdt(device_t dev)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));
		if (gnvs)
			memset(gnvs, 0, sizeof(*gnvs));
	}

	if (gnvs) {
		const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();

		acpi_create_gnvs(gnvs);

		gnvs->apic = 1;
		gnvs->mpen = 1; /* Enable Multi Processing */
		gnvs->pcnt = dev_count_cpu();

#if IS_ENABLED(CONFIG_CHROMEOS)
		chromeos_init_vboot(&(gnvs->chromeos));
#endif

		/* Update the mem console pointer. */
		gnvs->cbmc = (u32)cbmem_find(CBMEM_ID_CONSOLE);

		gnvs->ndid = gfx->ndid;
		memcpy(gnvs->did, gfx->did, sizeof(gnvs->did));

		acpi_save_gnvs((unsigned long)gnvs);
		/* And tell SMI about it */
		smm_setup_structures(gnvs, NULL, NULL);

		/* Add it to DSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (u32) gnvs);
		acpigen_pop_len();
	}
}

static unsigned long southbridge_write_acpi_tables(device_t device,
						   unsigned long start,
						   struct acpi_rsdp *rsdp)
{
	unsigned long current;
	acpi_hpet_t *hpet;
	acpi_header_t *ssdt;

	current = start;

	/* Align ACPI tables to 16byte */
	current = acpi_align_current(current);

	/*
	 * We explicitly add these tables later on:
	 */
	printk(BIOS_DEBUG, "ACPI:    * HPET\n");

	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	current = acpi_align_current(current);
	acpi_create_intel_hpet(hpet);
	acpi_add_table(rsdp, hpet);

	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "ACPI:     * SSDT2\n");
	ssdt = (acpi_header_t *)current;
	acpi_create_serialio_ssdt(ssdt);
	current += ssdt->length;
	acpi_add_table(rsdp, ssdt);
	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}


static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.write_acpi_tables      = southbridge_write_acpi_tables,
	.init			= lpc_init,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &pci_ops,
};


/* IDs for LPC device of Intel 8 Series Chipset (Lynx Point) */
static const unsigned short pci_device_ids[] = {
	0x8c41, /* Mobile Full Featured Engineering Sample. */
	0x8c42, /* Desktop Full Featured Engineering Sample. */
	0x8c44, /* Z87 SKU */
	0x8c46, /* Z85 SKU */
	0x8c49, /* HM86 SKU */
	0x8c4a, /* H87 SKU */
	0x8c4b, /* HM87 SKU */
	0x8c4c, /* Q85 SKU */
	0x8c4e, /* Q87 SKU */
	0x8c4f, /* QM87 SKU */
	0x9c41, /* LP Full Featured Engineering Sample */
	0x9c43, /* LP Premium SKU */
	0x9c45, /* LP Mainstream SKU */
	0x9c47, /* LP Value SKU */
	0 };

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
