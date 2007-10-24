/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>

static void ctrl_enable(struct device *dev)
{
	u8 regm, regm2, regm3;
	device_t devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
					   PCI_DEVICE_ID_VIA_K8T890CE_3, 0);

	/* TODO: Fix some ordering issue fo V-link set Rx77[6] and PCI1_Rx4F[0]
	   should to 1 */

	/* C2P Read ACK Return Priority */
	/* PCI CFG Address bits[27:24] are used as extended register address
	   bit[11:8] */
	pci_write_config8(dev, 0x47, 0x30);

	/* FIXME: Program V-link 8X 16bit full duplex, this needs to be fixed
	   for other than VT8237R SB */
	pci_write_config8(dev, 0x48, 0x23);

	/* Magic init. This is not well documented :/ */
	pci_write_config8(dev, 0x70, 0xc2);

	/* PCI Control */
	pci_write_config8(dev, 0x72, 0xee);
	pci_write_config8(dev, 0x73, 0x01);
	pci_write_config8(dev, 0x74, 0x24);
	pci_write_config8(dev, 0x75, 0x0f);
	pci_write_config8(dev, 0x76, 0x50);
	pci_write_config8(dev, 0x77, 0x08);
	pci_write_config8(dev, 0x78, 0x01);
	/* APIC on HT */
	pci_write_config8(dev, 0x7c, 0x7f);
	pci_write_config8(dev, 0x7f, 0x02);

	/* WARNING: Need to copy some registers from NB (D0F3) to SB (D0F7). */

	regm = pci_read_config8(devfun3, 0x88);	/* Shadow mem CTRL */
	pci_write_config8(dev, 0x57, regm);

	regm = pci_read_config8(devfun3, 0x80);	/* Shadow page C */
	pci_write_config8(dev, 0x61, regm);

	regm = pci_read_config8(devfun3, 0x81);	/* Shadow page D */
	pci_write_config8(dev, 0x62, regm);

	regm = pci_read_config8(devfun3, 0x86);	/* SMM and APIC decoding */
	pci_write_config8(dev, 0xe6, regm);

	regm3 = pci_read_config8(devfun3, 0x82);/* Shadow page E */

	/*
	 * All access bits for 0xE0000-0xEFFFF encode as just 2 bits!
	 * So the NB reg is quite inconsistent, we expect there only 0xff or 0x00,
	 * and write them to 0x63 7-6 but! VIA 8237A has the mirror at 0x64!
	 */
	if (regm3 == 0xff)
		regm3 = 0xc0;
	else
		regm3 = 0x0;

	/* Shadow page F + memhole copy */
	regm = pci_read_config8(devfun3, 0x83);
	pci_write_config8(dev, 0x63, regm3 | (regm & 0x3F));
}

static struct device_operations ctrl_ops = {
	.read_resources = pci_dev_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.enable = ctrl_enable,
	.ops_pci = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &ctrl_ops,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_K8T890CE_7,
};
