/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Rudolf Marek <r.marek@assembler.cz>
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
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "vt8237r.h"

/* We support here K8M890/K8T890 and VT8237/S/A PCI1/Vlink */

static void vt8237_cfg(struct device *dev)
{
	u8 regm;
	struct device *devfun3;

	devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
				  PCI_DEVICE_ID_VIA_K8T800_DRAM, 0);
	if (!devfun3)
		devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M800_DRAM, 0);
	if (!devfun3)
	  devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
				    PCI_DEVICE_ID_VIA_K8T890CE_3, 0);
	if (!devfun3)
		devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M890CE_3, 0);
	if (!devfun3)
		devfun3 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8T890CF_3, 0);
	if (!devfun3)
		die("Unknown NB");

	/* CPU to PCI Flow Control 1 & 2, just fill in recommended. */
	pci_write_config8(dev, 0x70, 0xc2);
	pci_write_config8(dev, 0x71, 0xc8);

	/* PCI Control */
	pci_write_config8(dev, 0x72, 0xee);
	pci_write_config8(dev, 0x73, 0x01);
	pci_write_config8(dev, 0x74, 0x3c);
	pci_write_config8(dev, 0x75, 0x0f);
	pci_write_config8(dev, 0x76, 0x50);
	pci_write_config8(dev, 0x77, 0x48);
	pci_write_config8(dev, 0x78, 0x01);
	/* APIC on HT */
	/* Maybe Enable LDT APIC Mode bit3 set to 1 */
	pci_write_config8(dev, 0x7c, 0x77);

	/* WARNING: Need to copy some registers from NB (D0F3) to SB (D11F7). */

	regm = pci_read_config8(devfun3, 0x88);	/* Shadow mem CTRL */
	pci_write_config8(dev, 0x57, regm);

	regm = pci_read_config8(devfun3, 0x80);	/* Shadow page C */
	pci_write_config8(dev, 0x61, regm);

	regm = pci_read_config8(devfun3, 0x81);	/* Shadow page D */
	pci_write_config8(dev, 0x62, regm);

	/* Shadow page F + memhole copy */
	regm = pci_read_config8(devfun3, 0x83);
	pci_write_config8(dev, 0x63, regm);

	regm = pci_read_config8(devfun3, 0x82);/* Shadow page E */
	pci_write_config8(dev, 0x64, regm);

	regm = pci_read_config8(devfun3, 0x86);	/* SMM and APIC decoding */
	pci_write_config8(dev, 0xe6, regm);
}

/**
 * Example of setup: Setup the V-Link for VT8237R, 8X mode.
 *
 * For K8T890CF VIA recommends what is in VIA column, AW is award 8X:
 *
 *						 REG   DEF   AW  VIA-8X VIA-4X
 *						 -----------------------------
 * NB V-Link Manual Driving Control strobe	 0xb5  0x46  0x46  0x88  0x88
 * NB V-Link Manual Driving Control - Data	 0xb6  0x46  0x46  0x88  0x88
 * NB V-Link Receiving Strobe Delay		 0xb7  0x02  0x02  0x61  0x01
 * NB V-Link Compensation Control bit4,0 (b5,b6) 0xb4  0x10  0x10  0x11  0x11
 * SB V-Link Strobe Drive Control		 0xb9  0x00  0xa5  0x98  0x98
 * SB V-Link Data drive Control????		 0xba  0x00  0xbb  0x77  0x77
 * SB V-Link Receive Strobe Delay????		 0xbb  0x04  0x11  0x11  0x11
 * SB V-Link Compensation Control bit0 (use b9)	 0xb8  0x00  0x01  0x01  0x01
 * V-Link CKG Control				 0xb0  0x05  0x05  0x06  0x03
 * V-Link CKG Control				 0xb1  0x05  0x05  0x01  0x03
 */

/* we setup 533MB/s mode full duplex */

static void vt8237s_vlink_init(struct device *dev)
{
	u8 reg;
	struct device *devfun7;

	devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
				  PCI_DEVICE_ID_VIA_K8T800_NB_SB_CTR, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M800_NB_SB_CTR, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8T890CE_7, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M890CE_7, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8T890CF_7, 0);
	/* No pairing NB was found. */
	if (!devfun7)
	{
		printk(BIOS_DEBUG, "vt8237s_vlink_init: No pairing NB was found.\n");
		return;
	}

	/*
	 * This init code is valid only for the VT8237S! For different
	 * southbridges (e.g. VT8237A, VT8237S, VT8237R (without plus R)
	 * and VT8251) a different init code is required.
	 */

	/* disable auto disconnect */
	reg = pci_read_config8(devfun7, 0x42);
	reg &= ~0x4;
	pci_write_config8(devfun7, 0x42, reg);

	/* NB part setup */
	pci_write_config8(devfun7, 0xb5, 0x66);
	pci_write_config8(devfun7, 0xb6, 0x66);
	pci_write_config8(devfun7, 0xb7, 0x64);

	reg = pci_read_config8(devfun7, 0xb4);
	reg |= 0x1;
	reg &= ~0x10;
	pci_write_config8(devfun7, 0xb4, reg);

	pci_write_config8(devfun7, 0xb0, 0x6);
	pci_write_config8(devfun7, 0xb1, 0x1);

	/* SB part setup */
	pci_write_config8(dev, 0xb7, 0x60);
	pci_write_config8(dev, 0xb9, 0x88);
	pci_write_config8(dev, 0xba, 0x88);
	pci_write_config8(dev, 0xbb, 0x89);

	reg = pci_read_config8(dev, 0xbd);
	reg |= 0x3;
	reg &= ~0x4;
	pci_write_config8(dev, 0xbd, reg);

	reg = pci_read_config8(dev, 0xbc);
	reg &= ~0x7;
	pci_write_config8(dev, 0xbc, reg);

	/* Program V-link 8X 8bit full duplex, parity enabled.  */
	pci_write_config8(dev, 0x48, 0x23 | 0x80);

	/* enable auto disconnect, for STPGNT and HALT */
	reg = pci_read_config8(devfun7, 0x42);
	reg |= 0x7;
	pci_write_config8(devfun7, 0x42, reg);

}

static void vt8237a_vlink_init(struct device *dev)
{
	u8 reg;
	struct device *devfun7;

	devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
				  PCI_DEVICE_ID_VIA_K8T800_NB_SB_CTR, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M800_NB_SB_CTR, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8T890CE_7, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8M890CE_7, 0);
	if (!devfun7)
		devfun7 = dev_find_device(PCI_VENDOR_ID_VIA,
					  PCI_DEVICE_ID_VIA_K8T890CF_7, 0);
	/* No pairing NB was found. */
	if (!devfun7)
	{
		printk(BIOS_DEBUG, "vt8237a_vlink_init: No pairing NB was found.\n");
		return;
	}

	/*
	 * This init code is valid only for the VT8237A! For different
	 * southbridges (e.g. VT8237S, VT8237R and VT8251) a different
	 * init code is required.
	 *
	 * FIXME: This is based on vt8237r_vlink_init() in
	 *        k8t890/k8t890_ctrl.c and modified to fit what the AMI
	 *        BIOS on my M2V wrote to these registers (by looking
	 *        at lspci -nxxx output).
	 *        Works for me.
	 */

	/* disable auto disconnect */
	reg = pci_read_config8(devfun7, 0x42);
	reg &= ~0x4;
	pci_write_config8(devfun7, 0x42, reg);

	/* NB part setup */
	pci_write_config8(devfun7, 0xb5, 0x88);
	pci_write_config8(devfun7, 0xb6, 0x88);
	pci_write_config8(devfun7, 0xb7, 0x61);

	reg = pci_read_config8(devfun7, 0xb4);
	reg |= 0x11;
	pci_write_config8(devfun7, 0xb4, reg);

	pci_write_config8(devfun7, 0xb0, 0x6);
	pci_write_config8(devfun7, 0xb1, 0x1);

	/* SB part setup */
	pci_write_config8(dev, 0xb7, 0x50);
	pci_write_config8(dev, 0xb9, 0x88);
	pci_write_config8(dev, 0xba, 0x8a);
	pci_write_config8(dev, 0xbb, 0x88);

	reg = pci_read_config8(dev, 0xbd);
	reg |= 0x3;
	reg &= ~0x4;
	pci_write_config8(dev, 0xbd, reg);

	reg = pci_read_config8(dev, 0xbc);
	reg &= ~0x7;
	pci_write_config8(dev, 0xbc, reg);

	pci_write_config8(dev, 0x48, 0x23);

	/* enable auto disconnect, for STPGNT and HALT */
	reg = pci_read_config8(devfun7, 0x42);
	reg |= 0x7;
	pci_write_config8(devfun7, 0x42, reg);
}

static void ctrl_enable(struct device *dev)
{
	/* Enable the 0:13 and 0:13.1. */
	/* FIXME */
	pci_write_config8(dev, 0x4f, 0x43);
}

static void ctrl_init(struct device *dev)
{
	/*
	 * TODO: Fix some ordering issue for V-link set Rx77[6] and
	 * PCI1_Rx4F[0] should to 1.
	 * FIXME DO you need?
	 */

	/*
	 * VT8237R specific configuration. Other SB are done in their own
	 * directories. TODO: Add A version.
	 */
	struct device *devsb = dev_find_device(PCI_VENDOR_ID_VIA,
					       PCI_DEVICE_ID_VIA_VT8237S_LPC,
					       0);
	if (devsb) {
		vt8237s_vlink_init(dev);
	}

	devsb = dev_find_device(PCI_VENDOR_ID_VIA,
				PCI_DEVICE_ID_VIA_VT8237A_LPC, 0);
	if (devsb) {
		vt8237a_vlink_init(dev);
	}

	/* Configure PCI1 and copy mirror registers from D0F3. */
	vt8237_cfg(dev);
	dump_south(dev);
}

static const struct device_operations ctrl_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ctrl_init,
	.enable			= ctrl_enable,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver_t __pci_driver = {
	.ops	= &ctrl_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237_VLINK,
};
