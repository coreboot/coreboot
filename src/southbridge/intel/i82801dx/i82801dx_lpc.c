/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2004 SuSE Linux AG
 * Copyright (C) 2004 Tyan Computer
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

void i82801dx_enable_ioapic(struct device *dev)
{
	u32 dword;
	volatile u32 *ioapic_sba = (volatile u32 *)0xfec00000;
	volatile u32 *ioapic_sbd = (volatile u32 *)0xfec00010;

	dword = pci_read_config32(dev, GEN_CNTL);
	dword |= (3 << 7);	/* enable ioapic */
	dword |= (1 << 13);	/* coprocessor error enable */
	dword |= (1 << 1);	/* delay transaction enable */
	dword |= (1 << 2);	/* DMA collection buf enable */
	pci_write_config32(dev, GEN_CNTL, dword);
	printk_debug("ioapic southbridge enabled %x\n", dword);
	*ioapic_sba = 0;
	*ioapic_sbd = (2 << 24);
	//lyh *ioapic_sba=3;
	//lyh *ioapic_sbd=1;    
	*ioapic_sba = 0;
	dword = *ioapic_sbd;
	printk_debug("Southbridge apic id = %x\n", dword);
	if (dword != (2 << 24))
		die("");
	//lyh *ioapic_sba=3;
	//lyh dword=*ioapic_sbd;
	//lyh printk_debug("Southbridge apic DT = %x\n",dword);
	//lyh if(dword!=1)
	//lyh   die("");

}

void i82801dx_enable_serial_irqs(struct device *dev)
{
	pci_write_config8(dev, SERIRQ_CNTL,
			  (1 << 7) | (1 << 6) | ((21 - 17) << 2) | (0 << 0));
}

void i82801dx_lpc_route_dma(struct device *dev, u8 mask)
{
	u16 word;
	int i;
	word = pci_read_config16(dev, PCI_DMA_CFG);
	word &= ((1 << 10) - (1 << 8));
	for (i = 0; i < 8; i++) {
		if (i == 4)
			continue;
		word |= ((mask & (1 << i)) ? 3 : 1) << (i * 2);
	}
	pci_write_config16(dev, PCI_DMA_CFG, word);
}

void i82801dx_rtc_init(struct device *dev)
{
	u8 byte;
	u32 dword;
	int rtc_failed;
	byte = pci_read_config8(dev, GEN_PMCON_3);
	rtc_failed = byte & RTC_FAILED;
	if (rtc_failed) {
		byte &= ~(1 << 1);	/* preserve the power fail state */
		pci_write_config8(dev, GEN_PMCON_3, byte);
	}
	dword = pci_read_config32(dev, GEN_STS);
	rtc_failed |= dword & (1 << 2);
	rtc_init(rtc_failed);
}

void i82801dx_1f0_misc(struct device *dev)
{
	pci_write_config16(dev, PCICMD, 0x014f);
	pci_write_config32(dev, PMBASE, 0x00001001);
	pci_write_config8(dev, ACPI_CNTL, 0x10);
	pci_write_config32(dev, GPIO_BASE, 0x00001181);
	pci_write_config8(dev, GPIO_CNTL, 0x10);
	pci_write_config32(dev, PIRQA_ROUT, 0x0A05030B);
	pci_write_config8(dev, PIRQE_ROUT, 0x07);
	pci_write_config8(dev, RTC_CONF, 0x04);
	pci_write_config8(dev, COM_DEC, 0x10);	//lyh E0->
	pci_write_config16(dev, LPC_EN, 0x000F);	//LYH 000D->
}

static void enable_hpet(struct device *dev)
{
	const unsigned long hpet_address = 0xfed00000;

	u32 dword;
	u32 code = (0 & 0x3);

	dword = pci_read_config32(dev, GEN_CNTL);
	dword |= (1 << 17);	/* enable hpet */
	/*Bits [16:15]Memory Address Range
	   00 FED0_0000h - FED0_03FFh
	   01 FED0_1000h - FED0_13FFh
	   10 FED0_2000h - FED0_23FFh
	   11 FED0_3000h - FED0_33FFh */

	dword &= ~(3 << 15);	/* clear it */
	dword |= (code << 15);

	printk_debug("enabling HPET @0x%lx\n", hpet_address | (code << 12));
}

static void lpc_init(struct device *dev)
{
	u8 byte;
	int pwr_on = -1;
	int nmi_option;

	/* IO APIC initialization */
	i82801dx_enable_ioapic(dev);

	i82801dx_enable_serial_irqs(dev);

#ifdef SUSPICIOUS_LOOKING_CODE
	// The ICH-4 datasheet does not mention this configuration register. 
	// This code may have been inherited (incorrectly) from code for the AMD 766 southbridge,
	// which *does* support this functionality.

	/* posted memory write enable */
	byte = pci_read_config8(dev, 0x46);
	pci_write_config8(dev, 0x46, byte | (1 << 0));
#endif

	/* power after power fail */
	/* FIXME this doesn't work! */
	/* Which state do we want to goto after g3 (power restored)?
	 * 0 == S0 Full On
	 * 1 == S5 Soft Off
	 */
	pci_write_config8(dev, GEN_PMCON_3, pwr_on ? 0 : 1);
	printk_info("set power %s after power fail\n", pwr_on ? "on" : "off");
#if 0
	/* Enable Error reporting */
	/* Set up sync flood detected */
	byte = pci_read_config8(dev, 0x47);
	byte |= (1 << 1);
	pci_write_config8(dev, 0x47, byte);
#endif

	/* Set up NMI on errors */
	byte = inb(0x61);
	byte &= ~(1 << 3);	/* IOCHK# NMI Enable */
	byte &= ~(1 << 2);	/* PCI SERR# Enable */
	outb(byte, 0x61);
	byte = inb(0x70);
	nmi_option = NMI_OFF;
	get_option(&nmi_option, "nmi");
	if (nmi_option) {
		byte &= ~(1 << 7);	/* set NMI */
		outb(byte, 0x70);
	}

	/* Initialize the real time clock */
	i82801dx_rtc_init(dev);

	i82801dx_lpc_route_dma(dev, 0xff);

	/* Initialize isa dma */
	isa_dma_init();

	i82801dx_1f0_misc(dev);
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
	res->size = 0x00800000;	/* 8 MB for flash */
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
	    IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, 3);	/* IOAPIC */
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
	.read_resources = i82801dx_lpc_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = i82801dx_lpc_enable_resources,
	.init = lpc_init,
	.scan_bus = scan_static_bus,
	.enable = i82801dx_enable,
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
