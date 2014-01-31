/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include <baytrail/iosf.h>
#include <baytrail/pci_devs.h>
#include <baytrail/ramstage.h>

/* Host Memory Map:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Usable DRAM          |
 * +--------------------------+ 4GiB
 * |     PCI Address Space    |
 * +--------------------------+ BMBOUND
 * |     TPM                  |
 * +--------------------------+ IMR2
 * |     TXE                  |
 * +--------------------------+ IMR1
 * |     iGD                  |
 * +--------------------------+
 * |     GTT                  |
 * +--------------------------+ SMMRRH, IRM0
 * |     TSEG                 |
 * +--------------------------+ SMMRRL
 * |     Usable DRAM          |
 * +--------------------------+ 0
 *
 * Note that there are really only a few regions that need to enumerated w.r.t.
 * coreboot's resrouce model:
 *
 * +--------------------------+ BMBOUND_HI
 * |     Cacheable/Usable     |
 * +--------------------------+ 4GiB
 *
 * +--------------------------+ BMBOUND
 * |    Uncacheable/Reserved  |
 * +--------------------------+ SMMRRH
 * |    Cacheable/Reserved    |
 * +--------------------------+ SMMRRL
 * |     Cacheable/Usable     |
 * +--------------------------+ 0
 */
#define RES_IN_KiB(r) ((r) >> 10)

static void soc_trans_router_read_resources(device_t dev)
{
	unsigned long mmconf;
	unsigned long bmbound;
	unsigned long bmbound_hi;
	unsigned long smmrrh;
	unsigned long smmrrl;
	unsigned long base_k, size_k;
	const unsigned long four_gig_kib = (4 << (30 - 10));
	int index = 0;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* PCIe memory-mapped config space access - 256 MiB. */
	mmconf = iosf_bunit_read(BUNIT_MMCONF_REG) & ~((1 << 28) - 1);
	mmio_resource(dev, BUNIT_MMCONF_REG, RES_IN_KiB(mmconf), 256 * 1024);

	/* 0 -> SMM_DEFAULT_BASE cacheable ram. */
	ram_resource(dev, index++, 0, RES_IN_KiB(SMM_DEFAULT_BASE));
	/* Default SMM region is cacheable but reserved for coreboot */
	reserved_ram_resource(dev, index++, RES_IN_KiB(SMM_DEFAULT_BASE),
	                      RES_IN_KiB(SMM_DEFAULT_SIZE));

	/* SMM_DEFAULT_BASE + SMM_DEFAULT_SIZE - > 0xa0000 */
	base_k = RES_IN_KiB(SMM_DEFAULT_BASE + SMM_DEFAULT_SIZE);
	size_k = RES_IN_KiB(0xa0000) - base_k;
	ram_resource(dev, index++, base_k, size_k);

	/* The SMMRR registers are 1MiB granularity with smmrrh being
	 * inclusive of the SMM region. */
	smmrrl = (iosf_bunit_read(BUNIT_SMRRL) & 0xffff) << 10;
	smmrrh = ((iosf_bunit_read(BUNIT_SMRRH) & 0xffff) + 1) << 10;

	/* 0xc0000 -> smrrl - cacheable and usable */
	base_k = RES_IN_KiB(0xc0000);
	size_k = smmrrl - base_k;
	ram_resource(dev, index++, base_k, size_k);

	if (smmrrh > smmrrl)
		reserved_ram_resource(dev, index++, smmrrl, smmrrh - smmrrl);

	/* All address space between bmbound and smmrrh is unusable. */
	bmbound = RES_IN_KiB(iosf_bunit_read(BUNIT_BMBOUND) & ~((1 << 27) - 1));
	mmio_resource(dev, index++, smmrrh, bmbound - smmrrh);

	/* The BMBOUND_HI register matches register bits of 31:24 with address
	 * bits of 35:28. Therefore, shift register to align properly. */
	bmbound_hi = iosf_bunit_read(BUNIT_BMBOUND_HI) & ~((1 << 24) - 1);
	bmbound_hi = RES_IN_KiB(bmbound_hi) << 4;
	if (bmbound_hi > four_gig_kib)
		ram_resource(dev, index++, four_gig_kib,
		             bmbound_hi - four_gig_kib);
}

static void nc_read_resources(device_t dev)
{
	/* For some reason the graphics pci device (0, 2, 0) and the SoC
	 * transaction router are coming up with the same device id. */
	if (dev_find_slot(0, PCI_DEVFN(SOC_DEV, SOC_FUNC)) == dev)
		return soc_trans_router_read_resources(dev);
	else
		/* Read standard PCI resources. */
		pci_dev_read_resources(dev);

}

static struct device_operations nc_ops = {
	.read_resources   = nc_read_resources,
	.set_resources    = NULL,
	.enable_resources = NULL,
	.init             = NULL,
	.enable           = NULL,
	.scan_bus         = NULL,
	.ops_pci          = &soc_pci_ops,
};

static const struct pci_driver nc_driver __pci_driver = {
	.ops    = &nc_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.device = SOC_DEVID,
};

