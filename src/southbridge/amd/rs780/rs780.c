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
 */

#include <console/console.h>
#include <arch/io.h>
#include <arch/acpi.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include "rs780.h"

/*****************************************
* rs780_config_misc_clk()
*****************************************/
void static rs780_config_misc_clk(struct device *nb_dev)
{
	u32 reg;
	u16 word;
	u8 byte;
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

	/* Programming NB CLK table. */
	byte = pci_cf8_conf1.read8(&pbus, 0, 1, 0xe0);
	byte |= 0x01;
	pci_cf8_conf1.write8(&pbus, 0, 1, 0xe0, byte);

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

	/* CLKCFG:0xE8 Bit[17] = 0x1	 Powerdown clock to IOC GFX block in no external graphics mode */
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

static void rs780_nb_pci_table(struct device *nb_dev)
{	/* NBPOR_InitPOR function. */
	u8 temp8;
	u16 temp16;
	u32 temp32;

	/* Program NB PCI table. */
	temp16 = pci_read_config16(nb_dev, 0x04);
	printk(BIOS_DEBUG, "NB_PCI_REG04 = %x.\n", temp16);
	temp32 = pci_read_config32(nb_dev, 0x84);
	printk(BIOS_DEBUG, "NB_PCI_REG84 = %x.\n", temp32);

	pci_write_config8(nb_dev, 0x4c, 0x42);

	temp8 = pci_read_config8(nb_dev, 0x4e);
	temp8 |= 0x05;
	pci_write_config8(nb_dev, 0x4e, temp8);

	temp32 = pci_read_config32(nb_dev, 0x4c);
	printk(BIOS_DEBUG, "NB_PCI_REG4C = %x.\n", temp32);

	/* set temporary NB TOM to 0x40000000. */
	rs780_set_tom(nb_dev);

	/* Program NB HTIU table. */
#if 0
	set_htiu_enable_bits(nb_dev, 0x05, 1<<10 | 1<<9, 1<<10|1<<9);
	set_htiu_enable_bits(nb_dev, 0x06, 1, 0x4203a202);
	set_htiu_enable_bits(nb_dev, 0x07, 1<<1 | 1<<2, 0x8001);
	set_htiu_enable_bits(nb_dev, 0x15, 0, 1<<31 | 1<<30 | 1<<27);
	set_htiu_enable_bits(nb_dev, 0x1c, 0, 0xfffe0000);
	set_htiu_enable_bits(nb_dev, 0x4b, 1<<11, 1<<11);
	set_htiu_enable_bits(nb_dev, 0x0c, 0x3f, 1 | 1<<3);
	set_htiu_enable_bits(nb_dev, 0x17, 1<<1 | 1<<27, 1<<1);
	set_htiu_enable_bits(nb_dev, 0x17, 0, 1<<30);
	set_htiu_enable_bits(nb_dev, 0x19, 0xfffff+(1<<31), 0x186a0+(1<<31));
	set_htiu_enable_bits(nb_dev, 0x16, 0x3f<<10, 0x7<<10);
	set_htiu_enable_bits(nb_dev, 0x23, 0, 1<<28);

	/* Program NB MISC table. */
	set_nbmisc_enable_bits(nb_dev, 0x0b, 0xffff, 0x00000180);
	set_nbmisc_enable_bits(nb_dev, 0x00, 0xffff, 0x00000106);
	set_nbmisc_enable_bits(nb_dev, 0x51, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x53, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x55, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x57, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x59, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x5b, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x5d, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x5f, 0xffffffff, 0x00100100);
	set_nbmisc_enable_bits(nb_dev, 0x20, 1<<1, 0);
	set_nbmisc_enable_bits(nb_dev, 0x37, 1<<11|1<<12|1<<13|1<<26, 0);
	set_nbmisc_enable_bits(nb_dev, 0x68, 1<<5|1<<6, 1<<5);
	set_nbmisc_enable_bits(nb_dev, 0x6b, 1<<22, 1<<10);
	set_nbmisc_enable_bits(nb_dev, 0x67, 1<<26, 1<<14|1<<10);
	set_nbmisc_enable_bits(nb_dev, 0x24, 1<<28|1<<26|1<<25|1<<16, 1<<29|1<<25);
	set_nbmisc_enable_bits(nb_dev, 0x38, 1<<24|1<<25, 1<<24);
	set_nbmisc_enable_bits(nb_dev, 0x36, 1<<29, 1<<29|1<<28);
	set_nbmisc_enable_bits(nb_dev, 0x0c, 0, 1<<13);
	set_nbmisc_enable_bits(nb_dev, 0x34, 1<<22, 1<<10);
	set_nbmisc_enable_bits(nb_dev, 0x39, 1<<10, 1<<30);
	set_nbmisc_enable_bits(nb_dev, 0x22, 1<<3, 0);
	set_nbmisc_enable_bits(nb_dev, 0x68, 1<<19, 0);
	set_nbmisc_enable_bits(nb_dev, 0x24, 1<<16|1<<17, 1<<17);
	set_nbmisc_enable_bits(nb_dev, 0x6a, 1<<22|1<<23, 1<<17|1<<23);
	set_nbmisc_enable_bits(nb_dev, 0x35, 1<<21|1<<22, 1<<22);
	set_nbmisc_enable_bits(nb_dev, 0x01, 0xffffffff, 0x48);

	/* the last two step. */
	set_nbmisc_enable_bits(nb_dev, 0x01, 1<<8, 1<<8);
	set_htiu_enable_bits(nb_dev, 0x2d, 1<<6|1<<4, 1<<6|1<<4);
#endif
}

static void rs780_nb_gfx_dev_table(struct device *nb_dev, struct device *dev)
{
	/* NB_InitGFXStraps */
	u32 MMIOBase, apc04, apc18, apc24, romstrap2;
	volatile u32 * strap;

	/* Choose a base address that is unused and routed to the RS780. */
	MMIOBase = 0xFFB00000;

	/* 1E: NB_BIF_SPARE */
	set_nbmisc_enable_bits(nb_dev, 0x1e, 0xffffffff, 1<<1 | 1<<4 | 1<<6 | 1<<7);
	/* Set a temporary Bus number. */
	apc18 = pci_read_config32(dev, 0x18);
	pci_write_config32(dev, 0x18, 0x010100);
	/* Set MMIO window for AGP target(graphics controller). */
	apc24 = pci_read_config32(dev, 0x24);
	pci_write_config32(dev, 0x24, (MMIOBase>>16)+((MMIOBase+0x20000)&0xffff0000));
	/* Enable memory access. */
	apc04 = pci_read_config32(dev, 0x04);
	pci_write_config8(dev, 0x04, 0x02);

	/* Program Straps. */
	romstrap2 = 1 << 26; // enables audio function
#if IS_ENABLED(CONFIG_GFXUMA)
	// bits 7-9: aperture size
	// 0-7: 128mb, 256mb, 64mb, 32mb, 512mb, 1g, 2g, 4g
	if (uma_memory_size == 0x02000000) romstrap2 |= 3 << 7;
	if (uma_memory_size == 0x04000000) romstrap2 |= 2 << 7;
	if (uma_memory_size == 0x08000000) romstrap2 |= 0 << 7;
	if (uma_memory_size == 0x10000000) romstrap2 |= 1 << 7;
	if (uma_memory_size == 0x20000000) romstrap2 |= 4 << 7;
	if (uma_memory_size == 0x40000000) romstrap2 |= 5 << 7;
	if (uma_memory_size == 0x80000000) romstrap2 |= 6 << 7;
#endif
	strap = (volatile u32 *)(MMIOBase + 0x15020);
	*strap = romstrap2;
	strap = (volatile u32 *)(MMIOBase + 0x15000);
	*strap = 0x2c006300;
	strap = (volatile u32 *)(MMIOBase + 0x15010);
	*strap = 0x03015330;
	strap = (volatile u32 *)(MMIOBase + 0x15020);
	*strap = romstrap2 | 0x00000040;
	strap = (volatile u32 *)(MMIOBase + 0x15030);
	*strap = 0x00001002;
	strap = (volatile u32 *)(MMIOBase + 0x15040);
	*strap = 0x00000000;
	strap = (volatile u32 *)(MMIOBase + 0x15050);
	*strap = 0x00000000;
	strap = (volatile u32 *)(MMIOBase + 0x15220);
	*strap = 0x03c03800;
	strap = (volatile u32 *)(MMIOBase + 0x15060);
	*strap = 0x00000000;

	/* BIF switches into normal functional mode. */
	set_nbmisc_enable_bits(nb_dev, 0x1e, 1<<4 | 1<<5, 1<<5);

	/* NB Revision is A12 or newer */
	if (get_nb_rev(nb_dev) >= REV_RS780_A12)
		set_nbmisc_enable_bits(nb_dev, 0x1e, 1<<9, 1<<9);

	/* Restore APC04, APC18, APC24. */
	pci_write_config32(dev, 0x04, apc04);
	pci_write_config32(dev, 0x18, apc18);
	pci_write_config32(dev, 0x24, apc24);

	printk(BIOS_INFO, "GC is accessible from now on.\n");
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
* the other is by rs780.
***********************************************/
void rs780_enable(struct device *dev)
{
	struct device *nb_dev = NULL, *sb_dev = NULL;
	int dev_ind;

	printk(BIOS_INFO, "rs780_enable: dev=%p, VID_DID=0x%x\n", dev, get_vid_did(dev));

	nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		die("rs780_enable: CAN NOT FIND RS780 DEVICE, HALT!\n");
		/* NOT REACHED */
	}

	/* sb_dev (dev 8) is a bridge that links to southbridge. */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	if (!sb_dev) {
		die("rs780_enable: CAN NOT FIND SB bridge, HALT!\n");
		/* NOT REACHED */
	}

	dev_ind = dev->path.pci.devfn >> 3;
	switch (dev_ind) {
	case 0:		/* bus0, dev0, fun0; */
		printk(BIOS_INFO, "Bus-0, Dev-0, Fun-0.\n");
		enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */
		config_gpp_core(nb_dev, sb_dev);
		rs780_gpp_sb_init(nb_dev, sb_dev, 8);
		/* 5.10.8.4. set SB payload size: 64byte */
		set_pcie_enable_bits(nb_dev, 0x10 | PCIE_CORE_INDEX_GPPSB, 3 << 11, 2 << 11);

		/* Bus0Dev0Fun1Clock control init, we have to do it here, for dev0 Fun1 doesn't have a vendor or device ID */
		rs780_config_misc_clk(nb_dev);

		rs780_nb_pci_table(nb_dev);
		break;

	case 1: /* bus0, dev1, APC. */
		printk(BIOS_INFO, "Bus-0, Dev-1, Fun-0.\n");
		rs780_nb_gfx_dev_table(nb_dev, dev);
		break;
	case 2:		/* bus0, dev2,3, two GFX */
	case 3:
		printk(BIOS_INFO, "Bus-0, Dev-2,3, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			rs780_gfx_init(nb_dev, dev, dev_ind);
		break;
	case 4:		/* bus0, dev4-7, four GPPSB */
	case 5:
	case 6:
	case 7:
		printk(BIOS_INFO, "Bus-0, Dev-4,5,6,7, Fun-0. enable=%d\n",
			    dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			rs780_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	case 8:		/* bus0, dev8, SB */
		printk(BIOS_INFO, "Bus-0, Dev-8, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 6,
				       (dev->enabled ? 1 : 0) << 6);
		if (dev->enabled)
			rs780_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	case 9:		/* bus 0, dev 9,10, GPP */
	case 10:
		printk(BIOS_INFO, "Bus-0, Dev-9, 10, Fun-0. enable=%d\n",
			    dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << (7 + dev_ind),
				       (dev->enabled ? 0 : 1) << (7 + dev_ind));
		if (dev->enabled)
			rs780_gpp_sb_init(nb_dev, dev, dev_ind);

		if (dev_ind == 10) {
			disable_pcie_bar3(nb_dev);
			pcie_hide_unused_ports(nb_dev);
		}
		break;
	default:
		printk(BIOS_DEBUG, "unknown dev: %s\n", dev_path(dev));
	}
}

#if !IS_ENABLED(CONFIG_AMD_SB_CIMX)
unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* FIXME
	 * Leave table blank until proper contents
	 * are determined.
	 */
	return current;
}
#endif

struct chip_operations southbridge_amd_rs780_ops = {
	CHIP_NAME("ATI RS780")
	.enable_dev = rs780_enable,
};
