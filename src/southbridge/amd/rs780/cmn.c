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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <delay.h>
#include <cpu/cpu.h>
#include "rs780.h"

static u32 nb_read_index(struct device *dev, u32 index_reg, u32 index)
{
	pci_write_config32(dev, index_reg, index);
	return pci_read_config32(dev, index_reg + 0x4);
}

static void nb_write_index(struct device *dev, u32 index_reg, u32 index, u32 data)
{
	pci_write_config32(dev, index_reg, index);
	pci_write_config32(dev, index_reg + 0x4, data);
}

/* extension registers */
u32 pci_ext_read_config32(struct device *nb_dev, struct device *dev, u32 reg)
{
	/* get BAR3 base address for nbcfg0x1c */
	u32 addr = pci_read_config32(nb_dev, 0x1c) & ~0xF;
	printk(BIOS_DEBUG, "addr=%x,bus=%x,devfn=%x\n", addr, dev->bus->secondary,
		     dev->path.pci.devfn);
	addr |= dev->bus->secondary << 20 |	/* bus num */
	    dev->path.pci.devfn << 12 | reg;
	return *((u32 *) addr);
}

void pci_ext_write_config32(struct device *nb_dev, struct device *dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;

	/* get BAR3 base address for nbcfg0x1c */
	u32 addr = pci_read_config32(nb_dev, 0x1c) & ~0xF;
	/*printk(BIOS_DEBUG, "write: addr=%x,bus=%x,devfn=%x\n", addr, dev->bus->secondary,
		     dev->path.pci.devfn);*/
	addr |= dev->bus->secondary << 20 |	/* bus num */
	    dev->path.pci.devfn << 12 | reg_pos;

	reg = reg_old = *((u32 *) addr);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		*((u32 *) addr) = reg;
	}
}

u32 nbmisc_read_index(struct device *nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMISC_INDEX, (index));
}

void nbmisc_write_index(struct device *nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMISC_INDEX, ((index) | 0x80), (data));
}

u32 nbpcie_p_read_index(struct device *dev, u32 index)
{
	return nb_read_index((dev), NBPCIE_INDEX, (index));
}

void nbpcie_p_write_index(struct device *dev, u32 index, u32 data)
{
	nb_write_index((dev), NBPCIE_INDEX, (index), (data));
}

u32 nbpcie_ind_read_index(struct device *nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBPCIE_INDEX, (index));
}

void nbpcie_ind_write_index(struct device *nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBPCIE_INDEX, (index), (data));
}

u32 htiu_read_index(struct device *nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBHTIU_INDEX, (index));
}

void htiu_write_index(struct device *nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBHTIU_INDEX, ((index) | 0x100), (data));
}

u32 nbmc_read_index(struct device *nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMC_INDEX, (index));
}

void nbmc_write_index(struct device *nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMC_INDEX, ((index) | 1 << 9), (data));
}

void set_nbcfg_enable_bits(struct device *nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(nb_dev, reg_pos, reg);
	}
}

void set_nbcfg_enable_bits_8(struct device *nb_dev, u32 reg_pos, u8 mask, u8 val)
{
	u8 reg_old, reg;
	reg = reg_old = pci_read_config8(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config8(nb_dev, reg_pos, reg);
	}
}

void set_nbmc_enable_bits(struct device *nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmc_write_index(nb_dev, reg_pos, reg);
	}
}

void set_htiu_enable_bits(struct device *nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = htiu_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		htiu_write_index(nb_dev, reg_pos, reg);
	}
}

void set_nbmisc_enable_bits(struct device *nb_dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmisc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmisc_write_index(nb_dev, reg_pos, reg);
	}
}

void set_pcie_enable_bits(struct device *dev, u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nb_read_index(dev, NBPCIE_INDEX, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nb_write_index(dev, NBPCIE_INDEX, reg_pos, reg);
	}
}

/*
 * To access bar3 we need to program PCI MMIO 7 in K8.
 * in_out:
 *	1: enable/enter k8 temp mmio base
 *	0: disable/restore
 */
void ProgK8TempMmioBase(u8 in_out, u32 pcie_base_add, u32 mmio_base_add)
{
	/* K8 Function1 is address map */
	struct device *k8_f1 = dev_find_slot(0, PCI_DEVFN(0x18, 1));
	struct device *k8_f0 = dev_find_slot(0, PCI_DEVFN(0x18, 0));

	if (in_out) {
		u32 dword, sblk;

		/* Get SBLink value (HyperTransport I/O Hub Link ID). */
		dword = pci_read_config32(k8_f0, 0x64);
		sblk = (dword >> 8) & 0x3;

		/* Fill MMIO limit/base pair. */
		pci_write_config32(k8_f1, 0xbc,
				   (((pcie_base_add + 0x10000000 -
				     1) >> 8) & 0xffffff00) | 0x80 | (sblk << 4));
		pci_write_config32(k8_f1, 0xb8, (pcie_base_add >> 8) | 0x3);
		pci_write_config32(k8_f1, 0xb4,
				   (((mmio_base_add + 0x10000000 -
				     1) >> 8) & 0xffffff00) | (sblk << 4));
		pci_write_config32(k8_f1, 0xb0, (mmio_base_add >> 8) | 0x3);
	} else {
		pci_write_config32(k8_f1, 0xb8, 0);
		pci_write_config32(k8_f1, 0xbc, 0);
		pci_write_config32(k8_f1, 0xb0, 0);
		pci_write_config32(k8_f1, 0xb4, 0);
	}
}

void PcieReleasePortTraining(struct device *nb_dev, struct device *dev, u32 port)
{
	switch (port) {
	case 2:		/* GFX, bit4-5 */
	case 3:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				       1 << (port + 2), 0 << (port + 2));
		break;
	case 4:		/* GPPSB, bit20-24 */
	case 5:
	case 6:
	case 7:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				       1 << (port + 17), 0 << (port + 17));
		break;
	case 9:		/* GPP, bit 4,5 of miscind 0x2D */
	case 10:
		set_nbmisc_enable_bits(nb_dev, 0x2D,
				      1 << (port - 5), 0 << (port - 5));
		break;
	}
}

/*
 * Output:
 *	0: no device is present.
 *	1: device is present and is trained.
 */
u8 PcieTrainPort(struct device *nb_dev, struct device *dev, u32 port)
{
	u16 count = 5000;
	u32 lc_state, reg, current_link_width, lane_mask;
	int8_t current, res = 0;
	u32 gfx_gpp_sb_sel;
	void set_pcie_dereset(void);
	void set_pcie_reset(void);

	switch (port) {
	case 2 ... 3:
		gfx_gpp_sb_sel = PCIE_CORE_INDEX_GFX;
		break;
	case 4 ... 7:
		gfx_gpp_sb_sel = PCIE_CORE_INDEX_GPPSB;
		break;
	case 9 ... 10:
		gfx_gpp_sb_sel = PCIE_CORE_INDEX_GPP;
		break;
	default:
		gfx_gpp_sb_sel = -1;
		return 0;
	}

	while (count--) {
		mdelay(40);
		udelay(200);
		lc_state = nbpcie_p_read_index(dev, 0xa5);	/* lc_state */
		printk(BIOS_DEBUG, "PcieLinkTraining port=%x:lc current state=%x\n",
			     port, lc_state);
		current = lc_state & 0x3f;	/* get LC_CURRENT_STATE, bit0-5 */

		switch (current) {
		case 0x00:	/* 0x00-0x04 means no device is present */
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
			res = 0;
			count = 0;
			break;
		case 0x06:
			/* read back current link width [6:4]. */
			current_link_width = (nbpcie_p_read_index(dev, 0xA2) >> 4) & 0x7;
			/* 4 means 7:4 and 15:12
			 * 3 means 7:2 and 15:10
			 * 2 means 7:1 and 15:9
			 * ignoring the reversal case
			 */
			lane_mask = (0xFF << (current_link_width - 2) * 2) & 0xFF;
			reg = nbpcie_ind_read_index(nb_dev, 0x65 | gfx_gpp_sb_sel);
			reg |= lane_mask << 8 | lane_mask;
			reg = 0xE0E0; /* TODO: See the comments in rs780_pcie.c, at about line 145. */
			nbpcie_ind_write_index(nb_dev, 0x65 | gfx_gpp_sb_sel, reg);
			printk(BIOS_DEBUG, "link_width=%x, lane_mask=%x",
				     current_link_width, lane_mask);
			set_pcie_reset();
			mdelay(1);
			set_pcie_dereset();
			break;
		case 0x07:	/* device is in compliance state (training sequence is done). Move to train the next device */
			res = 0;
			count = 0;
			break;
		case 0x10:
			reg = pci_ext_read_config32(nb_dev, dev, PCIE_VC0_RESOURCE_STATUS);
			printk(BIOS_DEBUG, "PcieTrainPort reg=0x%x\n", reg);
			/* check bit1 */
			if (reg & VC_NEGOTIATION_PENDING) {	/* bit1=1 means the link needs to be re-trained. */
				/* set bit8=1, bit0-2=bit4-6 */
				u32 tmp;
				reg = nbpcie_p_read_index(dev, PCIE_LC_LINK_WIDTH);
				tmp = (reg >> 4) & 0x07;	/* get bit4-6 */
				reg &= 0xfff8;	/* clear bit0-2 */
				reg += tmp;	/* merge */
				reg |= 1 << 8;
				nbpcie_p_write_index(dev, PCIE_LC_LINK_WIDTH, reg);
				count++;	/* CIM said "keep in loop"?  */
			} else {
				res = 1;
				count = 0;
			}
			break;
		default:	/* reset pcie */
			res = 0;
			count = 0;	/* break loop */
			break;
		}
	}
	return res;
}

/*
 * Compliant with CIM_33's ATINB_SetToms.
 * Set Top Of Memory below and above 4G.
 */
void rs780_set_tom(struct device *nb_dev)
{
	/* set TOM */
#if IS_ENABLED(CONFIG_GFXUMA)
	pci_write_config32(nb_dev, 0x90, uma_memory_base);
	//nbmc_write_index(nb_dev, 0x1e, uma_memory_base);
#else
	/* 1GB  system memory supposed */
	pci_write_config32(nb_dev, 0x90, 0x38000000);
	//nbmc_write_index(nb_dev, 0x1e, 0x38000000);
#endif
}

// extract single bit
u32 extractbit(u32 data, int bit_number)
{
	return (data >> bit_number) & 1;
}

// extract bit field
u32 extractbits(u32 source, int lsb, int msb)
{
	int field_width = msb - lsb + 1;
	u32 mask = 0xFFFFFFFF >> (32 - field_width);
	return (source >> lsb) & mask;
}

// return AMD cpuid family
int cpuidFamily(void)
{
	u32 baseFamily, extendedFamily, fms;

	fms = cpuid_eax (1);
	baseFamily = extractbits (fms, 8, 11);
	extendedFamily = extractbits (fms, 20, 27);
	return baseFamily + extendedFamily;
}


// return non-zero for AMD family 0Fh processor found
int is_family0Fh(void)
{
	return cpuidFamily() == 0x0F;
}


// return non-zero for AMD family 10h processor found
int is_family10h(void)
{
	return cpuidFamily() == 0x10;
}
