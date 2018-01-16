/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
#include <elog.h>
#include <arch/acpigen.h>
#include <drivers/intel/gma/i915.h>
#include <cpu/x86/smm.h>
#include <cbmem.h>
#include <string.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/common/rcba.h>
#include "pch.h"
#include "nvs.h"
#include <southbridge/intel/common/pciehp.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>

#define NMI_OFF	0

#define ENABLE_ACPI_MODE_IN_COREBOOT	0

typedef struct southbridge_intel_bd82x6x_config config_t;

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
	/* Interrupt 11 is not used by legacy devices and so can always be used for
	   PCI interrupts. Full legacy IRQ routing is complicated and hard to
	   get right. Fortunately all modern OS use MSI and so it's not that big of
	   an issue anyway. Still we have to provide a reasonable default. Using
	   interrupt 11 for it everywhere is a working default. ACPI-aware OS can
	   move it to any interrupt and others will just leave them at default.
	 */
	const u8 pirq_routing = 11;

	pci_write_config8(dev, PIRQA_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQB_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQC_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQD_ROUT, pirq_routing);

	pci_write_config8(dev, PIRQE_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQF_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQG_ROUT, pirq_routing);
	pci_write_config8(dev, PIRQH_ROUT, pirq_routing);

	for (irq_dev = all_devices; irq_dev; irq_dev = irq_dev->next) {
		u8 int_pin=0;

		if (!irq_dev->enabled || irq_dev->path.type != DEVICE_PATH_PCI)
			continue;

		int_pin = pci_read_config8(irq_dev, PCI_INTERRUPT_PIN);

		if (int_pin == 0)
			continue;

		pci_write_config8(irq_dev, PCI_INTERRUPT_LINE, pirq_routing);
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
	u16 reg16, pmbase;
	u32 reg32;
	const char *state;
	/* Get the chip configuration */
	config_t *config = dev->chip_info;

	int pwr_on=CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 *
	 * If the option is not existent (Laptops), use Kconfig setting.
	 */
	get_option(&pwr_on, "power_on_after_fail");

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
#if DEBUG_PERIODIC_SMIS
	/* Set DEBUG_PERIODIC_SMIS in pch.h to debug using
	 * periodic SMIs.
	 */
	reg16 |= (3 << 0); // Periodic SMI every 8s
#endif
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	// Set the board's GPI routing.
	pch_gpi_routing(dev);

	pmbase = pci_read_config16(dev, 0x40) & 0xfffe;

	outl(config->gpe0_en, pmbase + GPE0_EN);
	outw(config->alt_gp_smi_en, pmbase + ALT_GP_SMI_EN);

	/* Set up power management block and determine sleep mode */
	reg32 = inl(pmbase + 0x04); // PM1_CNT
	reg32 &= ~(7 << 10);	// SLP_TYP
	reg32 |= (1 << 0);	// SCI_EN
	outl(reg32, pmbase + 0x04);

	/* Clear magic status bits to prevent unexpected wake */
	reg32 = RCBA32(0x3310);
	reg32 |= (1 << 4)|(1 << 5)|(1 << 0);
	RCBA32(0x3310) = reg32;

	reg32 = RCBA32(0x3f02);
	reg32 &= ~0xf;
	RCBA32(0x3f02) = reg32;
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

/* CougarPoint PCH Power Management init */
static void cpt_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "CougarPoint PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);
	RCBA32_AND_OR(0x2238, ~0UL, (1 << 6)|(1 << 0));
	RCBA32_AND_OR(0x228c, ~0UL, (1 << 0));
	RCBA16_AND_OR(0x1100, ~0UL, (1 << 13)|(1 << 14));
	RCBA16_AND_OR(0x0900, ~0UL, (1 << 14));
	RCBA32(0x2304) = 0xc0388400;
	RCBA32_AND_OR(0x2314, ~0UL, (1 << 5)|(1 << 18));
	RCBA32_AND_OR(0x2320, ~0UL, (1 << 15)|(1 << 1));
	RCBA32_AND_OR(0x3314, ~0x1f, 0xf);
	RCBA32(0x3318) = 0x050f0000;
	RCBA32(0x3324) = 0x04000000;
	RCBA32_AND_OR(0x3340, ~0UL, 0xfffff);
	RCBA32_AND_OR(0x3344, ~0UL, (1 << 1));
	RCBA32(0x3360) = 0x0001c000;
	RCBA32(0x3368) = 0x00061100;
	RCBA32(0x3378) = 0x7f8fdfff;
	RCBA32(0x337c) = 0x000003fc;
	RCBA32(0x3388) = 0x00001000;
	RCBA32(0x3390) = 0x0001c000;
	RCBA32(0x33a0) = 0x00000800;
	RCBA32(0x33b0) = 0x00001000;
	RCBA32(0x33c0) = 0x00093900;
	RCBA32(0x33cc) = 0x24653002;
	RCBA32(0x33d0) = 0x062108fe;
	RCBA32_AND_OR(0x33d4, 0xf000f000, 0x00670060);
	RCBA32(0x3a28) = 0x01010000;
	RCBA32(0x3a2c) = 0x01010404;
	RCBA32(0x3a80) = 0x01041041;
	RCBA32_AND_OR(0x3a84, ~0x0000ffff, 0x00001001);
	RCBA32_AND_OR(0x3a84, ~0UL, (1 << 24)); /* SATA 2/3 disabled */
	RCBA32_AND_OR(0x3a88, ~0UL, (1 << 0));  /* SATA 4/5 disabled */
	RCBA32(0x3a6c) = 0x00000001;
	RCBA32_AND_OR(0x2344, 0x00ffff00, 0xff00000c);
	RCBA32_AND_OR(0x80c, ~(0xff << 20), 0x11 << 20);
	RCBA32(0x33c8) = 0;
	RCBA32_AND_OR(0x21b0, ~0UL, 0xf);
}

/* PantherPoint PCH Power Management init */
static void ppt_pm_init(struct device *dev)
{
	printk(BIOS_DEBUG, "PantherPoint PM init\n");
	pci_write_config8(dev, 0xa9, 0x47);
	RCBA32_AND_OR(0x2238, ~0UL, (1 << 0));
	RCBA32_AND_OR(0x228c, ~0UL, (1 << 0));
	RCBA16_AND_OR(0x1100, ~0UL, (1 << 13)|(1 << 14));
	RCBA16_AND_OR(0x0900, ~0UL, (1 << 14));
	RCBA32(0x2304) = 0xc03b8400;
	RCBA32_AND_OR(0x2314, ~0UL, (1 << 5)|(1 << 18));
	RCBA32_AND_OR(0x2320, ~0UL, (1 << 15)|(1 << 1));
	RCBA32_AND_OR(0x3314, ~0x1f, 0xf);
	RCBA32(0x3318) = 0x054f0000;
	RCBA32(0x3324) = 0x04000000;
	RCBA32_AND_OR(0x3340, ~0UL, 0xfffff);
	RCBA32_AND_OR(0x3344, ~0UL, (1 << 1)|(1 << 0));
	RCBA32(0x3360) = 0x0001c000;
	RCBA32(0x3368) = 0x00061100;
	RCBA32(0x3378) = 0x7f8fdfff;
	RCBA32(0x337c) = 0x000003fd;
	RCBA32(0x3388) = 0x00001000;
	RCBA32(0x3390) = 0x0001c000;
	RCBA32(0x33a0) = 0x00000800;
	RCBA32(0x33b0) = 0x00001000;
	RCBA32(0x33c0) = 0x00093900;
	RCBA32(0x33cc) = 0x24653002;
	RCBA32(0x33d0) = 0x067388fe;
	RCBA32_AND_OR(0x33d4, 0xf000f000, 0x00670060);
	RCBA32(0x3a28) = 0x01010000;
	RCBA32(0x3a2c) = 0x01010404;
	RCBA32(0x3a80) = 0x01040000;
	RCBA32_AND_OR(0x3a84, ~0x0000ffff, 0x00001001);
	RCBA32_AND_OR(0x3a84, ~0UL, (1 << 24)); /* SATA 2/3 disabled */
	RCBA32_AND_OR(0x3a88, ~0UL, (1 << 0));  /* SATA 4/5 disabled */
	RCBA32(0x3a6c) = 0x00000001;
	RCBA32_AND_OR(0x2344, 0x00ffff00, 0xff00000c);
	RCBA32_AND_OR(0x80c, ~(0xff << 20), 0x11 << 20);
	RCBA32_AND_OR(0x33a4, ~0UL, (1 << 0));
	RCBA32(0x33c8) = 0;
	RCBA32_AND_OR(0x21b0, ~0UL, 0xf);
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
}

static void enable_clock_gating(device_t dev)
{
	u32 reg32;
	u16 reg16;

	RCBA32_AND_OR(0x2234, ~0UL, 0xf);

	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 |= (1 << 2) | (1 << 11);
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	pch_iobp_update(0xEB007F07, ~0UL, (1 << 31));
	pch_iobp_update(0xEB004000, ~0UL, (1 << 7));
	pch_iobp_update(0xEC007F07, ~0UL, (1 << 31));
	pch_iobp_update(0xEC004000, ~0UL, (1 << 7));

	reg32 = RCBA32(CG);
	reg32 |= (1 << 31);
	reg32 |= (1 << 29) | (1 << 28);
	reg32 |= (1 << 27) | (1 << 26) | (1 << 25) | (1 << 24);
	reg32 |= (1 << 16);
	reg32 |= (1 << 17);
	reg32 |= (1 << 18);
	reg32 |= (1 << 22);
	reg32 |= (1 << 23);
	reg32 &= ~(1 << 20);
	reg32 |= (1 << 19);
	reg32 |= (1 << 0);
	reg32 |= (0xf << 1);
	RCBA32(CG) = reg32;

	RCBA32_OR(0x38c0, 0x7);
	RCBA32_OR(0x36d4, 0x6680c004);
	RCBA32_OR(0x3564, 0x3);
}

static void pch_set_acpi_mode(void)
{
	if (!acpi_is_wakeup_s3() && CONFIG_HAVE_SMI_HANDLER) {
#if ENABLE_ACPI_MODE_IN_COREBOOT
		printk(BIOS_DEBUG, "Enabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_ENABLE, APM_CNT); // Enable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#else
		printk(BIOS_DEBUG, "Disabling ACPI via APMC:\n");
		outb(APM_CNT_ACPI_DISABLE, APM_CNT); // Disable ACPI mode
		printk(BIOS_DEBUG, "done.\n");
#endif
	}
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

static void pch_decode_init(struct device *dev)
{
	config_t *config = dev->chip_info;

	printk(BIOS_DEBUG, "pch_decode_init\n");

	pci_write_config32(dev, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(dev, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(dev, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(dev, LPC_GEN4_DEC, config->gen4_dec);
}

static void pch_spi_init(const struct device *const dev)
{
	const config_t *const config = dev->chip_info;

	printk(BIOS_DEBUG, "pch_spi_init\n");

	if (config->spi_uvscc)
		RCBA32(0x3800 + 0xc8) = config->spi_uvscc;
	if (config->spi_lvscc)
		RCBA32(0x3800 + 0xc4) = config->spi_lvscc;

	if (config->spi_uvscc || config->spi_lvscc)
		RCBA32_OR(0x3800 + 0xc4, 1 << 23); /* lock both UVSCC + LVSCC */
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
	switch (pch_silicon_type()) {
	case PCH_TYPE_CPT: /* CougarPoint */
		cpt_pm_init(dev);
		break;
	case PCH_TYPE_PPT: /* PantherPoint */
		ppt_pm_init(dev);
		break;
	default:
		printk(BIOS_ERR, "Unknown Chipset: 0x%04x\n", dev->device);
	}

	/* Set the state of the GPIO lines. */
	//gpio_init(dev);

	/* Initialize the real time clock. */
	pch_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);

	/* Initialize Clock Gating */
	enable_clock_gating(dev);

	setup_i8259();

	/* The OS should do this? */
	/* Interrupt 9 should be level triggered (SCI) */
	i8259_configure_irq_trigger(9, 1);

	pch_disable_smm_only_flashing(dev);

	pch_set_acpi_mode();

	pch_fixups(dev);

	pch_spi_init(dev);
}

static void pch_lpc_read_resources(device_t dev)
{
	struct resource *res;
	config_t *config = dev->chip_info;
	u8 io_index = 0;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
	res->base = 0xff000000;
	/* Some systems (e.g. X230) have 12 MiB flash.
	   SPI controller supports up to 2 x 16 MiB of flash but
	   address map limits this to 16MiB.  */
	res->size = 0x01000000; /* 16 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Set PCH IO decode ranges if required.*/
	if ((config->gen1_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen1_dec & 0xFFFC;
		res->size = (config->gen1_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen2_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen2_dec & 0xFFFC;
		res->size = (config->gen2_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen3_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen3_dec & 0xFFFC;
		res->size = (config->gen3_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}

	if ((config->gen4_dec & 0xFFFC) > 0x1000) {
		res = new_resource(dev, IOINDEX_SUBTRACTIVE(io_index++, 0));
		res->base = config->gen4_dec & 0xFFFC;
		res->size = (config->gen4_dec >> 16) & 0xFC;
		res->flags = IORESOURCE_IO| IORESOURCE_SUBTRACTIVE |
				 IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
	}
}

static void pch_lpc_enable_resources(device_t dev)
{
	pch_decode_init(dev);
	return pci_dev_enable_resources(dev);
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
	global_nvs_t *gnvs = cbmem_add (CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));

	if (gnvs) {
		const struct i915_gpu_controller_info *gfx = intel_gma_get_controller_info();
		memset(gnvs, 0, sizeof(*gnvs));

		acpi_create_gnvs(gnvs);

		gnvs->apic = 1;
		gnvs->mpen = 1; /* Enable Multi Processing */
		gnvs->pcnt = dev_count_cpu();

		gnvs->ndid = gfx->ndid;
		memcpy(gnvs->did, gfx->did, sizeof(gnvs->did));

#if IS_ENABLED(CONFIG_CHROMEOS)
		chromeos_init_vboot(&(gnvs->chromeos));
#endif

		/* And tell SMI about it */
		smm_setup_structures(gnvs, NULL, NULL);

		/* Add it to DSDT.  */
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (u32) gnvs);
		acpigen_pop_len();
	}
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	config_t *chip = dev->chip_info;
	u16 pmbase = pci_read_config16(dev, 0x40) & 0xfffe;
	int c2_latency;

	fadt->model = 1;

	fadt->sci_int = 0x9;
	fadt->smi_cmd = APM_CNT;
	fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
	fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	fadt->s4bios_req = 0x0;
	fadt->pstate_cnt = 0;

	fadt->pm1a_evt_blk = pmbase;
	fadt->pm1b_evt_blk = 0x0;
	fadt->pm1a_cnt_blk = pmbase + 0x4;
	fadt->pm1b_cnt_blk = 0x0;
	fadt->pm2_cnt_blk = pmbase + 0x50;
	fadt->pm_tmr_blk = pmbase + 0x8;
	fadt->gpe0_blk = pmbase + 0x20;
	fadt->gpe1_blk = 0;

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	fadt->gpe0_blk_len = 16;
	fadt->gpe1_blk_len = 0;
	fadt->gpe1_base = 0;
	fadt->cst_cnt = 0;
	c2_latency = chip->c2_latency;
	if (!c2_latency) {
		c2_latency = 101; /* c2 unsupported */
	}
	fadt->p_lvl2_lat = c2_latency;
	fadt->p_lvl3_lat = 87;
	fadt->flush_size = 1024;
	fadt->flush_stride = 16;
	fadt->duty_offset = 1;
	if (chip->p_cnt_throttling_supported) {
		fadt->duty_width = 3;
	} else {
		fadt->duty_width = 0;
	}
	fadt->day_alrm = 0xd;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags = ACPI_FADT_WBINVD |
			ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_RESET_REGISTER |
			ACPI_FADT_SEALED_CASE |
			ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_PLATFORM_CLOCK;
	if (chip->docking_supported) {
		fadt->flags |= ACPI_FADT_DOCKING_SUPPORTED;
	}
	if (c2_latency < 100) {
		fadt->flags |= ACPI_FADT_C2_MP_SUPPORTED;
	}

	fadt->reset_reg.space_id = 1;
	fadt->reset_reg.bit_width = 8;
	fadt->reset_reg.bit_offset = 0;
	fadt->reset_reg.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->reset_reg.addrl = 0xcf9;
	fadt->reset_reg.addrh = 0;

	fadt->reset_value = 6;

	fadt->x_pm1a_evt_blk.space_id = 1;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = pmbase;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	fadt->x_pm1b_evt_blk.space_id = 1;
	fadt->x_pm1b_evt_blk.bit_width = 0;
	fadt->x_pm1b_evt_blk.bit_offset = 0;
	fadt->x_pm1b_evt_blk.access_size = 0;
	fadt->x_pm1b_evt_blk.addrl = 0x0;
	fadt->x_pm1b_evt_blk.addrh = 0x0;

	fadt->x_pm1a_cnt_blk.space_id = 1;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + 0x4;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	fadt->x_pm1b_cnt_blk.space_id = 1;
	fadt->x_pm1b_cnt_blk.bit_width = 0;
	fadt->x_pm1b_cnt_blk.bit_offset = 0;
	fadt->x_pm1b_cnt_blk.access_size = 0;
	fadt->x_pm1b_cnt_blk.addrl = 0x0;
	fadt->x_pm1b_cnt_blk.addrh = 0x0;

	fadt->x_pm2_cnt_blk.space_id = 1;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = pmbase + 0x50;
	fadt->x_pm2_cnt_blk.addrh = 0x0;

	fadt->x_pm_tmr_blk.space_id = 1;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = pmbase + 0x8;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	fadt->x_gpe0_blk.space_id = 1;
	fadt->x_gpe0_blk.bit_width = 128;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_gpe0_blk.addrl = pmbase + 0x20;
	fadt->x_gpe0_blk.addrh = 0x0;

	fadt->x_gpe1_blk.space_id = 1;
	fadt->x_gpe1_blk.bit_width = 0;
	fadt->x_gpe1_blk.bit_offset = 0;
	fadt->x_gpe1_blk.access_size = 0;
	fadt->x_gpe1_blk.addrl = 0x0;
	fadt->x_gpe1_blk.addrh = 0x0;
}

static const char *lpc_acpi_name(const struct device *dev)
{
	return "LPCB";
}

static void southbridge_fill_ssdt(device_t device)
{
	device_t dev = dev_find_slot(0, PCI_DEVFN(0x1f,0));
	config_t *chip = dev->chip_info;

	intel_acpi_pcie_hotplug_generator(chip->pcie_hotplug_map, 8);
	intel_acpi_gen_def_acpi_pirq(dev);
}

static void lpc_final(struct device *dev)
{
	/* Call SMM finalize() handlers before resume */
	if (IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)) {
		if (IS_ENABLED(CONFIG_INTEL_CHIPSET_LOCKDOWN) ||
		    acpi_is_wakeup_s3()) {
			outb(APM_CNT_FINALIZE, APM_CNT);
		}
	}
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= pch_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pch_lpc_enable_resources,
	.write_acpi_tables      = acpi_write_hpet,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.acpi_fill_ssdt_generator = southbridge_fill_ssdt,
	.acpi_name		= lpc_acpi_name,
	.init			= lpc_init,
	.final			= lpc_final,
	.enable			= pch_lpc_enable,
	.scan_bus		= scan_lpc_bus,
	.ops_pci		= &pci_ops,
};


/* IDs for LPC device of Intel 6 Series Chipset, Intel 7 Series Chipset, and
 * Intel C200 Series Chipset
 */

static const unsigned short pci_device_ids[] = {
	0x1c40, 0x1c41, 0x1c42, 0x1c43, 0x1c44, 0x1c45, 0x1c46, 0x1c47, 0x1c48,
	0x1c49, 0x1c4a, 0x1c4b, 0x1c4c, 0x1c4d, 0x1c4e, 0x1c4f, 0x1c50, 0x1c51,
	0x1c52, 0x1c53, 0x1c54, 0x1c55, 0x1c56, 0x1c57, 0x1c58, 0x1c59, 0x1c5a,
	0x1c5b, 0x1c5c, 0x1c5d, 0x1c5e, 0x1c5f,

	0x1e41, 0x1e42, 0x1e43, 0x1e44, 0x1e45, 0x1e46, 0x1e47, 0x1e48, 0x1e49,
	0x1e4a, 0x1e4b, 0x1e4c, 0x1e4d, 0x1e4e, 0x1e4f, 0x1e50, 0x1e51, 0x1e52,
	0x1e53, 0x1e54, 0x1e55, 0x1e56, 0x1e57, 0x1e58, 0x1e59, 0x1e5a, 0x1e5b,
	0x1e5c, 0x1e5d, 0x1e5e, 0x1e5f,

	0 };

static const struct pci_driver pch_lpc __pci_driver = {
	.ops	 = &device_ops,
	.vendor	 = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
