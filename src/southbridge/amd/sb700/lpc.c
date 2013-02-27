/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <cbmem.h>
#include "sb700.h"

static void lpc_init(device_t dev)
{
	u8 byte;
	u32 dword;
	device_t sm_dev;

	/* Enable the LPC Controller */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	dword = pci_read_config32(sm_dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(sm_dev, 0x64, dword);

	/* Initialize isa dma */
#if CONFIG_SOUTHBRIDGE_AMD_SB700_SKIP_ISA_DMA_INIT
	printk(BIOS_DEBUG, "Skipping isa_dma_init() to avoid getting stuck.\n");
#else
	isa_dma_init();
#endif

	/* Enable DMA transaction on the LPC bus */
	byte = pci_read_config8(dev, 0x40);
	byte |= (1 << 2);
	pci_write_config8(dev, 0x40, byte);

	/* Disable the timeout mechanism on LPC */
	byte = pci_read_config8(dev, 0x48);
	byte &= ~(1 << 7);
	pci_write_config8(dev, 0x48, byte);

	/* Disable LPC MSI Capability */
	byte = pci_read_config8(dev, 0x78);
	byte &= ~(1 << 1);
#if CONFIG_SOUTHBRIDGE_AMD_SUBTYPE_SP5100
	/* Disable FlowContrl, Always service the request from Host
	 * whenever there is a request from Host pending
	 */
	byte &= ~(1 << 0);
#endif
	pci_write_config8(dev, 0x78, byte);

	/* hack, but the whole sb700 startup lacks any device which
	   is doing the acpi init */
#if CONFIG_HAVE_ACPI_RESUME
	{
	extern u8 acpi_slp_type;
	u16 tmp = inw(ACPI_PM1_CNT_BLK);
	acpi_slp_type = ((tmp & (7 << 10)) >> 10);
	printk(BIOS_DEBUG, "SLP_TYP type was %x\n", acpi_slp_type);
	}
#endif

	rtc_check_update_cmos_date(RTC_HAS_ALTCENTURY);
}

void set_cbmem_toc(struct cbmem_entry *toc)
{
	u32 dword = (u32) toc;
	int nvram_pos = 0xfc, i;
	for (i = 0; i<4; i++) {
		outb(nvram_pos, BIOSRAM_INDEX);
		outb((dword >>(8 * i)) & 0xff , BIOSRAM_DATA);
		nvram_pos++;
	}
}

static void sb700_lpc_read_resources(device_t dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);	/* We got one for APIC, or one more for TRAP */

	pci_get_resource(dev, 0xA0); /* SPI ROM base address */

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

	compact_resources(dev);
}

static void sb700_lpc_set_resources(struct device *dev)
{
	struct resource *res;

	pci_dev_set_resources(dev);

	/* Specical case. SPI Base Address. The SpiRomEnable should be set. */
	res = find_resource(dev, 0xA0);
	pci_write_config32(dev, 0xA0, res->base | 1 << 1);
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whose children's resources are to be enabled
 *
 */
static void sb700_lpc_enable_childrens_resources(device_t dev)
{
	struct bus *link;
	u32 reg, reg_x;
	int var_num = 0;
	u16 reg_var[3] = {0x0, 0x0, 0x0};
	u8 wiosize = pci_read_config8(dev, 0x74);

	reg = pci_read_config32(dev, 0x44);
	reg_x = pci_read_config32(dev, 0x48);

	for (link = dev->link_list; link; link = link->next) {
		device_t child;
		for (child = link->children; child;
		     child = child->sibling) {
			if (child->enabled
			    && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					u32 base, end;	/*  don't need long long */
					if (!(res->flags & IORESOURCE_IO))
						continue;
					base = res->base;
					end = resource_end(res);
					printk(BIOS_DEBUG, "sb700 lpc decode:%s, base=0x%08x, end=0x%08x\n",
					     dev_path(child), base, end);
					switch (base) {
					case 0x60:	/*  KB */
					case 0x64:	/*  MS */
						reg |= (1 << 29);
						break;
					case 0x3f8:	/*  COM1 */
						reg |= (1 << 6);
						break;
					case 0x2f8:	/*  COM2 */
						reg |= (1 << 7);
						break;
					case 0x378:	/*  Parallel 1 */
						reg |= (1 << 0);
						reg |= (1 << 1); /* + 0x778 for ECP */
						break;
					case 0x3f0:	/*  FD0 */
						reg |= (1 << 26);
						break;
					case 0x220:	/*  Audio 0 */
						reg |= (1 << 8);
						break;
					case 0x300:	/*  Midi 0 */
						reg |= (1 << 18);
						break;
					case 0x400:
						reg_x |= (1 << 16);
						break;
					case 0x480:
						reg_x |= (1 << 17);
						break;
					case 0x500:
						reg_x |= (1 << 18);
						break;
					case 0x580:
						reg_x |= (1 << 19);
						break;
					case 0x4700:
						reg_x |= (1 << 22);
						break;
					case 0xfd60:
						reg_x |= (1 << 23);
						break;
					default:
						if (var_num >= 3)
							continue;	/* only 3 var ; compact them ? */
						switch (var_num) {
						case 0:
							reg_x |= (1 << 2);
							if ((end - base) < 16)
								wiosize |= (1 << 0);
							break;
						case 1:
							reg_x |= (1 << 24);
							if ((end - base) < 16)
								wiosize |= (1 << 2);
							break;
						case 2:
							reg_x |= (1 << 25);
							reg_x |= (1 << 24);
							if ((end - base) < 16)
								wiosize |= (1 << 3);
							break;
						}
						reg_var[var_num++] =
						    base & 0xffff;
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0x44, reg);
	pci_write_config32(dev, 0x48, reg_x);
	/* Set WideIO for as many IOs found (fall through is on purpose) */
	switch (var_num) {
	case 2:
		pci_write_config16(dev, 0x90, reg_var[2]);
	case 1:
		pci_write_config16(dev, 0x66, reg_var[1]);
	case 0:
		pci_write_config16(dev, 0x64, reg_var[0]);
		break;
	}
	pci_write_config8(dev, 0x74, wiosize);
}

static void sb700_lpc_enable_resources(device_t dev)
{
	pci_dev_enable_resources(dev);
	sb700_lpc_enable_childrens_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations lpc_ops = {
	.read_resources = sb700_lpc_read_resources,
	.set_resources = sb700_lpc_set_resources,
	.enable_resources = sb700_lpc_enable_resources,
	.init = lpc_init,
	.scan_bus = scan_static_bus,
	.ops_pci = &lops_pci,
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_ATI_SB700_LPC,
};
