/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2011 Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cbmem.h>
#include <arch/io.h>
#include "k8x8xx.h"

/* this may be later merged */

/* This fine tunes the HT link settings, which were loaded by ROM strap. */
static void host_ctrl_enable_k8t8xx(struct device *dev)
{
	/*
	 * Bit 4 is reserved but set by AW. Set PCI to HT outstanding
	 * requests to 3.
	 */
	pci_write_config8(dev, 0xa0, 0x13);

	/*
	 * NVRAM I/O base at K8T890_NVRAM_IO_BASE
	 * Some bits are set and reserved.
	 */
	pci_write_config8(dev, 0xa2, (K8T890_NVRAM_IO_BASE >> 8));

	/* enable NB NVRAM and enable non-posted PCI writes. */
	pci_write_config8(dev, 0xa1, 0x8f);
	/* Arbitration control, some bits are reserved. */
	pci_write_config8(dev, 0xa5, 0x3c);

	/* Arbitration control 2 */
	pci_write_config8(dev, 0xa6, 0x80);

	/* this will be possibly removed, when I figure out
	 * if the ROM SIP is good, second reason is that the
	 * unknown bits are AGP related, which are dummy on K8T890
	 */

	writeback(dev, 0xa0, 0x13);	/* Bit4 is reserved! */
	writeback(dev, 0xa1, 0x8e);	/* Some bits are reserved. */
	writeback(dev, 0xa2, 0x0e);	/* I/O NVRAM base 0xe00-0xeff disabled. */
	writeback(dev, 0xa3, 0x31);
	writeback(dev, 0xa4, 0x30);

	writeback(dev, 0xa5, 0x3c);	/* Some bits reserved. */
	writeback(dev, 0xa6, 0x80);	/* Some bits reserved. */
	writeback(dev, 0xa7, 0x86);	/* Some bits reserved. */
	writeback(dev, 0xa8, 0x7f);	/* Some bits reserved. */
	writeback(dev, 0xa9, 0xcf);	/* Some bits reserved. */
	writeback(dev, 0xaa, 0x44);
	writeback(dev, 0xab, 0x22);
	writeback(dev, 0xac, 0x35);	/* Maybe bit0 is read-only? */

	writeback(dev, 0xae, 0x22);
	writeback(dev, 0xaf, 0x40);
	/* b0 is missing. */
	writeback(dev, 0xb1, 0x13);
	writeback(dev, 0xb4, 0x02);	/* Some bits are reserved. */
	writeback(dev, 0xc0, 0x20);
	writeback(dev, 0xc1, 0xaa);
	writeback(dev, 0xc2, 0xaa);
	writeback(dev, 0xc3, 0x02);
	writeback(dev, 0xc4, 0x50);
	writeback(dev, 0xc5, 0x50);

	print_debug(" VIA_X_2 device dump:\n");
	dump_south(dev);
}

/* This fine tunes the HT link settings, which were loaded by ROM strap. */
static void host_ctrl_enable_k8m8xx(struct device *dev) {

	/*
	 * Set PCI to HT outstanding requests to 03.
	 * Bit 4 32 AGP ADS Read Outstanding Request Number
	 */
	pci_write_config8(dev, 0xa0, 0x13);

	/*
	 * NVRAM I/O base at K8T890_NVRAM_IO_BASE
	 */

	pci_write_config8(dev, 0xa2, (K8T890_NVRAM_IO_BASE >> 8));

	/* Enable NVRAM and enable non-posted PCI writes. */
	pci_write_config8(dev, 0xa1, 0x8f);

	/* Arbitration control  */
	pci_write_config8(dev, 0xa5, 0x3c);

	/* Arbitration control 2, Enable C2NOW delay to PSTATECTL */
	pci_write_config8(dev, 0xa6, 0x83);

}
void backup_top_of_ram(uint64_t ramtop) {
		outl((u32) ramtop, K8T890_NVRAM_IO_BASE+K8T890_NVRAM_TOP_OF_RAM);
}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static const struct device_operations host_ctrl_ops_t = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= host_ctrl_enable_k8t8xx,
	.ops_pci		= &lops_pci,
};

static const struct device_operations host_ctrl_ops_m = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= host_ctrl_enable_k8m8xx,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver northbridge_driver_t800 __pci_driver = {
	.ops	= &host_ctrl_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T800_HOST_CTR,
};

static const struct pci_driver northbridge_driver_m800 __pci_driver = {
	.ops	= &host_ctrl_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M800_HOST_CTR,
};

static const struct pci_driver northbridge_driver_t890 __pci_driver = {
	.ops	= &host_ctrl_ops_t,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_2,
};

static const struct pci_driver northbridge_driver_m890 __pci_driver = {
	.ops	= &host_ctrl_ops_m,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8M890CE_2,
};
