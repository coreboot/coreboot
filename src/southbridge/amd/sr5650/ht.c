/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/ioapic.h>
#include <lib.h>
#include "sr5650.h"
#include "cmn.h"

/* Table 6-6 Recommended Interrupt Routing Configuration */
typedef struct _apic_device_info {
	u8	group;
	u8	swizzle;
	u8	pin;
} apic_device_info;

#define ABCD		0
#define BCDA		1
#define CDAB		2
#define DABC		3

static const apic_device_info default_apic_device_info_t [] = {
	/* Group  Swizzling   Port Int Pin */
	[0] = {0,     0,          31},   /* HT */
	[1] = {0,     0,          31},   /* IOMMU */
	[2] = {0,     ABCD,       28},   /* Dev2  Grp0 [Int - 0..3] */
	[3] = {1,     ABCD,       28},   /* Dev3  Grp1 [Int - 4..7] */
	[4] = {5,     ABCD,       28},   /* Dev4  Grp5 [Int - 20..23] */
	[5] = {5,     CDAB,       28},   /* Dev5  Grp5 [Int - 20..23] */
	[6] = {6,     BCDA,       29},   /* Dev6  Grp6 [Int - 24..27] */
	[7] = {6,     CDAB,       29},   /* Dev7  Grp6 [Int - 24..27] */
	[8] = {0,     0,          0 },   /* Reserved */
	[9] = {6,     ABCD,       29},   /* Dev9  Grp6 [Int - 24..27] */
	[10] = {5,     BCDA,       30},   /* Dev10 Grp5 [Int - 20..23] */
	[11] = {2,     ABCD,       30},   /* Dev11 Grp2 [Int - 8..11] */
	[12] = {3,     ABCD,       30},   /* Dev12 Grp3 [Int - 12..15] */
	[13] = {4,     ABCD,       30}    /* Dev13 Grp4 [Int - 16..19] */
};

/* These define names are common, so undefine them to avoid potential issues in other code */
#undef ABCD
#undef BCDA
#undef CDAB
#undef DABC

/* Init APIC of sr5690 */
static void sr5690_apic_init(struct device *dev)
{
	u32 dword;
	const apic_device_info *entry = default_apic_device_info_t;

	/* rpr6.2.(2). Write to the IOAPIC Features Enable register */
	pci_write_config32(dev, 0xF8, 0x0);
	pci_write_config32(dev, 0xFC, 0x1F);
	/* rpr6.2.(3). Write to the remaining interrupt routing registers */

	/* IOAPICCMISCIND:0x3, group & swizzle of Dev 2,3,4,5 */
	dword = (entry[2].group | entry[2].swizzle << 4) << 0  |
		(entry[3].group | entry[3].swizzle << 4) << 8  |
		(entry[4].group | entry[4].swizzle << 4) << 16 |
		(entry[5].group | entry[5].swizzle << 4) << 24;
	pci_write_config32(dev, 0xF8, 0x3);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x4, group & swizzle of Dev 6,7,9,10 */
	dword = (entry[6].group | entry[6].swizzle << 4) << 0  |
		(entry[7].group | entry[7].swizzle << 4) << 8  |
		(entry[9].group | entry[9].swizzle << 4) << 16 |
		(entry[10].group | entry[10].swizzle << 4) << 24;
	pci_write_config32(dev, 0xF8, 0x4);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x5, group & swizzle of Dev 11,12,13 */
	dword = (entry[11].group | entry[11].swizzle << 4) << 0  |
		(entry[12].group | entry[12].swizzle << 4) << 8  |
		(entry[13].group | entry[13].swizzle << 4) << 16;
	pci_write_config32(dev, 0xF8, 0x5);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x6, pin map of dev 2,3,4,5 */
	dword = entry[2].pin |
		entry[3].pin << 8 |
		entry[4].pin << 16|
		entry[5].pin << 24;
	pci_write_config32(dev, 0xF8, 0x6);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x7, pin map of dev 6,7,8,9 */
	dword = entry[6].pin |
		entry[7].pin << 8 |
		entry[8].pin << 16|
		entry[9].pin << 24;
	pci_write_config32(dev, 0xF8, 0x7);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x8, pin map of dev 10,11,12,13 */
	dword = entry[10].pin |
		entry[11].pin << 8 |
		entry[12].pin << 16|
		entry[13].pin << 24;
	pci_write_config32(dev, 0xF8, 0x8);
	pci_write_config32(dev, 0xFC, dword);

	/* IOAPICCMISCIND:0x9, pin map of ht, iommu */
	dword = entry[0].pin | entry[1].pin << 8;
	pci_write_config32(dev, 0xF8, 0x9);
	pci_write_config32(dev, 0xFC, dword);

	pci_write_config32(dev, 0xF8, 0x1);
	dword = pci_read_config32(dev, 0xFC) & 0xfffffff0;
	/* TODO: On SR56x0/SP5100 board, the IOAPIC on SR56x0 is the
	 * 2nd one. We need to check if it also is on your board. */
	setup_ioapic((void *)dword, 1);
}

static void pcie_init(struct device *dev)
{
	/* Enable pci error detecting */
	u32 dword;

	printk(BIOS_INFO, "pcie_init in sr5650_ht.c\n");

	/* System error enable */
	dword = pci_read_config32(dev, 0x04);
	dword |= (1 << 8);	/* System error enable */
	dword |= (1 << 30);	/* Clear possible errors */
	pci_write_config32(dev, 0x04, dword);

	/*
	 * 1 is APIC enable
	 * 18 is enable nb to accept A4 interrupt request from SB.
	 */
	dword = pci_read_config32(dev, 0x4C);
	dword |= 1 << 1 | 1 << 18;	/* Clear possible errors */
	pci_write_config32(dev, 0x4C, dword);

	sr5690_apic_init(dev);
}

static void sr5690_read_resource(struct device *dev)
{
	if (IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)) {
		printk(BIOS_DEBUG,"%s: %s\n", __func__, dev_path(dev));
		set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 1 << 3);	/* Hide BAR3 */
	}

	pci_dev_read_resources(dev);

	/* rpr6.2.(1). Write the Base Address Register (BAR) */
	pci_write_config32(dev, 0xf8, 0x1);	/* Set IOAPIC's index to 1 and make sure no one changes it */
	pci_get_resource(dev, 0xfc);		/* APIC located in sr5690 */

	compact_resources(dev);
}

/* If IOAPIC's index changes, we should replace the pci_dev_set_resource(). */
static void sr5690_set_resources(struct device *dev)
{
	pci_write_config32(dev, 0xf8, 0x1);	/* Set IOAPIC's index to 1 and make sure no one changes it */

	if (IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)) {
		uint32_t reg;
		struct device *amd_ht_cfg_dev;
		struct device *amd_addr_map_dev;
		resource_t res_base;
		resource_t res_end;
		uint32_t base;
		uint32_t limit;
		struct resource *res;

		printk(BIOS_DEBUG,"%s %s\n", dev_path(dev), __func__);

		/* Find requisite AMD CPU devices */
		amd_ht_cfg_dev = dev_find_slot(0, PCI_DEVFN(0x18, 0));
		amd_addr_map_dev = dev_find_slot(0, PCI_DEVFN(0x18, 1));

		if (!amd_ht_cfg_dev || !amd_addr_map_dev) {
			printk(BIOS_WARNING, "%s: %s Unable to locate CPU control devices\n", __func__, dev_path(dev));
		} else {
			res = sr5650_retrieve_cpu_mmio_resource();
			if (res) {
				/* Set up MMCONFIG bus range */
				set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 0 << 3);	/* Make BAR3 visible */
				set_nbcfg_enable_bits(dev, 0x7c, 1 << 30, 1 << 30);	/* Enables writes to the BAR3 register */
				set_nbcfg_enable_bits(dev, 0x84, 7 << 16, 0 << 16);	/* Program bus range = 255 busses */
				pci_write_config32(dev, 0x1c, res->base);

				/* Enable MMCONFIG decoding. */
				set_htiu_enable_bits(dev, 0x32, 1 << 28, 1 << 28);	/* PCIEMiscInit */
				set_nbcfg_enable_bits(dev, 0x7c, 1 << 30, 0 << 30);	/* Disable writes to the BAR3 register */
				set_nbmisc_enable_bits(dev, 0x0, 1 << 3, 1 << 3);	/* Hide BAR3 */

				/* Set up nonposted resource in MMIO space */
				res_base = res->base;		/* Get the base address */
				res_end = resource_end(res);	/* Get the limit (rounded up) */
				printk(BIOS_DEBUG, "%s: %s[0x1c] base = %0llx limit = %0llx\n", __func__, dev_path(dev), res_base, res_end);

				/* Locate an unused MMIO resource */
				for (reg = 0xb8; reg >= 0x80; reg -= 8) {
					base = pci_read_config32(amd_addr_map_dev, reg);
					limit = pci_read_config32(amd_addr_map_dev, reg + 4);
					if (!(base & 0x3))
						break;	/* Unused resource found */
				}

				/* If an unused MMIO resource was available, set up the mapping */
				if (!(base & 0x3)) {
					uint32_t sblk;

					/* Remember this resource has been stored. */
					res->flags |= IORESOURCE_STORED;
					report_resource_stored(dev, res, " <mmconfig>");

					/* Get SBLink value (HyperTransport I/O Hub Link ID). */
					sblk = (pci_read_config32(amd_ht_cfg_dev, 0x64) >> 8) & 0x3;

					/* Calculate the MMIO mapping base */
					base &= 0x000000f0;
					base |= ((res_base >> 8) & 0xffffff00);
					base |= 3;

					/* Calculate the MMIO mapping limit */
					limit &= 0x00000048;
					limit |= ((res_end >> 8) & 0xffffff00);
					limit |= (sblk << 4);
					limit |= (1 << 7);

					/* Configure and enable MMIO mapping */
					printk(BIOS_INFO, "%s: %s <- index %x base %04x limit %04x\n", __func__, dev_path(amd_addr_map_dev), reg, base, limit);
					pci_write_config32(amd_addr_map_dev, reg + 4, limit);
					pci_write_config32(amd_addr_map_dev, reg, base);
				}
				else {
					printk(BIOS_WARNING, "%s: %s No free MMIO resources available\n", __func__, dev_path(dev));
				}
			} else {
				printk(BIOS_WARNING, "%s: %s Unable to locate CPU MMCONF resource\n", __func__, dev_path(dev));
			}
		}
	}

	pci_dev_set_resources(dev);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations ht_ops = {
	.read_resources = sr5690_read_resource,
	.set_resources = sr5690_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = pcie_init,
	.scan_bus = 0,
	.ops_pci = &lops_pci,
};

static const struct pci_driver ht_driver_sr5690 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5690_HT,
};

static const struct pci_driver ht_driver_sr5670 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5670_HT,
};

static const struct pci_driver ht_driver_sr5650 __pci_driver = {
	.ops = &ht_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5650_HT,
};
