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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
#include "sr5650.h"
#include "cmn.h"

/*
 * extern function declaration
 */
extern void set_pcie_dereset(void);
extern void set_pcie_reset(void);

/* extension registers */
u32 pci_ext_read_config32(device_t nb_dev, device_t dev, u32 reg)
{
	/*get BAR3 base address for nbcfg0x1c */
	u32 addr = pci_read_config32(nb_dev, 0x1c) & ~0xF;
	printk(BIOS_DEBUG, "addr=%x,bus=%x,devfn=%x\n", addr, dev->bus->secondary,
		     dev->path.pci.devfn);
	addr |= dev->bus->secondary << 20 |	/* bus num */
	    dev->path.pci.devfn << 12 | reg;
	return *((u32 *) addr);
}

void pci_ext_write_config32(device_t nb_dev, device_t dev, u32 reg_pos, u32 mask, u32 val)
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

u32 nbpcie_p_read_index(device_t dev, u32 index)
{
	return nb_read_index((dev), NBPCIE_INDEX, (index));
}

void nbpcie_p_write_index(device_t dev, u32 index, u32 data)
{
	nb_write_index((dev), NBPCIE_INDEX, (index), (data));
}

u32 nbpcie_ind_read_index(device_t nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBPCIE_INDEX, (index));
}

void nbpcie_ind_write_index(device_t nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBPCIE_INDEX, (index), (data));
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
	device_t k8_f1;
	device_t np = dev_find_slot(0, PCI_DEVFN(0x19, 1));
	u16 node;

	for (node = 0; node < CONFIG_MAX_PHYSICAL_CPUS; node++) {
		k8_f1 = dev_find_slot(0, PCI_DEVFN(0x18 + node, 1));
		if (!k8_f1) {
			break;
		}

		if (in_out) {
			/* Fill MMIO limit/base pair. */
			pci_write_config32(k8_f1, 0xbc,
					   (((pcie_base_add + 0x10000000 -
					      1) >> 8) & 0xffffff00) | 0x8 | (np ? 2 << 4 : 0 << 4));
			pci_write_config32(k8_f1, 0xb8, (pcie_base_add >> 8) | 0x3);
			pci_write_config32(k8_f1, 0xb4,
					   ((mmio_base_add + 0x10000000 -
					     1) >> 8) | (np ? 2 << 4 : 0 << 4));
			pci_write_config32(k8_f1, 0xb0, (mmio_base_add >> 8) | 0x3);
		} else {
			pci_write_config32(k8_f1, 0xb8, 0);
			pci_write_config32(k8_f1, 0xbc, 0);
			pci_write_config32(k8_f1, 0xb0, 0);
			pci_write_config32(k8_f1, 0xb4, 0);
		}
	}
}

void PcieReleasePortTraining(device_t nb_dev, device_t dev, u32 port)
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
u8 PcieTrainPort(device_t nb_dev, device_t dev, u32 port)
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
				reg =
				    nbpcie_p_read_index(dev,
							PCIE_LC_LINK_WIDTH);
				tmp = (reg >> 4) && 0x3;	/* get bit4-6 */
				reg &= 0xfff8;	/* clear bit0-2 */
				reg += tmp;	/* merge */
				reg |= 1 << 8;
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
void sr5650_set_tom(device_t nb_dev)
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

u32 get_vid_did(device_t dev)
{
	return pci_read_config32(dev, 0);
}

void sr5650_nb_pci_table(device_t nb_dev)
{	/* NBPOR_InitPOR function. */
	u8 temp8;
	u16 temp16;
	u32 temp32;

	/* Program NB PCI table. */
	temp16 = pci_read_config16(nb_dev, 0x04);
	printk(BIOS_DEBUG, "NB_PCI_REG04 = %x.\n", temp16);
	temp32 = pci_read_config32(nb_dev, 0x84);
	printk(BIOS_DEBUG, "NB_PCI_REG84 = %x.\n", temp32);
	//Reg4Ch[1]=1 (APIC_ENABLE) force cpu request with address 0xFECx_xxxx to south-bridge
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
* case 0 will be called twice, one is by cpu in hypertransport.c line458,
* the other is by sr5650.
***********************************************/
void sr5650_enable(device_t dev)
{
	device_t nb_dev = 0, sb_dev = 0;
	int dev_ind;

	printk(BIOS_INFO, "sr5650_enable: dev=%p, VID_DID=0x%x\n", dev, get_vid_did(dev));
	nb_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (!nb_dev) {
		die("sr5650_enable: CAN NOT FIND SR5650 DEVICE, HALT!\n");
		/* NOT REACHED */
	}

	/* sb_dev (dev 8) is a bridge that links to southbridge. */
	sb_dev = dev_find_slot(0, PCI_DEVFN(8, 0));
	if (!sb_dev) {
		die("sr5650_enable: CAN NOT FIND SB bridge, HALT!\n");
		/* NOT REACHED */
	}

	dev_ind = dev->path.pci.devfn >> 3;
	switch (dev_ind) {
	case 0:		/* bus0, dev0, fun0; */
		printk(BIOS_INFO, "Bus-0, Dev-0, Fun-0.\n");
		enable_pcie_bar3(nb_dev);	/* PCIEMiscInit */

		config_gpp_core(nb_dev, sb_dev);
		sr5650_gpp_sb_init(nb_dev, sb_dev, 8);

		sr5650_nb_pci_table(nb_dev);
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
		/* Dont call disable_pcie_bar3(nb_dev) here, otherwise the screen will crash. */
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
	}
}

struct chip_operations southbridge_amd_sr5650_ops = {
	CHIP_NAME("ATI SR5650")
	.enable_dev = sr5650_enable,
};
