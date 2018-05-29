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
 */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "k8x8xx.h"

void k8x8xx_vt8237_mirrored_regs_fill(struct k8x8xx_vt8237_mirrored_regs *regs)
{
	msr_t msr;

	regs->rom_shadow_ctrl_pg_c = 0xff;
	regs->rom_shadow_ctrl_pg_d = 0xff;
	regs->rom_shadow_ctrl_pg_e_memhole_smi_decoding = 0xff;
	regs->rom_shadow_ctrl_pg_f_memhole = 0x30;
	regs->smm_apic_decoding = 0x19;
	msr = rdmsr(TOP_MEM);
	regs->shadow_mem_ctrl = msr.lo >> 24;
	regs->low_top_address = msr.lo >> 16;
}

/* We support here K8M890/K8T890 and VT8237R PCI1/Vlink which setup is not in separate
 * PCI device 0:11.7, but it is mapped to PCI 0:0.7 (0x70-0x7c for PCI1)
 */

void k8x8xx_vt8237r_cfg(struct device *dev, struct device *devsb)
{
	u8 regm3;
	struct k8x8xx_vt8237_mirrored_regs mregs;

	pci_write_config8(dev, 0x70, 0xc2);

	/* PCI Control */
#if !IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD)
	pci_write_config8(dev, 0x72, 0xee);
#endif
	pci_write_config8(dev, 0x73, 0x01);
#if IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD)
	pci_write_config8(dev, 0x74, 0x64);
	pci_write_config8(dev, 0x75, 0x3f);
#else
	pci_write_config8(dev, 0x74, 0x24);
	pci_write_config8(dev, 0x75, 0x0f);
#endif
	pci_write_config8(dev, 0x76, 0x50);
#if !IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD)
	pci_write_config8(dev, 0x77, 0x08);
#endif
	pci_write_config8(dev, 0x78, 0x01);
	/* APIC on HT */
	pci_write_config8(dev, 0x7c, 0x7f);
	pci_write_config8(dev, 0x7f, 0x02);

	k8x8xx_vt8237_mirrored_regs_fill(&mregs);

	pci_write_config8(dev, 0x57, mregs.shadow_mem_ctrl);		/* Shadow mem CTRL */
	pci_write_config8(dev, 0x61, mregs.rom_shadow_ctrl_pg_c);	/* Shadow page C */
	pci_write_config8(dev, 0x62, mregs.rom_shadow_ctrl_pg_d);	/* Shadow page D */
	pci_write_config8(dev, 0xe6, mregs.smm_apic_decoding);		/* SMM and APIC decoding */

	regm3 = mregs.rom_shadow_ctrl_pg_e_memhole_smi_decoding;	/* Shadow page E */

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
	pci_write_config8(dev, 0x63, regm3 | (mregs.rom_shadow_ctrl_pg_f_memhole & 0x3F));

}



/**
 * Setup the V-Link for VT8237R, 8X mode.
 *
 * For K8T890CF VIA recommends what is in VIA column, AW is award 8X:
 *
 *						 REG   DEF   AW  VIA-8X VIA-4X
 *						 -----------------------------
 * NB V-Link Manual Driving Control strobe	 0xb5  0x46  0x46  0x88  0x88
 * NB V-Link Manual Driving Control - Data	 0xb6  0x46  0x46  0x88  0x88
 * NB V-Link Receiving Strobe Delay		 0xb7  0x02  0x02  0x61  0x01
 * NB V-Link Compensation Control bit4,0 (b5,b6) 0xb4  0x10  0x10  0x11  0x11
 * SB V-Link Strobe Drive Control 		 0xb9  0x00  0xa5  0x98  0x98
 * SB V-Link Data drive Control????		 0xba  0x00  0xbb  0x77  0x77
 * SB V-Link Receive Strobe Delay????		 0xbb  0x04  0x11  0x11  0x11
 * SB V-Link Compensation Control bit0 (use b9)	 0xb8  0x00  0x01  0x01  0x01
 * V-Link CKG Control				 0xb0  0x05  0x05  0x06  0x03
 * V-Link CKG Control				 0xb1  0x05  0x05  0x01  0x03
 */

static void vt8237r_vlink_init(struct device *dev)
{
	u8 reg;

	/*
	 * This init code is valid only for the VT8237R! For different
	 * southbridges (e.g. VT8237A, VT8237S, VT8237 (without plus R)
	 * and VT8251) a different init code is required.
	 */

	pci_write_config8(dev, 0xb5, 0x88);
	pci_write_config8(dev, 0xb6, 0x88);
	pci_write_config8(dev, 0xb7, 0x61);

	reg = pci_read_config8(dev, 0xb4);
	reg |= 0x11;
	pci_write_config8(dev, 0xb4, reg);

	pci_write_config8(dev, 0xb9, 0x98);
	pci_write_config8(dev, 0xba, 0x77);
	pci_write_config8(dev, 0xbb, 0x11);

	reg = pci_read_config8(dev, 0xb8);
	reg |= 0x1;
	pci_write_config8(dev, 0xb8, reg);

	pci_write_config8(dev, 0xb0, 0x06);
	pci_write_config8(dev, 0xb1, 0x01);

	/* Program V-link 8X 16bit full duplex, parity enabled. */
	pci_write_config8(dev, 0x48, 0xa3);
}

static void ctrl_init(struct device *dev)
{

	printk(BIOS_DEBUG, "K8x8xx: Initializing V-Link to VT8237R sb: ");
	/* TODO: Fix some ordering issue for V-link set Rx77[6] and PCI1_Rx4F[0]
	   should to 1 */

	/* C2P Read ACK Return Priority */
	/* PCI CFG Address bits[27:24] are used as extended register address
	   bit[11:8] */

#if !IS_ENABLED(CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD)
	pci_write_config8(dev, 0x47, 0x30);
#endif

	/* VT8237R specific configuration  other SB are done in their own directories */

	struct device *devsb = dev_find_device(PCI_VENDOR_ID_VIA,
					       PCI_DEVICE_ID_VIA_VT8237R_LPC,
					       0);
	if (devsb) {
		vt8237r_vlink_init(dev);
		k8x8xx_vt8237r_cfg(dev, devsb);
	} else {
		printk(BIOS_DEBUG, "VT8237R LPC not found !\n");
		return;
	}
	printk(BIOS_DEBUG, " Done\n");
	printk(BIOS_DEBUG, " VIA_X_7 device dump:\n");
	dump_south(dev);

}

static struct pci_operations lops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static const struct device_operations ctrl_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ctrl_init,
	.ops_pci		= &lops_pci,
};

static const unsigned short ctrl_ids[] = {
	PCI_DEVICE_ID_VIA_K8T800_NB_SB_CTR,
	PCI_DEVICE_ID_VIA_K8M800_NB_SB_CTR,
	PCI_DEVICE_ID_VIA_K8T890CE_7,
	PCI_DEVICE_ID_VIA_K8T890CF_7,
	PCI_DEVICE_ID_VIA_K8M890CE_7,
	0
};

static const struct pci_driver northbridge_driver_t800 __pci_driver = {
	.ops	= &ctrl_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.devices= ctrl_ids,
};
