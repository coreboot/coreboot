/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
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

#include <arch/io.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include "rs690.h"


/*****************************************
* Compliant with CIM_33's ATINB_MiscClockCtrl
*****************************************/
void static rs690_config_misc_clk(struct device *nb_dev)
{
	u32 reg;
	u16 word;
	/* u8 byte; */
	struct bus pbus; /* fake bus for dev0 fun1 */

	reg = pci_read_config32(nb_dev, 0x4c);
	reg |= 1 << 0;
	pci_write_config32(nb_dev, 0x4c, reg);

	word = pci_cf8_conf1.read16(&pbus, 0, 1, 0xf8);
	word &= 0xf00;
	pci_cf8_conf1.write16(&pbus, 0, 1, 0xf8, word);

	word = pci_cf8_conf1.read16(&pbus, 0, 1, 0xe8);
	word &= ~((1 << 12) | (1 << 13) | (1 << 14));
	word |= 1 << 13;
	pci_cf8_conf1.write16(&pbus, 0, 1, 0xe8, word);

	reg =  pci_cf8_conf1.read32(&pbus, 0, 1, 0x94);
	reg &= ~((1 << 16) | (1 << 24) | (1 << 28));
	pci_cf8_conf1.write32(&pbus, 0, 1, 0x94, reg);

	reg = pci_cf8_conf1.read32(&pbus, 0, 1, 0x8c);
	reg &= ~((1 << 13) | (1 << 14) | (1 << 24) | (1 << 25));
	reg |= 1 << 13;
	pci_cf8_conf1.write32(&pbus, 0, 1, 0x8c, reg);

	reg = pci_cf8_conf1.read32(&pbus, 0, 1, 0xcc);
	reg |= 1 << 24;
	pci_cf8_conf1.write32(&pbus, 0, 1, 0xcc, reg);

	reg = nbmc_read_index(nb_dev, 0x7a);
	reg &= ~0x3f;
	reg |= 1 << 2;
	reg &= ~(1 << 6);
	set_htiu_enable_bits(nb_dev, 0x05, 1 << 11, 1 << 11);
	nbmc_write_index(nb_dev, 0x7a, reg);
	/* Powering Down efuse and strap block clocks after boot-up. GFX Mode. */
	reg = pci_cf8_conf1.read32(&pbus, 0, 1, 0xcc);
	reg &= ~(1 << 23);
	reg |= 1 << 24;
	pci_cf8_conf1.write32(&pbus, 0, 1, 0xcc, reg);
#if 0
	/* Powerdown reference clock to graphics core PLL in northbridge only mode */
	reg = pci_cf8_conf1.read32(&pbus, 0, 1, 0x8c);
	reg |= 1 << 21;
	pci_cf8_conf1.write32(&pbus, 0, 1, 0x8c, reg);

	/* Powering Down efuse and strap block clocks after boot-up. NB Only Mode. */
	reg = pci_cf8_conf1.read32(&pbus, 0, 1, 0xcc);
	reg |= (1 << 23) | (1 << 24);
	pci_cf8_conf1.write32(&pbus, 0, 1, 0xcc, reg);

	/* Powerdown clock to memory controller in northbridge only mode */
	byte = pci_cf8_conf1.read8(&pbus, 0, 1, 0xe4);
	byte |= 1 << 0;
	pci_cf8_conf1.write8(&pbus, 0, 1, 0xe4, reg);

	/* CLKCFG:0xE8 Bit[17] = 0x1 	 Powerdown clock to IOC GFX block in no external graphics mode */
	/* TODO: */
#endif

	reg = pci_read_config32(nb_dev, 0x4c);
	reg &= ~(1 << 0);
	pci_write_config32(nb_dev, 0x4c, reg);

	set_htiu_enable_bits(nb_dev, 0x05, 7 << 8, 7 << 8);
}

static u32 get_vid_did(struct device *dev)
{
	return pci_read_config32(dev, 0);
}

/***********************************************
*	0:00.0  NBCFG	:
*	0:00.1  CLK	: bit 0 of nb_cfg 0x4c : 0 - disable, default
*	0:01.0  P2P Internal:
*	0:02.0  P2P	: bit 2 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:03.0  P2P	: bit 3 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:04.0  P2P	: bit 4 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:05.0  P2P	: bit 5 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:06.0  P2P	: bit 6 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:07.0  P2P	: bit 7 of nbmiscind 0x0c : 0 - enable, default	   + 32 * 2
*	0:08.0  NB2SB	: bit 6 of nbmiscind 0x00 : 0 - disable, default   + 32 * 1
* case 0 will be called twice, one is by CPU in hypertransport.c line458,
* the other is by rs690.
***********************************************/
void rs690_enable(struct device *dev)
{
	struct device *nb_dev = NULL, *sb_dev = NULL;
	int dev_ind;

	printk(BIOS_INFO, "rs690_enable: dev=%p, VID_DID=0x%x\n", dev, get_vid_did(dev));

	nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		die("rs690_enable: CAN NOT FIND RS690 DEVICE, HALT!\n");
		/* NOT REACHED */
	}

	/* sb_dev (dev 8) is a bridge that links to southbridge. */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	if (!sb_dev) {
		die("rs690_enable: CAN NOT FIND SB bridge, HALT!\n");
		/* NOT REACHED */
	}

	dev_ind = dev->path.pci.devfn >> 3;
	switch (dev_ind) {
	case 0:		/* bus0, dev0, fun0; */
		printk(BIOS_INFO, "Bus-0, Dev-0, Fun-0.\n");
		enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */
		config_gpp_core(nb_dev, sb_dev);
		rs690_gpp_sb_init(nb_dev, sb_dev, 8);
		/* set SB payload size: 64byte */
		set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPPSB, 3 << 11, 2 << 11);

		/* Bus0Dev0Fun1Clock control init, we have to do it here, for dev0 Fun1 doesn't have a vendor or device ID */
		rs690_config_misc_clk(nb_dev);
		break;

	case 1:		/* bus0, dev1 */
		printk(BIOS_INFO, "Bus-0, Dev-1, Fun-0.\n");
		break;
	case 2:		/* bus0, dev2,3, two GFX */
	case 3:
		printk(BIOS_INFO, "Bus-0, Dev-2,3, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			rs690_gfx_init(nb_dev, dev, dev_ind);
		break;
	case 4:		/* bus0, dev4-7, four GPP */
	case 5:
	case 6:
	case 7:
		printk(BIOS_INFO, "Bus-0, Dev-4,5,6,7, Fun-0. enable=%d\n",
			    dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			rs690_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	case 8:		/* bus0, dev8, SB */
		printk(BIOS_INFO, "Bus-0, Dev-8, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 6,
				       (dev->enabled ? 1 : 0) << 6);
		if (dev->enabled)
			rs690_gpp_sb_init(nb_dev, dev, dev_ind);
		disable_pcie_bar3(nb_dev);
		break;
	default:
		printk(BIOS_DEBUG, "unknown dev: %s\n", dev_path(dev));
	}
}

struct chip_operations southbridge_amd_rs690_ops = {
	CHIP_NAME("ATI RS690")
	.enable_dev = rs690_enable,
};
