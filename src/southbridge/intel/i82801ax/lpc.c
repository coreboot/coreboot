/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2003 SuSE Linux AG
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
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
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include "i82801ax.h"

#define GPIO_BASE_ADDR	0x00000500 /* GPIO Base Address Register */

#define NMI_OFF 0

typedef struct southbridge_intel_i82801ax_config config_t;

/* PIRQ[n]_ROUT[3:0] - IRQ Routing (ISA compatible)
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
 *
 * PIRQ[n]_ROUT[7] - Interrupt Routing Enable (IRQEN)
 * 0 - The PIRQ is routed to the ISA-compatible interrupt specified above.
 * 1 - The PIRQ is not routed to the 8259.
 */

#define PIRQA 0x03
#define PIRQB 0x04
#define PIRQC 0x05
#define PIRQD 0x06

/*
 * Use 0x0ef8 for a bitmap to cover all these IRQ's.
 * Use the defined IRQ values above or set mainboard
 * specific IRQ values in your devicetree.cb.
 */


/**
 * Enable ACPI I/O range.
 *
 * @param dev PCI device with ACPI and PM BAR's
 */
static void i82801ax_enable_acpi(struct device *dev)
{
	/* Set ACPI base address (I/O space). */
	pci_write_config32(dev, PMBASE, (PMBASE_ADDR | 1));

	/* Enable ACPI I/O range decode and ACPI power management. */
	pci_write_config8(dev, ACPI_CNTL, ACPI_EN);
}

/**
 * Set miscellanous static southbridge features.
 *
 * @param dev PCI device with I/O APIC control registers
 */
static void i82801ax_enable_ioapic(struct device *dev)
{
	u32 reg32;

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (1 << 13);	/* Coprocessor error enable (COPR_ERR_EN) */
	reg32 |= (3 << 7);	/* IOAPIC enable (APIC_EN) */
	reg32 |= (1 << 2);	/* DMA collection buffer enable (DCB_EN) */
	reg32 |= (1 << 1);	/* Delayed transaction enable (DTE) */
	pci_write_config32(dev, GEN_CNTL, reg32);
	printk(BIOS_DEBUG, "IOAPIC Southbridge enabled %x\n", reg32);

	set_ioapic_id(VIO_APIC_VADDR, 0x02);

	/*
	 * Select Boot Configuration register (0x03) and
	 * use Processor System Bus (0x01) to deliver interrupts.
	 */
	io_apic_write(VIO_APIC_VADDR, 0x03, 0x01);
}

static void i82801ax_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
	/* TODO: Explain/#define the real meaning of these magic numbers. */
}

static void i82801ax_pirq_init(struct device *dev)
{
	u8 reg8;
	config_t *config = dev->chip_info;

	reg8 = (config->pirqa_routing) ? config->pirqa_routing : PIRQA;
	pci_write_config8(dev, PIRQA_ROUT, reg8);

	reg8 = (config->pirqb_routing) ? config->pirqb_routing : PIRQB;
	pci_write_config8(dev, PIRQB_ROUT, reg8);

	reg8 = (config->pirqc_routing) ? config->pirqc_routing : PIRQC;
	pci_write_config8(dev, PIRQC_ROUT, reg8);

	reg8 = (config->pirqd_routing) ? config->pirqd_routing : PIRQD;
	pci_write_config8(dev, PIRQD_ROUT, reg8);
}

static void i82801ax_power_options(struct device *dev)
{
	uint8_t byte;
	int pwr_on = -1;
	int nmi_option;

	/* power after power fail */
	/* FIXME this doesn't work! */
	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
	pci_write_config8(dev, GEN_PMCON_3, pwr_on ? 0 : 1);
	printk(BIOS_INFO, "Set power %s if power fails\n", pwr_on ? "on" : "off");

	/* Set up NMI on errors. */
	byte = inb(0x61);
	byte &= ~(1 << 3);	/* IOCHK# NMI Enable */
	byte &= ~(1 << 2);	/* PCI SERR# Enable */
	outb(byte, 0x61);
	byte = inb(0x70);

	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7);	/* Set NMI. */
		outb(byte, 0x70);
	}
}

static void gpio_init(struct device *dev)
{
	pci_write_config32(dev, GPIO_BASE, (GPIO_BASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL, GPIO_EN);
}

static void i82801ax_rtc_init(struct device *dev)
{
	uint8_t reg8;
	uint32_t reg32;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~(1 << 1);	/* Preserve the power fail state. */
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	reg32 = pci_read_config32(dev, GEN_STA);
	rtc_failed |= reg32 & (1 << 2);
	cmos_init(rtc_failed);

	/* Enable access to the upper 128 byte bank of CMOS RAM. */
	pci_write_config8(dev, RTC_CONF, 0x04);
}

static void i82801ax_lpc_route_dma(struct device *dev, uint8_t mask)
{
	uint16_t reg16;
	int i;

	reg16 = pci_read_config16(dev, PCI_DMA_CFG);
	reg16 &= 0x300;
	for (i = 0; i < 8; i++) {
		if (i == 4)
			continue;
		reg16 |= ((mask & (1 << i)) ? 3 : 1) << (i * 2);
	}
	pci_write_config16(dev, PCI_DMA_CFG, reg16);
}

static void i82801ax_lpc_decode_en(struct device *dev)
{
	/* Decode 0x3F8-0x3FF (COM1) for COMA port, 0x2F8-0x2FF (COM2) for COMB.
	 * LPT decode defaults to 0x378-0x37F and 0x778-0x77F.
	 * Floppy decode defaults to 0x3F0-0x3F5, 0x3F7.
	 * We also need to set the value for LPC I/F Enables Register.
	 */
	pci_write_config8(dev, COM_DEC, 0x10);
	pci_write_config16(dev, LPC_EN, 0x300F);
}

static void lpc_init(struct device *dev)
{
	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	i82801ax_enable_acpi(dev);
	/* IO APIC initialization. */
	i82801ax_enable_ioapic(dev);

	i82801ax_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801ax_pirq_init(dev);

	/* Setup power options. */
	i82801ax_power_options(dev);

	/* Set the state of the GPIO lines. */
	gpio_init(dev);

	/* Initialize the real time clock. */
	i82801ax_rtc_init(dev);

	/* Route DMA. */
	i82801ax_lpc_route_dma(dev, 0xff);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Setup decode ports and LPC I/F enables. */
	i82801ax_lpc_decode_en(dev);
}

static void i82801ax_lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal PCI resources of this device. */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = 0xff800000;
	res->size = 0x00800000; /* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations lpc_ops = {
	.read_resources		= i82801ax_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_lpc_bus,
	.enable			= i82801ax_enable,
};

/* 82801AA (ICH) */
static const struct pci_driver i82801aa_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2410,
};

/* 82801AB (ICH0) */
static const struct pci_driver i82801ab_lpc __pci_driver = {
	.ops	= &lpc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2420,
};
