/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
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
#include <acpi/acpi.h>
#include <pc80/i8254.h>
#include <pc80/i8259.h>
#include "hudson.h"
#include "pci_devs.h"

static void lpc_init(struct device *dev)
{
	u8 byte;
	u32 dword;
	struct device *sm_dev;

	/* Enable the LPC Controller */
	sm_dev = pcidev_on_root(0x14, 0);
	dword = pci_read_config32(sm_dev, 0x64);
	dword |= 1 << 20;
	pci_write_config32(sm_dev, 0x64, dword);

	/* Initialize isa dma */
	isa_dma_init();

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
	byte &= ~(1 << 0);	/* Keep the old way. i.e., when bus master/DMA cycle is going
				   on on LPC, it holds PCI grant, so no LPC slave cycle can
				   interrupt and visit LPC. */
	pci_write_config8(dev, 0x78, byte);

	/* bit0: Enable prefetch a cacheline (64 bytes) when Host reads code from SPI ROM */
	/* bit3: Fix SPI_CS# timing issue when running at 66M. TODO:A12. */
	byte = pci_read_config8(dev, 0xBB);
	byte |= 1 << 0 | 1 << 3;
	pci_write_config8(dev, 0xBB, byte);

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
}

static void hudson_lpc_read_resources(struct device *dev)
{
	struct resource *res;

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);	/* We got one for APIC, or one more for TRAP */

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

	/* Add a memory resource for the SPI BAR. */
	mmio_range(dev, 2, SPI_BASE_ADDRESS, 1 * KiB);

	res = new_resource(dev, 3); /* IOAPIC */
	res->base = IO_APIC_ADDR;
	res->size = 0x00001000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;

	compact_resources(dev);
}

static void hudson_lpc_set_resources(struct device *dev)
{
	struct resource *res;

	/* Special case. SPI Base Address. The SpiRomEnable should STAY set. */
	res = find_resource(dev, 2);
	pci_write_config32(dev, SPIROM_BASE_ADDRESS_REGISTER, res->base | SPI_ROM_ENABLE);

	pci_dev_set_resources(dev);
}

/**
 * @brief Enable resources for children devices
 *
 * @param dev the device whose children's resources are to be enabled
 *
 */
static void hudson_lpc_enable_childrens_resources(struct device *dev)
{
	struct bus *link;
	u32 reg, reg_x;
	int var_num = 0;
	u16 reg_var[3];
	u16 reg_size[1] =  {512};
	u8 wiosize = pci_read_config8(dev, 0x74);

	/* Be bit relaxed, tolerate that LPC region might be bigger than resource we try to fit,
	 * do it like this for all regions < 16 bytes. If there is a resource > 16 bytes
	 * it must be 512 bytes to be able to allocate the fresh LPC window.
	 *
	 * AGESA likes to enable already one LPC region in wide port base 0x64-0x65,
	 * using DFLT_SIO_PME_BASE_ADDRESS, 512 bytes size
	 * The code tries to check if resource can fit into this region
	 */

	reg = pci_read_config32(dev, 0x44);
	reg_x = pci_read_config32(dev, 0x48);

	/* check if ranges are free and not use them if entry is just already taken */
	if (reg_x & (1 << 2))
		var_num = 1;
	/* just in case check if someone did not manually set other ranges too */
	if (reg_x & (1 << 24))
		var_num = 2;

	if (reg_x & (1 << 25))
		var_num = 3;

	/* check AGESA region size */
	if (wiosize & (1 << 0))
		reg_size[0] = 16;

	reg_var[2] = pci_read_config16(dev, 0x90);
	reg_var[1] = pci_read_config16(dev, 0x66);
	reg_var[0] = pci_read_config16(dev, 0x64);

	for (link = dev->link_list; link; link = link->next) {
		struct device *child;
		for (child = link->children; child;
		     child = child->sibling) {
			if (child->enabled
			    && (child->path.type == DEVICE_PATH_PNP)) {
				struct resource *res;
				for (res = child->resource_list; res; res = res->next) {
					u32 base, end;	/*  don't need long long */
					u32 rsize, set = 0, set_x = 0;
					if (!(res->flags & IORESOURCE_IO))
						continue;
					base = res->base;
					end = resource_end(res);
					/* find a resource size */
					printk(BIOS_DEBUG, "hudson lpc decode:%s, base=0x%08x, end=0x%08x\n",
					     dev_path(child), base, end);
					switch (base) {
					case 0x60:	/*  KB */
					case 0x64:	/*  MS */
						set |= (1 << 29);
						rsize = 1;
						break;
					case 0x3f8:	/*  COM1 */
						set |= (1 << 6);
						rsize = 8;
						break;
					case 0x2f8:	/*  COM2 */
						set |= (1 << 7);
						rsize = 8;
						break;
					case 0x378:	/*  Parallel 1 */
						set |= (1 << 0);
						set |= (1 << 1); /* + 0x778 for ECP */
						rsize = 8;
						break;
					case 0x3f0:	/*  FD0 */
						set |= (1 << 26);
						rsize = 8;
						break;
					case 0x220:	/*  0x220 - 0x227 */
						set |= (1 << 8);
						rsize = 8;
						break;
					case 0x228:	/*  0x228 - 0x22f */
						set |= (1 << 9);
						rsize = 8;
						break;
					case 0x238:	/*  0x238 - 0x23f */
						set |= (1 << 10);
						rsize = 8;
						break;
					case 0x300:	/*  0x300 -0x301 */
						set |= (1 << 18);
						rsize = 2;
						break;
					case 0x400:
						set_x |= (1 << 16);
						rsize = 0x40;
						break;
					case 0x480:
						set_x |= (1 << 17);
						rsize = 0x40;
						break;
					case 0x500:
						set_x |= (1 << 18);
						rsize = 0x40;
						break;
					case 0x580:
						set_x |= (1 << 19);
						rsize = 0x40;
						break;
					case 0x4700:
						set_x |= (1 << 22);
						rsize = 0xc;
						break;
					case 0xfd60:
						set_x |= (1 << 23);
						rsize = 16;
						break;
					default:
						rsize = 0;
						/* try AGESA allocated region in region 0 */
						if ((var_num > 0) && ((base >=reg_var[0]) &&
								((base + res->size) <= (reg_var[0] + reg_size[0]))))
							rsize = reg_size[0];
					}
					/* check if region found and matches the enable */
					if (res->size <= rsize) {
						reg |= set;
						reg_x |= set_x;
					/* check if we can fit resource in variable range */
					} else if ((var_num < 3) &&
						    ((res->size <= 16) || (res->size == 512))) {
						/* use variable ranges if pre-defined do not match */
						switch (var_num) {
						case 0:
							reg_x |= (1 << 2);
							if (res->size <= 16) {
								wiosize |= (1 << 0);
							}
							break;
						case 1:
							reg_x |= (1 << 24);
							if (res->size <= 16)
								wiosize |= (1 << 2);
							break;
						case 2:
							reg_x |= (1 << 25);
							if (res->size <= 16)
								wiosize |= (1 << 3);
							break;
						}
						reg_var[var_num++] =
						    base & 0xffff;
					} else {
						printk(BIOS_ERR, "cannot fit LPC decode region:%s, base=0x%08x, end=0x%08x\n",
							dev_path(child), base, end);
					}
				}
			}
		}
	}
	pci_write_config32(dev, 0x44, reg);
	pci_write_config32(dev, 0x48, reg_x);
	/* Set WideIO for as many IOs found (fall through is on purpose) */
	switch (var_num) {
	case 3:
		pci_write_config16(dev, 0x90, reg_var[2]);
		__fallthrough;
	case 2:
		pci_write_config16(dev, 0x66, reg_var[1]);
		__fallthrough;
	case 1:
		pci_write_config16(dev, 0x64, reg_var[0]);
		break;
	}
	pci_write_config8(dev, 0x74, wiosize);
}

static void hudson_lpc_enable_resources(struct device *dev)
{
	pci_dev_enable_resources(dev);
	hudson_lpc_enable_childrens_resources(dev);
}

static const char *lpc_acpi_name(const struct device *dev)
{
	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	if (dev->path.pci.devfn == LPC_DEVFN)
		return "LIBR";

	return NULL;
}

static void lpc_final(struct device *dev)
{
	if (!acpi_is_wakeup_s3()) {
		if (CONFIG(HAVE_SMI_HANDLER))
			outl(0x0, ACPI_PM1_CNT_BLK);	/* clear SCI_EN */
		else
			outl(0x1, ACPI_PM1_CNT_BLK);	/* set SCI_EN */
	}
}

static struct device_operations lpc_ops = {
	.read_resources = hudson_lpc_read_resources,
	.set_resources = hudson_lpc_set_resources,
	.enable_resources = hudson_lpc_enable_resources,
#if CONFIG(HAVE_ACPI_TABLES)
	.write_acpi_tables = acpi_write_hpet,
#endif
	.init = lpc_init,
	.final = lpc_final,
	.scan_bus = scan_static_bus,
	.ops_pci = &pci_dev_ops_pci,
	.acpi_name = lpc_acpi_name,
};
static const struct pci_driver lpc_driver __pci_driver = {
	.ops = &lpc_ops,
	.vendor = PCI_VID_AMD,
	.device = PCI_DID_AMD_SB900_LPC,
};
