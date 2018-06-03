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
#include <arch/io.h>
#include <arch/acpi_ivrs.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <stdlib.h>
#include <delay.h>
#include <option.h>
#include "sr5650.h"
#include "cmn.h"

/*
 * extern function declaration
 */
struct resource * sr5650_retrieve_cpu_mmio_resource() {
	struct device *domain;
	struct resource *res;

	for (domain = all_devices; domain; domain = domain->next) {
		if (domain->bus->dev->path.type != DEVICE_PATH_DOMAIN)
			continue;
		res = probe_resource(domain->bus->dev, 0xc0010058);
		if (res)
			return res;
	}

	return NULL;
}

/* extension registers */
u32 pci_ext_read_config32(struct device *nb_dev, struct device *dev, u32 reg)
{
	/*get BAR3 base address for nbcfg0x1c */
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

	/*get BAR3 base address for nbcfg0x1c */
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

uint32_t l2cfg_ind_read_index(struct device *nb_dev, uint32_t index)
{
	return nb_read_index((nb_dev), L2CFG_INDEX, (index));
}

void l2cfg_ind_write_index(struct device *nb_dev, uint32_t index, uint32_t data)
{
	nb_write_index((nb_dev), L2CFG_INDEX | (0x1 << 8), (index), (data));
}

uint32_t l1cfg_ind_read_index(struct device *nb_dev, uint32_t index)
{
	return nb_read_index((nb_dev), L1CFG_INDEX, (index));
}

void l1cfg_ind_write_index(struct device *nb_dev, uint32_t index, uint32_t data)
{
	nb_write_index((nb_dev), L1CFG_INDEX | (0x1 << 31), (index), (data));
}

/***********************************************************
* To access bar3 we need to program PCI MMIO 7 in K8.
* in_out:
*	1: enable/enter k8 temp mmio base
*	0: disable/restore
***********************************************************/
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
	case 2:		/* GPP1, bit4-5 */
	case 3:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				       1 << (port + 2), 0 << (port + 2));
		break;
	case 4:		/* GPP3a, bit20-24 */
	case 5:
	case 6:
	case 7:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				       1 << (port + 17), 0 << (port + 17));
		break;
	case 9:		/* GPP3a, bit25,26 */
	case 10:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				      1 << (port + 16), 0 << (port + 16));
		break;
	case 11:	/* GPP2, bit6-7 */
	case 12:
		set_nbmisc_enable_bits(nb_dev, PCIE_LINK_CFG,
				       1 << (port - 5), 0 << (port - 5));
		break;
	case 13:	/* GPP3b, bit4 of NBMISCIND:0x2A */
		set_nbmisc_enable_bits(nb_dev, 0x2A,
				       1 << 4, 0 << 4);
		break;
	}
}

/********************************************************************************************************
* Output:
*	0: no device is present.
*	1: device is present and is trained.
********************************************************************************************************/
u8 PcieTrainPort(struct device *nb_dev, struct device *dev, u32 port)
{
	u16 count = 5000;
	u32 lc_state, reg, current_link_width, lane_mask;
	u8 current, res = 0;
	u32 gpp_sb_sel = 0;

	switch (port) {
	case 2:
	case 3:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP1;
		break;
	case 4 ... 7:
	case 9:
	case 10:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP3a;
		break;
	case 11:
	case 12:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP2;
		break;
	case 13:
		gpp_sb_sel = PCIE_CORE_INDEX_GPP3b;
		break;
	}

	while (count--) {
		udelay(40200);
		lc_state = nbpcie_p_read_index(dev, 0xa5);	/* lc_state */
		printk(BIOS_DEBUG, "PcieLinkTraining port=%x:lc current state=%x\n",
			     port, lc_state);
		current = lc_state & 0x3f;	/* get LC_CURRENT_STATE, bit0-5 */

		switch (current) {
			/* 0x00-0x04 means no device is present */
		case 0x06:
			/* read back current link width [6:4]. */
			current_link_width = (nbpcie_p_read_index(dev, 0xA2) >> 4) & 0x7;
			/* 4 means 7:4 and 15:12
			 * 3 means 7:2 and 15:10
			 * 2 means 7:1 and 15:9
			 * ignoring the reversal case
			 */
			lane_mask = (0xFF << (current_link_width - 2) * 2) & 0xFF;
			reg = nbpcie_ind_read_index(nb_dev, 0x65 | gpp_sb_sel);
			reg |= lane_mask << 8 | lane_mask;
			/* NOTE: See the comments in rs780_pcie.c
			 * switching_gppsb_configurations
			 * In CIMx 4.5.0 and RPR, 4c is done before 5 & 6.
			 * But in this way, a x4 device in port B (dev 4) of
			 * Configuration B can only be detected as x1, instead
			 * of x4. When the port B is being trained, the
			 * LC_CURRENT_STATE is 6 and the LC_LINK_WIDTH_RD is 1.
			 * We have to set the PCIEIND:0x65 as 0xE0E0 and reset
			 * the slot. Then the card seems to work in x1 mode.
			 */
			reg = 0xE0E0; /*I think that the lane_mask calc above is wrong, and this can't be hardcoded because the configuration changes.*/
			nbpcie_ind_write_index(nb_dev, 0x65 | gpp_sb_sel, reg);
			printk(BIOS_DEBUG, "link_width=%x, lane_mask=%x",
				     current_link_width, lane_mask);
			set_pcie_reset();
			mdelay(1);
			set_pcie_dereset();
			break;
		case 0x07:	/* device is in compliance state (training sequence is done). Move to train the next device */
			res = 1;
			count = 0;
			break;
		case 0x10:
			reg =
			    pci_ext_read_config32(nb_dev, dev,
						  PCIE_VC0_RESOURCE_STATUS);
			printk(BIOS_DEBUG, "PcieTrainPort reg=0x%x\n", reg);
			/* check bit1 */
			if (reg & VC_NEGOTIATION_PENDING) {	/* bit1=1 means the link needs to be re-trained. */
				/* set bit8=1, bit0-2=bit4-6 */
				u32 tmp;
				reg = nbpcie_p_read_index(dev, PCIE_LC_LINK_WIDTH);
				tmp = (reg >> 4) & 0x7;	/* get bit4-6 */
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
		default:
			/* CIMx Unknown Workaround - There is a device that won't train. Try to reset it. */
			/* if there are no device resets and nothing works, CIMx does a cf9 system reset (yikes!) */
			set_pcie_reset();
			mdelay(1);
			set_pcie_dereset();
			res = 0;
			count = 0;	/* break loop */
			break;
		}
	}
	return res;
}

/*
 * Set Top Of Memory below and above 4G.
 */
void sr5650_set_tom(struct device *nb_dev)
{
	msr_t sysmem;

	/* The system top memory in SR56X0. */
	sysmem = rdmsr(0xc001001A);
	printk(BIOS_DEBUG, "Sysmem TOM = %x_%x\n", sysmem.hi, sysmem.lo);
	pci_write_config32(nb_dev, 0x90, sysmem.lo);

	sysmem = rdmsr(0xc001001D);
	printk(BIOS_DEBUG, "Sysmem TOM2 = %x_%x\n", sysmem.hi, sysmem.lo);
	htiu_write_index(nb_dev, 0x31, sysmem.hi);
	htiu_write_index(nb_dev, 0x30, sysmem.lo | 1);
}

u32 get_vid_did(struct device *dev)
{
	return pci_read_config32(dev, 0);
}

void detect_and_enable_iommu(struct device *iommu_dev) {
	uint32_t dword;
	uint8_t l1_target;
	unsigned char iommu;
	void * mmio_base;

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		printk(BIOS_DEBUG, "Initializing IOMMU\n");

		struct device *nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));

		if (!nb_dev) {
			printk(BIOS_WARNING, "Unable to find SR5690 device!  IOMMU NOT initialized\n");
			return;
		}

		mmio_base = (void*)(pci_read_config32(iommu_dev, 0x44) & 0xffffc000);

		// if (get_nb_rev(nb_dev) == REV_SR5650_A11) {
		//	dword = pci_read_config32(iommu_dev, 0x6c);
		//	dword &= ~(0x1 << 8);
		//	pci_write_config32(iommu_dev, 0x6c, dword);
		// }

		dword = pci_read_config32(iommu_dev, 0x50);
		dword &= ~(0x1 << 22);
		pci_write_config32(iommu_dev, 0x50, dword);

		dword = pci_read_config32(iommu_dev, 0x44);
		dword |= 0x1;
		pci_write_config32(iommu_dev, 0x44, dword);

		write32((void*)(mmio_base + 0x8), 0x0);
		write32((void*)(mmio_base + 0xc), 0x08000000);
		write32((void*)(mmio_base + 0x10), 0x0);
		write32((void*)(mmio_base + 0x2008), 0x0);
		write32((void*)(mmio_base + 0x2010), 0x0);

		/* IOMMU L1 initialization */
		for (l1_target = 0; l1_target < 6; l1_target++) {
			dword = l1cfg_ind_read_index(nb_dev, (l1_target << 16) + 0xc);
			dword |= (0x7 << 28);
			l1cfg_ind_write_index(nb_dev, (l1_target << 16) + 0xc, dword);

			dword = l1cfg_ind_read_index(nb_dev, (l1_target << 16) + 0x7);
			dword |= (0x1 << 5);
			l1cfg_ind_write_index(nb_dev, (l1_target << 16) + 0x7, dword);
		}

		/* IOMMU L2 initialization */
		dword = l2cfg_ind_read_index(nb_dev, 0xc);
		dword |= (0x7 << 29);
		l2cfg_ind_write_index(nb_dev, 0xc, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x10);
		dword &= ~(0x3 << 8);
		dword |= (0x2 << 8);
		l2cfg_ind_write_index(nb_dev, 0x10, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x14);
		dword &= ~(0x3 << 8);
		dword |= (0x2 << 8);
		l2cfg_ind_write_index(nb_dev, 0x14, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x18);
		dword &= ~(0x3 << 8);
		dword |= (0x2 << 8);
		l2cfg_ind_write_index(nb_dev, 0x18, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x1c);
		dword &= ~(0x3 << 8);
		dword |= (0x2 << 8);
		l2cfg_ind_write_index(nb_dev, 0x1c, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x50);
		dword &= ~(0x3 << 8);
		dword |= (0x2 << 8);
		l2cfg_ind_write_index(nb_dev, 0x50, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x10);
		dword |= (0x1 << 4);
		l2cfg_ind_write_index(nb_dev, 0x10, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x14);
		dword |= (0x1 << 4);
		l2cfg_ind_write_index(nb_dev, 0x14, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x18);
		dword |= (0x1 << 4);
		l2cfg_ind_write_index(nb_dev, 0x18, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x1c);
		dword |= (0x1 << 4);
		l2cfg_ind_write_index(nb_dev, 0x1c, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x50);
		dword |= (0x1 << 4);
		l2cfg_ind_write_index(nb_dev, 0x50, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x6);
		dword |= (0x1 << 7);
		l2cfg_ind_write_index(nb_dev, 0x6, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x44);
		dword |= (0x1 << 0);
		l2cfg_ind_write_index(nb_dev, 0x44, dword);

// 		if (get_nb_rev(nb_dev) == REV_SR5650_A21) {
			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 1);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x44);
			dword |= (0x1 << 1);
			l2cfg_ind_write_index(nb_dev, 0x44, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 2);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 3);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x44);
			dword |= (0x1 << 3);
			l2cfg_ind_write_index(nb_dev, 0x44, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 4);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x6);
			dword |= (0x1 << 5);
			l2cfg_ind_write_index(nb_dev, 0x6, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x6);
			dword |= (0x1 << 6);
			l2cfg_ind_write_index(nb_dev, 0x6, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 5);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x44);
			dword |= (0x1 << 4);
			l2cfg_ind_write_index(nb_dev, 0x44, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 6);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x7);
			dword |= (0x1 << 7);
			l2cfg_ind_write_index(nb_dev, 0x7, dword);

			dword = l2cfg_ind_read_index(nb_dev, 0x6);
			dword |= (0x1 << 8);
			l2cfg_ind_write_index(nb_dev, 0x6, dword);
// 		}

		l2cfg_ind_write_index(nb_dev, 0x52, 0xf0000002);

		dword = l2cfg_ind_read_index(nb_dev, 0x80);
		dword |= (0x1 << 0);
		l2cfg_ind_write_index(nb_dev, 0x80, dword);

		dword = l2cfg_ind_read_index(nb_dev, 0x30);
		dword |= (0x1 << 0);
		l2cfg_ind_write_index(nb_dev, 0x30, dword);
	}
}

void sr5650_iommu_read_resources(struct device *dev)
{
	unsigned char iommu;
	struct resource *res;

	iommu = 1;
	get_option(&iommu, "iommu");

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	if (iommu) {
		/* Request MMIO range allocation */
		res = new_resource(dev, 0x44);		/* IOMMU */
		res->base = 0x0;
		res->size = 0x4000;
		res->limit = 0xFFFFFFFFUL;		/* res->base + res->size -1; */
		res->align = 14;			/* 16k alignment */
		res->gran = 14;
		res->flags = IORESOURCE_MEM | IORESOURCE_RESERVE;
	}

	compact_resources(dev);
}

void sr5650_iommu_set_resources(struct device *dev)
{
	unsigned char iommu;
	struct resource *res;

	iommu = 1;
	get_option(&iommu, "iommu");

	/* Get the normal pci resources of this device */
	pci_dev_read_resources(dev);

	if (iommu) {
		/* Get the allocated range */
		res = find_resource(dev, 0x44);

		if (res->base == 0) {
			printk(BIOS_WARNING, "Unable to allocate MMIO range to IOMMU\n");
		}

		/* Assign the range to hardware */
		pci_write_config32(dev, 0x44, res->base & 0xffffc000);
		pci_write_config32(dev, 0x48, 0x0);
	}

	/* Run standard resource set routine */
	pci_dev_set_resources(dev);
}

void sr5650_iommu_enable_resources(struct device *dev)
{
	detect_and_enable_iommu(dev);
}

void sr5650_nb_pci_table(struct device *nb_dev)
{	/* NBPOR_InitPOR function. */
	u8 temp8;
	u16 temp16;
	u32 temp32;

	/* Program NB PCI table. */
	temp16 = pci_read_config16(nb_dev, 0x04);
	printk(BIOS_DEBUG, "NB_PCI_REG04 = %x.\n", temp16);
	temp32 = pci_read_config32(nb_dev, 0x84);
	printk(BIOS_DEBUG, "NB_PCI_REG84 = %x.\n", temp32);
	//Reg4Ch[1]=1 (APIC_ENABLE) force CPU request with address 0xFECx_xxxx to south-bridge
	//Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
	pci_write_config8(nb_dev, 0x4c, 0x42);
	temp8 = pci_read_config8(nb_dev, 0x4e);
	temp8 |= 0x05; /* BAR1_ENABLE */
	pci_write_config8(nb_dev, 0x4e, temp8);

	temp32 = pci_read_config32(nb_dev, 0x4c);
	printk(BIOS_DEBUG, "NB_PCI_REG4C = %x.\n", temp32);

	/* disable GFX debug. */
	temp8 = pci_read_config8(nb_dev, 0x8d);
	temp8 &= ~(1<<1);
	pci_write_config8(nb_dev, 0x8d, temp8);

	/* The system top memory in SR56X0. */
	sr5650_set_tom(nb_dev);

	/* Program NB HTIU table. */
	//set_htiu_enable_bits(nb_dev, 0x05, 1<<10 | 1<<9, 1<<10|1<<9);
	set_htiu_enable_bits(nb_dev, 0x06, 1, 0x4203a202);
	//set_htiu_enable_bits(nb_dev, 0x07, 1<<1 | 1<<2, 0x8001);
	set_htiu_enable_bits(nb_dev, 0x15, 0, 1<<31 | 1<<30 | 1<<27);
	set_htiu_enable_bits(nb_dev, 0x1c, 0, 0xfffe0000);
	set_htiu_enable_bits(nb_dev, 0x0c, 0x3f, 1 | 1<<3);
	set_htiu_enable_bits(nb_dev, 0x19, 0xfffff+(1<<31), 0x186a0+(1<<31));
	set_htiu_enable_bits(nb_dev, 0x16, 0x3f<<10, 0x7<<10);
	set_htiu_enable_bits(nb_dev, 0x23, 0, 1<<28);
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
* the other is by sr5650.
***********************************************/
void sr5650_enable(struct device *dev)
{
	struct device *nb_dev = NULL, *sb_dev = NULL;
	int dev_ind;
	struct southbridge_amd_sr5650_config *cfg;

	printk(BIOS_INFO, "sr5650_enable: dev=%p, VID_DID=0x%x\n", dev, get_vid_did(dev));
	nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		die("sr5650_enable: CAN NOT FIND SR5650 DEVICE, HALT!\n");
		/* NOT REACHED */
	}
	cfg = (struct southbridge_amd_sr5650_config *)nb_dev->chip_info;

	/* sb_dev (dev 8) is a bridge that links to southbridge. */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	if (!sb_dev) {
		die("sr5650_enable: CAN NOT FIND SB bridge, HALT!\n");
		/* NOT REACHED */
	}

	dev_ind = dev->path.pci.devfn >> 3;
	switch (dev_ind) {
	case 0:		/* bus0, dev0, fun0; */
		switch (dev->path.pci.devfn & 0x7) {
			case 0:
				printk(BIOS_INFO, "Bus-0, Dev-0, Fun-0.\n");
				enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */

				config_gpp_core(nb_dev, sb_dev);
				sr5650_gpp_sb_init(nb_dev, sb_dev, 8);

				sr5650_nb_pci_table(nb_dev);
				break;
			case 1:
				printk(BIOS_INFO, "Bus-0, Dev-0, Fun-1.\n");
				break;
			case 2:
				printk(BIOS_INFO, "Bus-0, Dev-0, Fun-2.\n");
				break;
		}
		break;

	case 2:		/* bus0, dev2,3 GPP1 */
	case 3:
		printk(BIOS_INFO, "Bus-0, Dev-2,3, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind); /* Note, dev 2,3 are generic PCIe ports. */
		break;
	case 4:		/* bus0, dev4-7, four GPP3a */
	case 5:
	case 6:
	case 7:
		enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */
		printk(BIOS_INFO, "Bus-0, Dev-4,5,6,7, Fun-0. enable=%d\n",
			    dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << dev_ind,
				       (dev->enabled ? 0 : 1) << dev_ind);
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	case 8:		/* bus0, dev8, SB */
		printk(BIOS_INFO, "Bus-0, Dev-8, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 6,
				       (dev->enabled ? 1 : 0) << 6);
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind);
		disable_pcie_bar3(nb_dev);
		break;
	case 9:		/* bus 0, dev 9,10, GPP3a */
	case 10:
		printk(BIOS_INFO, "Bus-0, Dev-9, 10, Fun-0. enable=%d\n",
			    dev->enabled);
		enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << (7 + dev_ind),
				       (dev->enabled ? 0 : 1) << (7 + dev_ind));
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind);
		/* Don't call disable_pcie_bar3(nb_dev) here, otherwise the screen will crash. */
		break;
	case 11:
	case 12:	/* bus 0, dev 11,12, GPP2 */
		printk(BIOS_INFO, "Bus-0, Dev-11,12, Fun-0. enable=%d\n", dev->enabled);
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << (7 + dev_ind),
				       (dev->enabled ? 0 : 1) << (7 + dev_ind));
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	case 13:	/* bus 0, dev 12, GPP3b */
		set_nbmisc_enable_bits(nb_dev, 0x0c, 1 << (7 + dev_ind),
				       (dev->enabled ? 0 : 1) << (7 + dev_ind));
		if (dev->enabled)
			sr5650_gpp_sb_init(nb_dev, dev, dev_ind);
		break;
	default:
		printk(BIOS_DEBUG, "unknown dev: %s\n", dev_path(dev));
	}

	/* Lock HWInit Register after the last device was done */
	if (dev_ind == 13) {
		sr56x0_lock_hwinitreg();
		udelay(cfg->pcie_settling_time);
	}
}

static void add_ivrs_device_entries(struct device *parent, struct device *dev,
		int depth, int linknum, int8_t *root_level,
		unsigned long *current, uint16_t *length)
{
	uint8_t *p = (uint8_t *) *current;

	struct device *sibling;
	struct bus *link;

	if (!root_level) {
		root_level = malloc(sizeof(int8_t));
		if (root_level == NULL)
			die("Error: Could not allocate a byte!\n");
		*root_level = -1;
	}

	if ((dev->path.type == DEVICE_PATH_PCI) &&
		(dev->bus->secondary == 0x0) && (dev->path.pci.devfn == 0x0))
		*root_level = depth;

	if ((dev->path.type == DEVICE_PATH_PCI) && (*root_level != -1) &&
		(depth >= *root_level) && (dev->enabled)) {

		*p = 0;
		if (depth == *root_level) {
			if (dev->path.pci.devfn < (0x1 << 3)) {
				/* SR5690 control device */
			} else if ((dev->path.pci.devfn >= (0x1 << 3)) &&
					(dev->path.pci.devfn < (0xe << 3))) {
				/* SR5690 PCIe bridge device */
			} else if (dev->path.pci.devfn == (0x14 << 3)) {
				/* SMBUS controller */
				p[0] = IVHD_DEV_4_BYTE_SELECT;	/* Entry type */
				p[1] = dev->path.pci.devfn;	/* Device */
				p[2] = dev->bus->secondary;	/* Bus */
				p[3] =  IVHD_DTE_LINT_1_PASS |	/* Data */
					IVHD_DTE_SYS_MGT_NO_TRANS |
					IVHD_DTE_NMI_PASS |
					IVHD_DTE_EXT_INT_PASS |
					IVHD_DTE_INIT_PASS;
			} else {
				/* Other southbridge device */
				p[0] = IVHD_DEV_4_BYTE_SELECT;	/* Entry type */
				p[1] = dev->path.pci.devfn;	/* Device */
				p[2] = dev->bus->secondary;	/* Bus */
				p[3] = 0x0;			/* Data */
			}
		} else if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_NORMAL) {
			/* Device behind bridge */
			if (pci_find_capability(dev, PCI_CAP_ID_PCIE)) {
				/* Device is PCIe */
				p[0] = IVHD_DEV_4_BYTE_SELECT;	/* Entry type */
				p[1] = dev->path.pci.devfn;	/* Device */
				p[2] = dev->bus->secondary;	/* Bus */
				p[3] = 0x0;			/* Data */
			} else {
				/* Device is legacy PCI or PCI-X */
				p[0] = IVHD_DEV_8_BYTE_ALIAS_SELECT; /* Entry */
				p[1] = dev->path.pci.devfn;	/* Device */
				p[2] = dev->bus->secondary;	/* Bus */
				p[3] = 0x0;			/* Data */
				p[4] = 0x0;			/* Reserved */
				p[5] = parent->path.pci.devfn;	/* Device */
				p[6] = parent->bus->secondary;	/* Bus */
				p[7] = 0x0;			/* Reserved */
			}
		}

		if (*p == IVHD_DEV_4_BYTE_SELECT) {
			*length += 4;
			*current += 4;
		} else if (*p == IVHD_DEV_8_BYTE_ALIAS_SELECT) {
			*length += 8;
			*current += 8;
		}
	}

	for (link = dev->link_list; link; link = link->next)
		for (sibling = link->children; sibling;
			sibling = sibling->sibling)
			add_ivrs_device_entries(dev, sibling, depth + 1,
				depth, root_level, current, length);

	if (depth == 0)
		free(root_level);
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	struct resource *res;
	resource_t mmconf_base = EXT_CONF_BASE_ADDRESS;

	if (IS_ENABLED(CONFIG_EXT_CONF_SUPPORT)) {
		res = sr5650_retrieve_cpu_mmio_resource();
		if (res)
			mmconf_base = res->base;

		current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current, mmconf_base, 0x0, 0x0, 0x1f);
	}

	return current;
}

static unsigned long acpi_fill_ivrs(acpi_ivrs_t* ivrs, unsigned long current)
{
	uint8_t *p;

	struct device *nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		printk(BIOS_WARNING, "acpi_fill_ivrs: Unable to locate SR5650 "
				"device!  IVRS table not generated...\n");
		return (unsigned long)ivrs;
	}

	struct device *iommu_dev = dev_find_slot(0, PCI_DEVFN(0, 2));
	if (!iommu_dev) {
		printk(BIOS_WARNING, "acpi_fill_ivrs: Unable to locate SR5650 "
				"IOMMU device!  IVRS table not generated...\n");
		return (unsigned long)ivrs;
	}

	ivrs->iv_info = IVINFO_VA_SIZE_64_BITS | IVINFO_PA_SIZE_52_BITS;

	ivrs->ivhd.type = IVHD_BLOCK_TYPE_LEGACY__FIXED;
	ivrs->ivhd.flags = IVHD_FLAG_ISOC |
			   IVHD_FLAG_RES_PASS_PW |
			   IVHD_FLAG_PASS_PW |
			   IVHD_FLAG_IOTLB_SUP;

	ivrs->ivhd.length = sizeof(struct acpi_ivrs_ivhd);

	/* BDF <bus>:00.2 */
	ivrs->ivhd.device_id = 0x2 | (nb_dev->bus->secondary << 8);

	/* Capability block 0x40 (type 0xf, "Secure device") */
	ivrs->ivhd.capability_offset = 0x40;
	ivrs->ivhd.iommu_base_low = pci_read_config32(iommu_dev, 0x44) &
			0xffffc000;
	ivrs->ivhd.iommu_base_high = pci_read_config32(iommu_dev, 0x48);
	ivrs->ivhd.pci_segment_group = 0x0;
	ivrs->ivhd.iommu_info = 0x0;
	ivrs->ivhd.iommu_info |= (0x14 << IOMMU_INFO_UNIT_ID_SHIFT);
	ivrs->ivhd.iommu_feature_info = 0x0;

	/* Describe HPET */
	p = (uint8_t *)current;
	p[0] = IVHD_DEV_8_BYTE_EXT_SPECIAL_DEV;	/* Entry type */
	p[1] = 0;				/* Device */
	p[2] = 0;				/* Bus */
	p[3] = IVHD_DTE_LINT_1_PASS |		/* DTE */
	       IVHD_DTE_LINT_0_PASS |
	       IVHD_DTE_SYS_MGT_INTX_NO_TRANS |
	       IVHD_DTE_NMI_PASS |
	       IVHD_DTE_EXT_INT_PASS |
	       IVHD_DTE_INIT_PASS;
	p[4] = 0x0;				/* HPET number */
	p[5] = 0x14 << 3;			/* HPET device */
	p[6] = nb_dev->bus->secondary;		/* HPET bus */
	p[7] = IVHD_SPECIAL_DEV_HPET;		/* Variety */
	ivrs->ivhd.length += 8;
	current += 8;

	/* Describe PCI devices */
	add_ivrs_device_entries(NULL, all_devices, 0, -1, NULL, &current,
			&ivrs->ivhd.length);

	/* Describe IOAPICs */
	unsigned long prev_current = current;
	current = acpi_fill_ivrs_ioapic(ivrs, current);
	ivrs->ivhd.length += (current - prev_current);

	return current;
}

unsigned long southbridge_write_acpi_tables(struct device *device,
						unsigned long current,
						struct acpi_rsdp *rsdp)
{
	unsigned char iommu;

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		acpi_ivrs_t *ivrs;

		/* IVRS */
		current = ALIGN(current, 8);
		printk(BIOS_DEBUG, "ACPI:   * IVRS at %lx\n", current);
		ivrs = (acpi_ivrs_t *) current;
		acpi_create_ivrs(ivrs, acpi_fill_ivrs);
		current += ivrs->header.length;
		acpi_add_table(rsdp, ivrs);
	}

	return current;
}

static struct pci_operations iommu_ops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

static struct device_operations iommu_ops = {
	.read_resources = sr5650_iommu_read_resources,
	.set_resources = sr5650_iommu_set_resources,
	.enable_resources = sr5650_iommu_enable_resources,
	.write_acpi_tables = southbridge_write_acpi_tables,
	.init = 0,
	.scan_bus = 0,
	.ops_pci = &iommu_ops_pci,
};

static const struct pci_driver ht_driver_sr5690 __pci_driver = {
	.ops = &iommu_ops,
	.vendor = PCI_VENDOR_ID_ATI,
	.device = PCI_DEVICE_ID_AMD_SR5650_IOMMU,
};

struct chip_operations southbridge_amd_sr5650_ops = {
	CHIP_NAME("ATI SR5650")
	.enable_dev = sr5650_enable,
};
