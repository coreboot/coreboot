/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "i82801gx.h"

#include "../../../northbridge/intel/i945/ich7.h"

#define MAINBOARD_POWER_OFF 0
#define MAINBOARD_POWER_ON  1

#ifndef MAINBOARD_POWER_ON_AFTER_FAIL
#define MAINBOARD_POWER_ON_AFTER_FAIL MAINBOARD_POWER_ON
#endif

#define NMI_OFF 0

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

#define PIRQA 0x03
#define PIRQB 0x05
#define PIRQC 0x06
#define PIRQD 0x07
#define PIRQE 0x09
#define PIRQF 0x0A
#define PIRQG 0x0B
#define PIRQH 0x0C

static void i82801gx_enable_apic(struct device *dev)
{
	int i;
	u32 reg32;
	volatile u32 *ioapic_index = (volatile u32 *)0xfec00000;
	volatile u32 *ioapic_data = (volatile u32 *)0xfec00010;

	/* Enable ACPI I/O and power management. */
	pci_write_config8(dev, ACPI_CNTL, 0x80);

	*ioapic_index = 0;
	*ioapic_data = (1 << 25);

	*ioapic_index = 0;
	reg32 = *ioapic_data;
	printk_debug("Southbridge APIC ID = %x\n", (reg32 >> 24) & 0x0f);
	if (reg32 != (1 << 25))
		die("APIC Error\n");

	printk_spew("Dumping IOAPIC registers\n");
	for (i=0; i<3; i++) {
		*ioapic_index = i;
		printk_spew("  reg 0x%04x:", i);
		reg32 = *ioapic_data;
		printk_spew(" 0x%08x\n", reg32);
	}

	*ioapic_index = 3; /* Select Boot Configuration register. */
	*ioapic_data = 1; /* Use Processor System Bus to deliver interrupts. */
}

static void i82801gx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit for one frame. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
}

static void i82801gx_pirq_init(device_t dev)
{
	pci_write_config8(dev, PIRQA_ROUT, 0x85);
	pci_write_config8(dev, PIRQB_ROUT, 0x87);
	pci_write_config8(dev, PIRQC_ROUT, 0x86);
	pci_write_config8(dev, PIRQD_ROUT, 0x87);

	pci_write_config8(dev, PIRQE_ROUT, 0x80);
	pci_write_config8(dev, PIRQF_ROUT, 0x80);
	pci_write_config8(dev, PIRQG_ROUT, 0x80);
	pci_write_config8(dev, PIRQH_ROUT, 0x85);
}

static void i82801gx_power_options(device_t dev)
{
	u8 reg8;
	u16 reg16;

	int pwr_on=MAINBOARD_POWER_ON_AFTER_POWER_FAIL;
	int nmi_option;

	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
        get_option(&pwr_on, "power_on_after_fail");
	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	reg8 &= 0xfe;
	if (pwr_on) {
		reg8 &= ~1;
	} else {
		reg8 |= 1;
	}
	reg8 |= (3 << 4);	/* avoid #S4 assertions */

	pci_write_config8(dev, GEN_PMCON_3, reg8);
	printk_info("Set power %s after power failure.\n", pwr_on ? "on" : "off");

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
		printk_info ("NMI sources enabled.\n");
		reg8 &= ~(1 << 7);	/* Set NMI. */
	} else {
		printk_info ("NMI sources disabled.\n");
		reg8 |= ( 1 << 7);	/* Can't mask NMI from PCI-E and NMI_NOW */
	}
	outb(reg8, 0x70);

	// Enable CPU_SLP# and Intel Speedstep, set SMI# rate down
	reg16 = pci_read_config16(dev, GEN_PMCON_1);
	reg16 &= ~3;
	reg16 |= (1 << 3) | (1 << 5) | (1 << 10);
	pci_write_config16(dev, GEN_PMCON_1, reg16);

	// Set GPIO13 to SCI (?)
	// This might be board specific
	pci_write_config32(dev, 0xb8, 0x08000000);
}

void i82801gx_rtc_init(struct device *dev)
{
	u8 reg8;
	u32 reg32;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~RTC_BATTERY_DEAD;
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	printk_debug("rtc_failed = 0x%x\n", rtc_failed);

	rtc_init(rtc_failed);
}

static void enable_hpet(struct device *dev)
{
	u32 reg32;
	u32 code = (0 & 0x3);

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (1 << 17);	/* Enable HPET. */
	/*
	 * Bits [16:15]	Memory Address Range
	 * 00		FED0_0000h - FED0_03FFh
	 * 01		FED0_1000h - FED0_13FFh
	 * 10		FED0_2000h - FED0_23FFh
	 * 11		FED0_3000h - FED0_33FFh
	 */
	reg32 &= ~(3 << 15);	/* Clear it */
	reg32 |= (code << 15);
	/* TODO: reg32 is never written to anywhere? */
	printk_debug("Enabling HPET @0x%x\n", HPET_ADDR | (code << 12));
}

static void i82801gx_lock_smm(struct device *dev)
{
	void smm_lock(void);
	u8 reg8;

#if ENABLE_ACPI_MODE_IN_COREBOOT
	printk_debug("Enabling ACPI via APMC:\n");
	outb(0xe1, 0xb2); // Enable ACPI mode
	printk_debug("done.\n");
#else
	printk_debug("Disabling ACPI via APMC:\n");
	outb(0x1e, 0xb2); // Disable ACPI mode
	printk_debug("done.\n");
#endif
	/* Don't allow evil boot loaders, kernels, or 
	 * userspace applications to deceive us:
	 */
	smm_lock();

#if TEST_SMM_FLASH_LOCKDOWN
	/* Now try this: */
	printk_debug("Locking BIOS to RO... ");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk_debug(" BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");
	reg8 &= ~(1 << 0);			/* clear BIOSWE */
	pci_write_config8(dev, 0xdc, reg8);
	reg8 |= (1 << 1);			/* set BLE */
	pci_write_config8(dev, 0xdc, reg8);
	printk_debug("ok.\n");
	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk_debug(" BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");

	printk_debug("Writing:\n");
	*(volatile u8 *)0xfff00000 = 0x00;
	printk_debug("Testing:\n");
	reg8 |= (1 << 0);			/* set BIOSWE */
	pci_write_config8(dev, 0xdc, reg8);

	reg8 = pci_read_config8(dev, 0xdc);	/* BIOS_CNTL */
	printk_debug(" BLE: %s; BWE: %s\n", (reg8&2)?"on":"off",
			(reg8&1)?"rw":"ro");
	printk_debug("Done.\n");
#endif
}

static void lpc_init(struct device *dev)
{
	printk_debug("i82801gx: lpc_init\n");

	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	i82801gx_enable_apic(dev);

	i82801gx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801gx_pirq_init(dev);

	/* Setup power options. */
	i82801gx_power_options(dev);

	/* Set the state of the GPIO lines. */
	//gpio_init(dev);

	/* Initialize the real time clock. */
	i82801gx_rtc_init(dev);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Initialize the High Precision Event Timers, if present. */
	enable_hpet(dev);

	setup_i8259();

	i82801gx_lock_smm(dev);
}

static void i82801gx_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->flags =
	    IORESOURCE_IO | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->flags =
	    IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE | IORESOURCE_ASSIGNED;
}

static void i82801gx_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static void set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	printk_debug("Setting LPC bridge subsystem ID\n");
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
			pci_read_config32(dev, 0));
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};

static struct device_operations device_ops = {
	.read_resources		= i82801gx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= i82801gx_lpc_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.enable			= i82801gx_enable,
	.ops_pci		= &pci_ops,
};

/* ICH7 / ICH7R */
static const struct pci_driver ich7_ich7r_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27b8,
};

/* ICH7M / ICH7U */
static const struct pci_driver ich7m_ich7u_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27b9,
};

/* ICH7M DH */
static const struct pci_driver ich7m_dh_lpc __pci_driver = {
	.ops	= &device_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x27bd,
};

