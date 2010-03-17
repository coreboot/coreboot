/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2004 SuSE Linux AG
 * Copyright (C) 2004 Tyan Computer
 * Copyright (C) 2010 Joseph Smith <joe@settoplinux.org>
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include "i82801dx.h"

#define NMI_OFF 0

typedef struct southbridge_intel_i82801dx_config config_t;

static void i82801dx_enable_ioapic(struct device *dev)
{
	u32 reg32;
	volatile u32 *ioapic_index = (volatile u32 *)(IO_APIC_ADDR);
	volatile u32 *ioapic_data = (volatile u32 *)(IO_APIC_ADDR + 0x10);

	/* Set ACPI base address (I/O space). */
	pci_write_config32(dev, PMBASE, (PMBASE_ADDR | 1));

	/* Enable ACPI I/O and power management. */
	pci_write_config8(dev, ACPI_CNTL, 0x10);

	reg32 = pci_read_config32(dev, GEN_CNTL);
	reg32 |= (3 << 7);	/* Enable IOAPIC */
	reg32 |= (1 << 13);	/* Coprocessor error enable */
	reg32 |= (1 << 1);	/* Delayed transaction enable */
	reg32 |= (1 << 2);	/* DMA collection buffer enable */
	pci_write_config32(dev, GEN_CNTL, reg32);
	printk_debug("IOAPIC Southbridge enabled %x\n", reg32);

	*ioapic_index = 0;
	*ioapic_data = (1 << 25);

	*ioapic_index = 0;
	reg32 = *ioapic_data;
	printk_debug("Southbridge APIC ID = %x\n", reg32);
	if (reg32 != (1 << 25))
		die("APIC Error\n");

	/* TODO: From i82801ca, needed/useful on other ICH? */
	*ioapic_index = 3; /* Select Boot Configuration register. */
	*ioapic_data = 1; /* Use Processor System Bus to deliver interrupts. */
}

static void i82801dx_enable_serial_irqs(struct device *dev)
{
	/* Set packet length and toggle silent mode bit. */
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (0 << 6) | ((21 - 17) << 2) | (0 << 0));
}

static void i82801dx_pirq_init(device_t dev)
{
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

}

static void i82801dx_power_options(device_t dev)
{
	u8 byte;
	int pwr_on = -1;
	int nmi_option;

	/* power after power fail */
	/* FIXME this doesn't work! */
	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
	pci_write_config8(dev, GEN_PMCON_3, pwr_on ? 0 : 1);
	printk_info("Set power %s if power fails\n", pwr_on ? "on" : "off");

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

static void gpio_init(device_t dev)
{
	/* This should be done in romstage.c already */
	pci_write_config32(dev, GPIO_BASE, (GPIOBASE_ADDR | 1));
	pci_write_config8(dev, GPIO_CNTL, 0x10);
}

static void i82801dx_rtc_init(struct device *dev)
{
	u8 reg8;
	u32 reg32;
	int rtc_failed;

	reg8 = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = reg8 & RTC_BATTERY_DEAD;
	if (rtc_failed) {
		reg8 &= ~(1 << 1);	/* Preserve the power fail state. */
		pci_write_config8(dev, GEN_PMCON_3, reg8);
	}
	reg32 = pci_read_config32(dev, GEN_STS);
	rtc_failed |= reg32 & (1 << 2);
	rtc_init(rtc_failed);

	/* Enable access to the upper 128 byte bank of CMOS RAM. */
	pci_write_config8(dev, RTC_CONF, 0x04);
}

static void i82801dx_lpc_route_dma(struct device *dev, u8 mask)
{
	u16 reg16;
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

static void i82801dx_lpc_decode_en(device_t dev)
{
	/* Decode 0x3F8-0x3FF (COM1) for COMA port, 0x2F8-0x2FF (COM2) for COMB.
	 * LPT decode defaults to 0x378-0x37F and 0x778-0x77F.
	 * Floppy decode defaults to 0x3F0-0x3F5, 0x3F7.
	 * We also need to set the value for LPC I/F Enables Register.
	 */
	pci_write_config8(dev, COM_DEC, 0x10);
	pci_write_config16(dev, LPC_EN, 0x300F);
}

/* ICH4 does not mention HPET in the docs, but
 * all ICH3 and ICH4 do have HPETs built in.
 */
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
	pci_write_config32(dev, GEN_CNTL, reg32);

	printk_debug("Enabling HPET @0x%x\n", HPET_ADDR | (code << 12));
}

static void lpc_init(struct device *dev)
{
	/* Set the value for PCI command register. */
	pci_write_config16(dev, PCI_COMMAND, 0x000f);

	/* IO APIC initialization. */
	i82801dx_enable_ioapic(dev);

	i82801dx_enable_serial_irqs(dev);

	/* Setup the PIRQ. */
	i82801dx_pirq_init(dev);

	/* Setup power options. */
	i82801dx_power_options(dev);

	/* Set the state of the GPIO lines. */
	gpio_init(dev);

	/* Initialize the real time clock. */
	i82801dx_rtc_init(dev);

	/* Route DMA. */
	i82801dx_lpc_route_dma(dev, 0xff);

	/* Initialize ISA DMA. */
	isa_dma_init();

	/* Setup decode ports and LPC I/F enables. */
	i82801dx_lpc_decode_en(dev);

	/* Initialize the High Precision Event Timers */
	enable_hpet(dev);
}

static void i82801dx_lpc_read_resources(device_t dev)
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
	res->base = 0xfec00000;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static void i82801dx_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	enable_childrens_resources(dev);
}

static struct device_operations lpc_ops = {
	.read_resources		= i82801dx_lpc_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= i82801dx_lpc_enable_resources,
	.init			= lpc_init,
	.scan_bus		= scan_static_bus,
	.enable			= i82801dx_enable,
};

/* 82801DB/DBL */
static const struct pci_driver lpc_driver_db __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DB_LPC,
};

/* 82801DBM */
static const struct pci_driver lpc_driver_dbm __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = PCI_DEVICE_ID_INTEL_82801DBM_LPC,
};
