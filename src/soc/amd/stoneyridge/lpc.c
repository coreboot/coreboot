/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010-2017 Advanced Micro Devices, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC
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

#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_def.h>
#include <pc80/mc146818rtc.h>
#include <pc80/isa-dma.h>
#include <arch/io.h>
#include <arch/ioapic.h>
#include <arch/acpi.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include <soc/acpi.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <soc/nvs.h>

static void lpc_init(struct device *dev)
{
	u8 byte;
	u32 dword;
	struct device *sm_dev;

	/*
	 * Enable the LPC Controller
	 * SMBus register 0x64 is not defined in public datasheet.
	 */
	sm_dev = dev_find_slot(0, SMBUS_DEVFN);
	dword = pci_read_config32(sm_dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(sm_dev, 0x64, dword);

	/* Initialize isa dma */
	isa_dma_init();

	/* Enable DMA transaction on the LPC bus */
	byte = pci_read_config8(dev, LPC_PCI_CONTROL);
	byte |= LEGACY_DMA_EN;
	pci_write_config8(dev, LPC_PCI_CONTROL, byte);

	/* Disable the timeout mechanism on LPC */
	byte = pci_read_config8(dev, LPC_IO_OR_MEM_DECODE_ENABLE);
	byte &= ~LPC_SYNC_TIMEOUT_COUNT_ENABLE;
	pci_write_config8(dev, LPC_IO_OR_MEM_DECODE_ENABLE, byte);

	/* Disable LPC MSI Capability */
	byte = pci_read_config8(dev, LPC_MISC_CONTROL_BITS);
	/* BIT 1 is not defined in public datasheet. */
	byte &= ~(1 << 1);

	/*
	 * Keep the old way. i.e., when bus master/DMA cycle is going
	 * on on LPC, it holds PCI grant, so no LPC slave cycle can
	 * interrupt and visit LPC.
	 */
	byte &= ~LPC_NOHOG;
	pci_write_config8(dev, LPC_MISC_CONTROL_BITS, byte);

	/*
	 * IMC is not used, but some of its registers and ports need to be
	 * programmed/accessed. So enable CPU access to them. This fixes
	 * SPI_CS# timing issue when running at 66MHz.
	 */
	byte = pci_read_config8(dev, LPC_HOST_CONTROL);
	byte |= IMC_PAGE_FROM_HOST_EN | IMC_PORT_FROM_HOST_EN;
	pci_write_config8(dev, LPC_HOST_CONTROL, byte);

	cmos_check_update_date();

	/*
	 * Initialize the real time clock.
	 * The 0 argument tells cmos_init not to
	 * update CMOS unless it is invalid.
	 * 1 tells cmos_init to always initialize the CMOS.
	 */
	cmos_init(0);

	/* Initialize i8259 pic */
	setup_i8259();

	/* Initialize i8254 timers */
	setup_i8254();

	/* Set up SERIRQ, enable continuous mode */
	byte = (PM_SERIRQ_NUM_BITS_21 | PM_SERIRQ_ENABLE);
	if (!IS_ENABLED(CONFIG_SERIRQ_CONTINUOUS_MODE))
		byte |= PM_SERIRQ_MODE;

	pm_write8(PM_SERIRQ_CONF, byte);
}

static void lpc_read_resources(struct device *dev)
{
	struct resource *res;
	global_nvs_t *gnvs;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	/* Add an extra subtractive resource for both memory and I/O. */
	res = new_resource(dev, IOINDEX_SUBTRACTIVE(0, 0));
	res->base = 0;
	res->size = 0x1000;
	res->flags = IORESOURCE_IO | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	res = new_resource(dev, IOINDEX_SUBTRACTIVE(1, 0));
	res->base = FLASH_BASE_ADDR;
	res->size = CONFIG_ROM_SIZE;
	res->flags = IORESOURCE_MEM | IORESOURCE_SUBTRACTIVE |
		     IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* Add a memory resource for the SPI BAR. */
	fixed_mem_resource(dev, 2, SPI_BASE_ADDRESS / 1024, 1,
			IORESOURCE_SUBTRACTIVE);

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	/* I2C devices (all 4 devices) */
	res = new_resource(dev, 4);
	res->base = I2C_BASE_ADDRESS;
	res->size = I2C_DEVICE_SIZE * I2C_DEVICE_COUNT;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);

	/* Allocate ACPI NVS in CBMEM */
	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));
}

static void lpc_set_resources(struct device *dev)
{
	struct resource *res;
	u32 spi_enable_bits;

	/* Special case. The SpiRomEnable and other enables should STAY set. */
	res = find_resource(dev, 2);
	spi_enable_bits = pci_read_config32(dev, SPIROM_BASE_ADDRESS_REGISTER);
	spi_enable_bits &= SPI_PRESERVE_BITS;
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER,
			res->base | spi_enable_bits);

	pci_dev_set_resources(dev);
}

static void set_child_resource(struct device *child,
				u32 *reg,
				u32 *reg_x)
{
	struct resource *res;
	u32 base, end;
	u32 rsize = 0, set = 0, set_x = 0;
	int wideio_index;

	/*
	 * Be a bit relaxed, tolerate that LPC region might be bigger than
	 * resource we try to fit, do it like this for all regions < 16 bytes.
	 * If there is a resource > 16 bytes it must be 512 bytes to be able
	 * to allocate the fresh LPC window.
	 *
	 * AGESA and early initialization can set a wide IO port. This code
	 * will verify if required region was previously set and will avoid
	 * setting a new wide IO resource if one is already set.
	 */

	for (res = child->resource_list; res; res = res->next) {
		if (!(res->flags & IORESOURCE_IO))
			continue;
		base = res->base;
		end = resource_end(res);
		printk(BIOS_DEBUG,
			"Southbridge LPC decode:%s, base=0x%08x, end=0x%08x\n",
			dev_path(child), base, end);
		/* find a resource size */
		switch (base) {
		case 0x60:	/*  KB */
		case 0x64:	/*  MS */
			set |= DECODE_ENABLE_KBC_PORT;
			rsize = 1;
			break;
		case 0x3f8:	/*  COM1 */
			set |= DECODE_ENABLE_SERIAL_PORT0;
			rsize = 8;
			break;
		case 0x2f8:	/*  COM2 */
			set |= DECODE_ENABLE_SERIAL_PORT1;
			rsize = 8;
			break;
		case 0x378:	/*  Parallel 1 */
			set |= DECODE_ENABLE_PARALLEL_PORT0;
			/* enable 0x778 for ECP mode */
			set |= DECODE_ENABLE_PARALLEL_PORT1;
			rsize = 8;
			break;
		case 0x3f0:	/*  FD0 */
			set |= DECODE_ENABLE_FDC_PORT0;
			rsize = 8;
			break;
		case 0x220:	/*  0x220 - 0x227 */
			set |= DECODE_ENABLE_SERIAL_PORT2;
			rsize = 8;
			break;
		case 0x228:	/*  0x228 - 0x22f */
			set |= DECODE_ENABLE_SERIAL_PORT3;
			rsize = 8;
			break;
		case 0x238:	/*  0x238 - 0x23f */
			set |= DECODE_ENABLE_SERIAL_PORT4;
			rsize = 8;
			break;
		case 0x300:	/*  0x300 - 0x301 */
			set |= DECODE_ENABLE_MIDI_PORT0;
			rsize = 2;
			break;
		case 0x400:
			set_x |= DECODE_IO_PORT_ENABLE0;
			rsize = 0x40;
			break;
		case 0x480:
			set_x |= DECODE_IO_PORT_ENABLE1;
			rsize = 0x40;
			break;
		case 0x500:
			set_x |= DECODE_IO_PORT_ENABLE2;
			rsize = 0x40;
			break;
		case 0x580:
			set_x |= DECODE_IO_PORT_ENABLE3;
			rsize = 0x40;
			break;
		case 0x4700:
			set_x |= DECODE_IO_PORT_ENABLE5;
			rsize = 0xc;
			break;
		case 0xfd60:
			set_x |= DECODE_IO_PORT_ENABLE6;
			rsize = 16;
			break;
		default:
			rsize = 0;
			wideio_index = sb_find_wideio_range(base, res->size);
			if (wideio_index != WIDEIO_RANGE_ERROR) {
				rsize = sb_wideio_size(wideio_index);
				printk(BIOS_DEBUG, "Covered by wideIO");
				printk(BIOS_DEBUG, " %d\n", wideio_index);
			}
		}
		/* check if region found and matches the enable */
		if (res->size <= rsize) {
			*reg |= set;
			*reg_x |= set_x;
		/* check if we can fit resource in variable range */
		} else {
			wideio_index = sb_set_wideio_range(base, res->size);
			if (wideio_index != WIDEIO_RANGE_ERROR) {
				/* preserve wide IO related bits. */
				*reg_x = pci_read_config32(SOC_LPC_DEV,
					LPC_IO_OR_MEM_DECODE_ENABLE);

				printk(BIOS_DEBUG,
					"Range assigned to wide IO %d\n",
					wideio_index);
			} else {
				printk(BIOS_ERR,
					"cannot fit LPC decode region:");
				printk(BIOS_ERR,
					"%s, base = 0x%08x, end = 0x%08x\n",
					dev_path(child), base, end);
			}
		}
	}
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whose children's resources are to be enabled
 *
 */
static void lpc_enable_childrens_resources(struct device *dev)
{
	struct bus *link;
	u32 reg, reg_x;

	reg = pci_read_config32(dev, LPC_IO_PORT_DECODE_ENABLE);
	reg_x = pci_read_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE);

	for (link = dev->link_list; link; link = link->next) {
		struct device *child;
		for (child = link->children; child;
		     child = child->sibling) {
			if (child->enabled
			    && (child->path.type == DEVICE_PATH_PNP)) {
				set_child_resource(child,
						&reg,
						&reg_x);
			}
		}
	}
	pci_write_config32(dev, LPC_IO_PORT_DECODE_ENABLE, reg);
	pci_write_config32(dev, LPC_IO_OR_MEM_DECODE_ENABLE, reg_x);
}

static void lpc_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);
	lpc_enable_childrens_resources(dev);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations lpc_ops = {
	.read_resources = lpc_read_resources,
	.set_resources = lpc_set_resources,
	.enable_resources = lpc_enable_resources,
	.acpi_inject_dsdt_generator = southbridge_inject_dsdt,
	.write_acpi_tables = southbridge_write_acpi_tables,
	.init = lpc_init,
	.scan_bus = scan_lpc_bus,
	.ops_pci = &lops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DEVICE_ID_AMD_SB900_LPC,
	PCI_DEVICE_ID_AMD_CZ_LPC,
	0
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.devices = pci_device_ids,
};
