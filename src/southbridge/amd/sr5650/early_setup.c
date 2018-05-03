/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

#include <stdint.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <option.h>
#include <reset.h>
#include "sr5650.h"
#include "cmn.h"

/* space = 0: AX_INDXC, AX_DATAC
 * space = 1: AX_INDXP, AX_DATAP
 */
static void alink_ax_indx(u32 space, u32 axindc, u32 mask, u32 val)
{
	u32 tmp;

	/* read axindc to tmp */
	outl(space << 30 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(space << 30 | space << 3 | 0x34, AB_INDX);
	tmp = inl(AB_DATA);

	tmp &= ~mask;
	tmp |= val;

	/* write tmp */
	outl(space << 30 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(space << 30 | space << 3 | 0x34, AB_INDX);
	outl(tmp, AB_DATA);
}


static void set_fam10_ext_cfg_enable_bits(device_t fam10_dev,
	u32 reg_pos, u32 mask, u32 val)
{
	u32 reg_old, reg;

	/* family 10 only, for reg > 0xFF */
	if (!IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10))
		return;

	reg = reg_old = pci_read_config32(fam10_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(fam10_dev, reg_pos, reg);
	}
}

/*
* Compliant with CIM_33's ATINB_PrepareInit
*/
static void get_cpu_rev(void)
{
	u32 eax;

	eax = cpuid_eax(1);
	printk(BIOS_INFO, "get_cpu_rev EAX=0x%x.\n", eax);
	if (eax <= 0xfff)
		printk(BIOS_INFO, "CPU Rev is K8_Cx.\n");
	else if (eax <= 0x10fff)
		printk(BIOS_INFO, "CPU Rev is K8_Dx.\n");
	else if (eax <= 0x20fff)
		printk(BIOS_INFO, "CPU Rev is K8_Ex.\n");
	else if (eax <= 0x40fff)
		printk(BIOS_INFO, "CPU Rev is K8_Fx.\n");
	else if (eax == 0x60fb1 || eax == 0x60f81)	/*These two IDS are exception, they are G1. */
		printk(BIOS_INFO, "CPU Rev is K8_G1.\n");
	else if (eax <= 0X60FF0)
		printk(BIOS_INFO, "CPU Rev is K8_G0.\n");
	else if (eax <= 0x100000)
		printk(BIOS_INFO, "CPU Rev is K8_G1.\n");
	else if (eax <= 0x100fa0)
		printk(BIOS_INFO, "CPU Rev is Fam 10.\n");
	else if (eax <= 0x600f20)
		printk(BIOS_INFO, "CPU Rev is Fam 15.\n");
	else
		printk(BIOS_INFO, "CPU Rev is not recognized.\n");
}

/*
CIM NB_GetRevisionInfo()
*/
static u8 get_nb_rev(device_t nb_dev)
{
	u8 reg;
	reg = pci_read_config8(nb_dev, 0x8);	/* copy from CIM, can't find in doc */
	switch (reg & 3)
	{
	case 0x00:
		reg = REV_SR5650_A11;
		break;
	case 0x02:
	default:
		reg = REV_SR5650_A12;
		break;
	}
	return reg;
}

/*****************************************
* Compliant with SR5650_CIMX_4_5_0 NBHT_InitHT().
* Init HT link speed/width for sr5650 -- k8 link
1: Check CPU Family, Family10?
2: Get CPU's HT speed and width
3: Decide HT mode 1 or 3 by HT Speed. >1GHz: HT3, else HT1
4:
*****************************************/
static const u8 sr5650_ibias[] = {
	/* 1, 3 are reserved. */
	[0x0] = 0x44,		/* 200MHz HyperTransport 1 only */
	[0x2] = 0x44,		/* 400MHz HyperTransport 1 only */
	[0x4] = 0xB6,		/* 600MHz HyperTransport 1 only */
	[0x5] = 0x44,		/* 800MHz HyperTransport 1 only */
	[0x6] = 0x96,		/* 1GHz   HyperTransport 1 only */
	/* HT3 for Family 10 */
	[0x7] = 0xB6,		/* 1.2GHz HyperTransport 3 only */
	[0x8] = 0x23,		/* 1.4GHz HyperTransport 3 only */
	[0x9] = 0x44,		/* 1.6GHz HyperTransport 3 only */
	[0xa] = 0x64,		/* 1.8GHz HyperTransport 3 only */
	[0xb] = 0x96,		/* 2.0GHz HyperTransport 3 only */
	[0xc] = 0xA6,		/* 2.2GHz HyperTransport 3 only */
	[0xd] = 0xB6,		/* 2.4GHz HyperTransport 3 only */
	[0xe] = 0xC6,		/* 2.6GHz HyperTransport 3 only */
};

void sr5650_htinit(void)
{
	/*
	 * About HT, it has been done in enumerate_ht_chain().
	 */
	device_t cpu_f0, sr5650_f0, clk_f1;
	u32 reg;
	u8 cpu_ht_freq, cpu_htfreq_max, ibias;
	u8 sbnode;
	u8 sblink;
	u16 linkfreq_reg;
	u16 linkfreqext_reg;

	/************************
	* get cpu's ht freq, in cpu's function 0, offset 0x88
	* bit11-8, specifics the maximum operation frequency of the link's transmitter clock.
	* The link frequency field (Frq) is cleared by cold reset. SW can write a nonzero
	* value to this reg, and that value takes effect on the next warm reset or
	* LDTSTOP_L disconnect sequence.
	* please see the table sr5650_ibias about the value and its corresponding frequency.
	************************/
	/* Link0, Link1 are for connection between P0 and P1.
	 * TODO: Check the topology of the MP and NB. Or we just read the nbconfig? */
	/* NOTE: In most cases, we only have one CPU. In that case, we should read 0x88. */

	/* Find out the node ID and the Link ID that
	 * connects to the Southbridge (system IO hub).
	 */
	sbnode = (pci_read_config32(PCI_DEV(0, 0x18, 0), 0x60) >> 8) & 7;
	sblink = (pci_read_config32(PCI_DEV(0, 0x18, 0), 0x64) >> 8) & 3; /* bit[10] sublink, bit[9,8] link. */
	cpu_f0 = PCI_DEV(0, (0x18 + sbnode), 0);

	/*
	 * link freq reg of Link0, 1, 2, 3 is 0x88, 0xA8, 0xC8, 0xE8 respectively
	 * link freq ext reg of Link0, 1, 2, 3 is 0x9C, 0xBC, 0xDC, 0xFC respectively
	 */
	linkfreq_reg = 0x88 + (sblink << 5);
	linkfreqext_reg = 0x9C + (sblink << 5);
	reg = pci_read_config32(cpu_f0, linkfreq_reg);

	cpu_ht_freq = (reg & 0xf00) >> 8;

	/* Freq[4] is only valid for revision D and later processors */
	if (cpuid_eax(1) >= 0x100F80) {
		cpu_htfreq_max = 0x14;
		cpu_ht_freq |= ((pci_read_config32(cpu_f0, linkfreqext_reg) & 0x01) << 4);
	} else {
		cpu_htfreq_max = 0x0F;
	}

	printk(BIOS_INFO, "sr5650_htinit: Node %x Link %x, HT freq=%x.\n",
			sbnode, sblink, cpu_ht_freq);
	sr5650_f0 = PCI_DEV(0, 0, 0);

	clk_f1 = PCI_DEV(0, 0, 1); /* We need to make sure the F1 is accessible. */

	ibias = sr5650_ibias[cpu_ht_freq];

	/* If HT freq>1GHz, we assume the CPU is fam10, else it is K8.
	 * Is it appropriate?
	 * Frequency is 1GHz, i.e. cpu_ht_freq is 6, in most cases.
	 * So we check 6 only, it would be faster. */
	if ((cpu_ht_freq == 0x6) || (cpu_ht_freq == 0x5) || (cpu_ht_freq == 0x4) ||
		(cpu_ht_freq == 0x2) || (cpu_ht_freq == 0x0)) {
		printk(BIOS_INFO, "sr5650_htinit: HT1 mode\n");

		/* HT1 mode, RPR 5.4.2 */
		/* set IBIAS code */
		set_nbcfg_enable_bits(clk_f1, 0xD8, 0x3FF, ibias);
		/* Optimizes chipset HT transmitter drive strength */
		set_htiu_enable_bits(sr5650_f0, 0x2A, 0x3, 0x3);
	} else if ((cpu_ht_freq > 0x6) && (cpu_ht_freq < cpu_htfreq_max)) {
		printk(BIOS_INFO, "sr5650_htinit: HT3 mode\n");

		/* Enable Protocol checker */
		set_htiu_enable_bits(sr5650_f0, 0x1E, 0xFFFFFFFF, 0x7FFFFFFC);

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
		/* HT3 mode, RPR 5.4.3 */
		set_nbcfg_enable_bits(sr5650_f0, 0x9c, 0x3 << 16, 0);

		/* set IBIAS code */
		set_nbcfg_enable_bits(clk_f1, 0xD8, 0x3FF, ibias);
		/* Optimizes chipset HT transmitter drive strength */
		set_htiu_enable_bits(sr5650_f0, 0x2A, 0x3, 0x1);
		/* Enables error-retry mode */
		set_nbcfg_enable_bits(sr5650_f0, 0x44, 0x1, 0x1);
		/* Enables scrambling and Disables command throttling */
		set_nbcfg_enable_bits(sr5650_f0, 0xac, (1 << 3) | (1 << 14), (1 << 3) | (1 << 14));
		/* Enables transmitter de-emphasis */
		set_nbcfg_enable_bits(sr5650_f0, 0xa4, 1 << 31, 1 << 31);
		/* Enables transmitter de-emphasis level */
		/* Sets training 0 time */
		set_nbcfg_enable_bits(sr5650_f0, 0xa0, 0x3F, 0x14);

		/* Enables strict TM4 detection */
		set_htiu_enable_bits(sr5650_f0, 0x15, 0x1 << 22, 0x1 << 22);

		/* Optimizes chipset HT transmitter drive strength */
		set_htiu_enable_bits(sr5650_f0, 0x2A, 0x3 << 0, 0x1 << 0);

		/* HyperTransport 3 Processor register settings to be done in northbridge */

		/* Enables error-retry mode */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x130 + (sblink << 2), 1 << 0, 1 << 0);

		/* Enables scrambling */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x170 + (sblink << 2), 1 << 3, 1 << 3);

		/* Enables transmitter de-emphasis
		 * This depends on the PCB design and the trace
		 */
		/* Disables command throttling */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x168, 1 << 10, 1 << 10);

		/* Sets Training 0 Time. See T0Time table for encodings */
		/* AGESA have set it to recommended value already
		 * The recommended values are 14h(2us) if F0x[18C:170][LS2En]=0
		 * and 26h(12us) if F0x[18C:170][LS2En]=1
		 */
		//set_fam10_ext_cfg_enable_bits(cpu_f0, 0x16C, 0x3F, 0x26);

		/* HT Buffer Allocation for Ganged Links!!! */
#endif	/* CONFIG_NORTHBRIDGE_AMD_AMDFAM10 */
	}

}

/* Must be run immediately after HT setup is complete and first warm reset has occurred (if applicable)
 * Attempting to switch the NB into isochronous mode before the CPUs have engaged isochronous mode
 * will cause a system hard lockup...
 */
void sr5650_htinit_dect_and_enable_isochronous_link(void)
{
	device_t sr5650_f0;
	unsigned char iommu;

	sr5650_f0 = PCI_DEV(0, 0, 0);

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		/* Enable isochronous mode */
		set_nbcfg_enable_bits(sr5650_f0, 0xc8, 1 << 12, 1 << 12);

		/* Apply pending changes */
		if (!((pci_read_config32(sr5650_f0, 0xc8) >> 12) & 0x1)) {
			printk(BIOS_INFO, "...WARM RESET...\n\n\n");
			soft_reset();
			die("After soft_reset - shouldn't see this message!!!\n");
		}
	}
}

void fam10_optimization(void)
{
	device_t cpu_f0, cpu_f2, cpu_f3;
	device_t cpu1_f0, cpu1_f2, cpu1_f3;
	msr_t msr;
	u32 val;

	if (!IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10))
		return;

	printk(BIOS_INFO, "fam10_optimization()\n");
	msr = rdmsr(0xC001001F);
	msr.hi |= 1 << 14;	/* bit 46: EnableCf8ExtCfg */
	wrmsr(0xC001001F, msr);

	cpu_f0 = PCI_DEV(0, 0x18, 0);
	cpu_f2 = PCI_DEV(0, 0x18, 2);
	cpu_f3 = PCI_DEV(0, 0x18, 3);
	cpu1_f0 = PCI_DEV(0, 0x19, 0);
	cpu1_f2 = PCI_DEV(0, 0x19, 2);
	cpu1_f3 = PCI_DEV(0, 0x19, 3);

	val = pci_read_config32(cpu1_f3, 0x8C);
	val |= 1 << 14;
	pci_write_config32(cpu1_f3, 0x8C, val);

	/* TODO: HT Buffer Allocation for (un)Ganged Links */
	/* rpr Table 5-11, 5-12 */
}

/*****************************************
* Compliant with CIM_33's ATINB_PCICFG_POR_TABLE
*****************************************/
static void sr5650_por_pcicfg_init(device_t nb_dev)
{
	/* enable PCI Memory Access */
	set_nbcfg_enable_bits_8(nb_dev, 0x04, (u8)(~0xFD), 0x02);

	set_nbcfg_enable_bits(nb_dev, 0x14, ~0, 0x0);
	set_nbcfg_enable_bits(nb_dev, 0x18, ~0, 0x0);
	set_nbcfg_enable_bits(nb_dev, 0x20, ~0, 0x0);
	set_nbcfg_enable_bits(nb_dev, 0x84, ~0, 0x03000010);

	/* Reg4Ch[1]=1 (APIC_ENABLE) force CPU request with address 0xFECx_xxxx to south-bridge
	 * Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
	 * BMMsgEn */
	set_nbcfg_enable_bits(nb_dev, 0x4C, (u8)(~0x00), 0x52042);

	set_nbcfg_enable_bits(nb_dev, 0x7C, (u8)(~0), 0x0);

	/* Reg8Ch[10:9] = 0x3 Enables Gfx Debug BAR,
	 * force this BAR as mem type in sr5650_gfx.c */
	//set_nbcfg_enable_bits_8(nb_dev, 0x8D, (u8)(~0xFF), 0x03);
}

/*****************************************
* Compliant with CIM_33's ATINB_MISCIND_POR_TABLE
* Compliant with CIM_33's MISC_INIT_TBL
*****************************************/
static void sr5650_por_misc_index_init(device_t nb_dev)
{
	unsigned char iommu;

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		/* enable IOMMU */
		printk(BIOS_DEBUG, "Enabling IOMMU\n");
		set_nbmisc_enable_bits(nb_dev, 0x75, 0x1, 0x1);
	} else {
		/* disable IOMMU */
		printk(BIOS_DEBUG, "Disabling IOMMU\n");
		set_nbmisc_enable_bits(nb_dev, 0x75, 0x1, 0x0);
	}

	/* NBMISCIND:0x75[29]= 1 Device ID for hotplug and PME message */
	set_nbmisc_enable_bits(nb_dev, 0x75, 1 << 29, 1 << 29);
	set_nbmisc_enable_bits(nb_dev, 0x75, 1 << 9, 1 << 9); /* no doc reference, comply with BTS */
	set_nbmisc_enable_bits(nb_dev, 0x46, 1 << 7, 1 << 7); /* bit7 BTS fail*/
	/*P2P*/
	set_nbmisc_enable_bits(nb_dev, 0x48, 1 << 8, 0);

	set_nbmisc_enable_bits(nb_dev, 0x2A, 1 << 15 | 1 << 17, 1 << 17);
	set_nbmisc_enable_bits(nb_dev, 0x2B, 1 << 15 | 1 << 27, 1 << 15 | 1 << 27);
	set_nbmisc_enable_bits(nb_dev, 0x2C, 1 << 0 | 1 << 1 | 1 << 5 | 1 << 4 | 1 << 10, 1 << 0 | 1 << 1 | 1 << 5);
	set_nbmisc_enable_bits(nb_dev, 0x32, 0x3F << 20, 0x2A << 20);
	set_nbmisc_enable_bits(nb_dev, 0x34, 1 << 7 | 1 << 15 | 1 << 23, 0);
	set_nbmisc_enable_bits(nb_dev, 0x35, 0x3F << 26, 0x2A << 26);
	set_nbmisc_enable_bits(nb_dev, 0x37, 0xfff << 20, 0xddd << 20);
	set_nbmisc_enable_bits(nb_dev, 0x37, 7 << 11, 0);
	/* PCIE CDR setting */
	set_nbmisc_enable_bits(nb_dev, 0x38, 0xFFFFFFFF, 0xC0C0C0);
	set_nbmisc_enable_bits(nb_dev, 0x22, 0xFFFFFFFF, (1 << 27) | (0x8 << 12) | (0x8 << 16) | (0x8 << 20));
	set_nbmisc_enable_bits(nb_dev, 0x22, 1 << 1 | 1 << 2 | 1 << 6 | 1 << 7, 1 << 1 | 1 << 2 | 1 << 6 | 1 << 7);

	set_nbmisc_enable_bits(nb_dev, 0x07, 0xF << 4 | 1 << 24, 0xF << 4 | 1 << 24);
	set_nbmisc_enable_bits(nb_dev, 0x67, 1 << 10 | 1 << 11 | 1 << 26, 1 << 11);
	set_nbmisc_enable_bits(nb_dev, 0x67, 3 << 21, 3 << 21);
	set_nbmisc_enable_bits(nb_dev, 0x68, 1 << 8 | 1 << 9 | 1 << 19, 1 << 9 | 1 << 19);
	set_nbmisc_enable_bits(nb_dev, 0x6B, 3 << 3 | 1 << 15 | 0x1F << 27, 3 << 3 | 1 << 15 | 0x1F << 27);
	set_nbmisc_enable_bits(nb_dev, 0x6C, 0xFFFFFFFF, 0x41183000);

	/* NB_MISC_IND_WR_EN + IOC_PCIE_CNTL
	 * Block non-snoop DMA request if PMArbDis is set.
	 * Set BMSetDis */
	set_nbmisc_enable_bits(nb_dev, 0x0B, 0xFFFFFFFF, 0x00400180);
	set_nbmisc_enable_bits(nb_dev, 0x01, 0xFFFFFFFF, 0x00000310);

	/* NBCFG (NBMISCIND 0x0): NB_CNTL -
	 *   HIDE_NB_AGP_CAP    ([0], default=1)HIDE
	 *   HIDE_P2P_AGP_CAP   ([1], default=1)HIDE
	 *   HIDE_NB_GART_BAR   ([2], default=1)HIDE
	 *   HIDE_MMCFG_BAR     ([3], default=1)SHOW
	 *   AGPMODE30          ([4], default=0)DISABLE
	 *   AGP30ENCHANCED     ([5], default=0)DISABLE
	 *   HIDE_CLKCFG_HEADER ([8], default=0)SHOW */
	set_nbmisc_enable_bits(nb_dev, 0x00, 0x0000FFFF, 0 << 0 | 1 << 1 | 1 << 2 | 0 << 3 | 0 << 6 | 0 << 8);

	/* IOC_LAT_PERF_CNTR_CNTL */
	set_nbmisc_enable_bits(nb_dev, 0x30, 0xFF, 0x00);
	//set_nbmisc_enable_bits(nb_dev, 0x31, 0xFF, 0x00);

	/* IOC_LAT_PERF_CNTR_OUT */
	/* IOC_JTAG_CNTL */
	set_nbmisc_enable_bits(nb_dev, 0x47, 0xFFFFFFFF, 0x0000000B);

	set_nbmisc_enable_bits(nb_dev, 0x12, 0xFFFFFFFF, 0x00FB5555);
	set_nbmisc_enable_bits(nb_dev, 0x0C, 0xFFFFFFFF, 0x001F37FC);
	set_nbmisc_enable_bits(nb_dev, 0x15, 0xFFFFFFFF, 0x0);

	/* NB_PROG_DEVICE_REMAP */
	set_nbmisc_enable_bits(nb_dev, 0x20, 0xFFFFFFFF, 0x0);
	set_nbmisc_enable_bits(nb_dev, 0x21, 0xFFFFFFFF, 0x0);

	/* Compliant with CIM_33's MISC_INIT_TBL, except Hide NB_BAR3_PCIE
	 * Enable access to DEV8
	 * Enable setPower message for all ports
	 */
	set_nbmisc_enable_bits(nb_dev, 0x51, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x53, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x55, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x57, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x59, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x5B, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x5D, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x5F, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x61, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);
	set_nbmisc_enable_bits(nb_dev, 0x63, 1 << 20 | 1 << 8, 1 << 20 | 1 << 8);

	/* Disable bus-master trigger event from SB and Enable set_slot_power message to SB */
	set_nbmisc_enable_bits(nb_dev, 0x0B, 0xffffffff, 0x400180);
}

/*****************************************
* Some setting is from rpr. Some is from CIMx.
*****************************************/
static void sr5650_por_htiu_index_init(device_t nb_dev)
{
	device_t cpu_f0;

	cpu_f0 = PCI_DEV(0, 0x18, 0);

	set_htiu_enable_bits(nb_dev, 0x1C, 0x1<<17, 0x1<<17);
	set_htiu_enable_bits(nb_dev, 0x05, 0x1<<8,  0x1<<8);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<0,  0x0<<0);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<1,  0x1<<1);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<9,  0x1<<9);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<13, 0x1<<13);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<17, 0x1<<17);
	set_htiu_enable_bits(nb_dev, 0x06, 0x3<<15, 0x3<<15);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<25, 0x1<<25);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<30, 0x1<<30);

	set_htiu_enable_bits(nb_dev, 0x07, 0x1 << 0 | 0x1 << 1 | 0x1 << 2, 0x1 << 0);

	set_htiu_enable_bits(nb_dev, 0x16, 0x1<<11,  0x1<<11);

	set_htiu_enable_bits(nb_dev, 0x1D, 0x1<<2,  0x1<<2);
	set_htiu_enable_bits(nb_dev, 0x1D, 0x1<<4,  0x1<<4);

	axindxc_reg(0x10, 1 << 9, 1 << 9);
	set_pcie_enable_bits(nb_dev, 0x10 | 5 << 16, 1 << 9, 1 << 9);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<26, 0x1<<26);
	set_htiu_enable_bits(nb_dev, 0x16, 0x1<<10, 0x1<<10);

	/* Enable BIAS circuit for all lanes. */
	//set_htiu_enable_bits(nb_dev, 0x2B, 0xF<<28, 0xF<<28);
	set_htiu_enable_bits(nb_dev, 0x2B, 0xF << 28, 0);
	set_htiu_enable_bits(nb_dev, 0x05, 0xFFFFFF, 0xFF558);
	set_htiu_enable_bits(nb_dev, 0x06, 0xFFFFFFFE, 0x04203A202);
	set_htiu_enable_bits(nb_dev, 0x0C, 0xFFFF, 0x101);

	/* A21 only */
	//if (REV_SR5650_A21 == get_nb_rev(nb_dev)) {
	if (get_nb_rev(nb_dev) > REV_SR5650_A11) {
		set_htiu_enable_bits(nb_dev, 0x05, 0x3<<3| 1<<6 | 1<<10 | 0xFF<<12, 0x3<<3 | 1<<6 | 1<<10 | 0xFF<<12);
		set_htiu_enable_bits(nb_dev, 0x1D, 1 << 2 | 1 << 4, 0);
	}
}

/*****************************************
* Compliant with CIM_33's ATINB_POR_INIT_JMPDI
* Configure SR5650 registers to power-on default RPR.
* POR: Power On Reset
* RPR: Register Programming Requirements
*****************************************/
static void sr5650_por_init(device_t nb_dev)
{
	printk(BIOS_INFO, "sr5650_por_init\n");
	/* ATINB_PCICFG_POR_TABLE, initialize the values for sr5650 PCI Config registers */
	sr5650_por_pcicfg_init(nb_dev);

	/* ATINB_MISCIND_POR_TABLE */
	sr5650_por_misc_index_init(nb_dev);

	/* ATINB_HTIUNBIND_POR_TABLE */
	sr5650_por_htiu_index_init(nb_dev);

	/* ATINB_CLKCFG_PORT_TABLE */
	/* sr5650 A11 SB Link full swing? */
}

/* enable CFG access to Dev8, which is the SB P2P Bridge */
void enable_sr5650_dev8(void)
{
	set_nbmisc_enable_bits(PCI_DEV(0, 0, 0), 0x00, 1 << 6, 1 << 6);
}

/*
* Compliant with CIM_33's AtiNBInitEarlyPost (AtiInitNBBeforePCIInit).
*/
void sr5650_before_pci_init(void)
{
}

/*
* The calling sequence is same as CIM.
*/
void sr5650_early_setup(void)
{
	device_t nb_dev = PCI_DEV(0, 0, 0);
	printk(BIOS_INFO, "sr5650_early_setup()\n");

	/*ATINB_PrepareInit */
	get_cpu_rev();

	uint8_t revno = get_nb_rev(nb_dev);
	switch (revno) {	/* PCIEMiscInit */
	case REV_SR5650_A11:
		printk(BIOS_INFO, "NB Revision is A11.\n");
		break;
	case REV_SR5650_A12:
		printk(BIOS_INFO, "NB Revision is A12.\n");
		break;
	case REV_SR5650_A21:
		printk(BIOS_INFO, "NB Revision is A21.\n");
		break;
	default:
		printk(BIOS_INFO, "NB Revision is %02x (Unrecognized).\n", revno);
		break;
	}

	fam10_optimization();
	sr5650_por_init(nb_dev);
}

/**
 * @brief disable GPP1 Port0,1, GPP2, GPP3a Port0,1,2,3,4,5, GPP3b
 *
 */
void sr5650_disable_pcie_bridge(void)
{
	u32 mask;
	u32 reg;
	device_t nb_dev = PCI_DEV(0, 0, 0);

	mask = (1 << 2) | (1 << 3); /*GPP1*/
	mask |= (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7) | (1 << 16) | (1 << 17); /*GPP3a*/
	mask |= (1 << 18) | (1 << 19); /*GPP2*/
	mask |= (1 << 20); /*GPP3b*/
	reg = mask;
	set_nbmisc_enable_bits(nb_dev, 0x0c, mask, reg);
}
