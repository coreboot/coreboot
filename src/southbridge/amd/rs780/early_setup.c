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

#include <types.h>
#include <arch/io.h>
#include <northbridge/amd/amdmct/mct/mct_d.h>
#include <console/console.h>
#include <cpu/x86/msr.h>

#include "rev.h"
#include "rs780.h"

#define NBHTIU_INDEX		0x94 /* Note: It is different with RS690, whose HTIU index is 0xA8 */
#define NBMISC_INDEX		0x60
#define NBMC_INDEX 		0xE8

static u32 nb_read_index(pci_devfn_t dev, u32 index_reg, u32 index)
{
	pci_write_config32(dev, index_reg, index);
	return pci_read_config32(dev, index_reg + 0x4);
}

static void nb_write_index(pci_devfn_t dev, u32 index_reg, u32 index, u32 data)
{
	pci_write_config32(dev, index_reg, index /* | 0x80 */ );
	pci_write_config32(dev, index_reg + 0x4, data);
}

static u32 nbmisc_read_index(pci_devfn_t nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMISC_INDEX, (index));
}

static void nbmisc_write_index(pci_devfn_t nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMISC_INDEX, ((index) | 0x80), (data));
}

static u32 htiu_read_index(pci_devfn_t nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBHTIU_INDEX, (index));
}

static void htiu_write_index(pci_devfn_t nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBHTIU_INDEX, ((index) | 0x100), (data));
}

static u32 nbmc_read_index(pci_devfn_t nb_dev, u32 index)
{
	return nb_read_index((nb_dev), NBMC_INDEX, (index));
}

static void nbmc_write_index(pci_devfn_t nb_dev, u32 index, u32 data)
{
	nb_write_index((nb_dev), NBMC_INDEX, ((index) | 1 << 9), (data));
}

static void set_htiu_enable_bits(pci_devfn_t nb_dev, u32 reg_pos, u32 mask,
				 u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = htiu_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		htiu_write_index(nb_dev, reg_pos, reg);
	}
}

static void set_nbmisc_enable_bits(pci_devfn_t nb_dev, u32 reg_pos, u32 mask,
				   u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmisc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmisc_write_index(nb_dev, reg_pos, reg);
	}
}

static void set_nbcfg_enable_bits(pci_devfn_t nb_dev, u32 reg_pos, u32 mask,
				  u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = pci_read_config32(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config32(nb_dev, reg_pos, reg);
	}
}
/* family 10 only, for reg > 0xFF */
#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
static void set_fam10_ext_cfg_enable_bits(pci_devfn_t fam10_dev, u32 reg_pos,
		u32 mask, u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = Get_NB32(fam10_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		Set_NB32(fam10_dev, reg_pos, reg);
	}
}
#else
#define set_fam10_ext_cfg_enable_bits(a, b, c, d) do {} while (0)
#endif


static void set_nbcfg_enable_bits_8(pci_devfn_t nb_dev, u32 reg_pos, u8 mask,
				    u8 val)
{
	u8 reg_old, reg;
	reg = reg_old = pci_read_config8(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		pci_write_config8(nb_dev, reg_pos, reg);
	}
}

static void set_nbmc_enable_bits(pci_devfn_t nb_dev, u32 reg_pos, u32 mask,
				 u32 val)
{
	u32 reg_old, reg;
	reg = reg_old = nbmc_read_index(nb_dev, reg_pos);
	reg &= ~mask;
	reg |= val;
	if (reg != reg_old) {
		nbmc_write_index(nb_dev, reg_pos, reg);
	}
}

static u8 is_famly10(void)
{
	return (cpuid_eax(1) & 0xff00000) != 0;
}

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
static u8 l3_cache(void)
{
	return (cpuid_edx(0x80000006) & (0x3FFF << 18)) != 0;
}

static u8 cpu_core_number(void)
{
	return (cpuid_ecx(0x80000008) & 0xFF) + 1;
}
#endif

/*****************************************
 * Init HT link speed/width for rs780 -- k8 link
 * 1: Check CPU Family, Family10?
 * 2: Get CPU's HT speed and width
 * 3: Decide HT mode 1 or 3 by HT Speed. >1GHz: HT3, else HT1
 *****************************************/
static const u8 rs780_ibias[] = {
	/* 1, 3 are reserved. */
	[0x0] = 0x4C,		/* 200MHz HyperTransport 1 only */
	[0x2] = 0x4C,		/* 400MHz HyperTransport 1 only */
	[0x4] = 0xB6,		/* 600MHz HyperTransport 1 only */
	[0x5] = 0x4C,		/* 800MHz HyperTransport 1 only */
	[0x6] = 0x9D,		/* 1GHz   HyperTransport 1 only */
	/* HT3 for Family 10 */
	[0x7] = 0xB6,		/* 1.2GHz HyperTransport 3 only */
	[0x8] = 0x2B,		/* 1.4GHz HyperTransport 3 only */
	[0x9] = 0x4C,		/* 1.6GHz HyperTransport 3 only */
	[0xa] = 0x6C,		/* 1.8GHz HyperTransport 3 only */
	[0xb] = 0x9D,		/* 2.0GHz HyperTransport 3 only */
	[0xc] = 0xAD,		/* 2.2GHz HyperTransport 3 only */
	[0xd] = 0xB6,		/* 2.4GHz HyperTransport 3 only */
	[0xe] = 0xC6,		/* 2.6GHz HyperTransport 3 only */
};

void rs780_htinit(void)
{
	/*
	 * About HT, it has been done in enumerate_ht_chain().
	 */
	pci_devfn_t cpu_f0, rs780_f0, clk_f1;
	u32 reg;
	u8 cpu_ht_freq, ibias;

	cpu_f0 = PCI_DEV(0, 0x18, 0);
	/************************
	* get cpu's ht freq, in cpu's function 0, offset 0x88
	* bit11-8, specifics the maximum operation frequency of the link's transmitter clock.
	* The link frequency field (Frq) is cleared by cold reset. SW can write a nonzero
	* value to this reg, and that value takes effect on the next warm reset or
	* LDTSTOP_L disconnect sequence.
	* please see the table rs780_ibias about the value and its corresponding frequency.
	************************/
	reg = pci_read_config32(cpu_f0, 0x88);
	cpu_ht_freq = (reg & 0xf00) >> 8;
	printk(BIOS_INFO, "rs780_htinit cpu_ht_freq=%x.\n", cpu_ht_freq);
	rs780_f0 = PCI_DEV(0, 0, 0);
	//set_nbcfg_enable_bits(rs780_f0, 0xC8, 0x7<<24 | 0x7<<28, 1<<24 | 1<<28);

	clk_f1 = PCI_DEV(0, 0, 1); /* We need to make sure the F1 is accessible. */

	ibias = rs780_ibias[cpu_ht_freq];

	/* If HT freq>1GHz, we assume the CPU is fam10, else it is K8.
	 * Is it appropriate?
	 * Frequency is 1GHz, i.e. cpu_ht_freq is 6, in most cases.
	 * So we check 6 only, it would be faster. */
	if ((cpu_ht_freq == 0x6) || (cpu_ht_freq == 0x5) || (cpu_ht_freq == 0x4) ||
		(cpu_ht_freq == 0x2) || (cpu_ht_freq == 0x0)) {
		printk(BIOS_INFO, "rs780_htinit: HT1 mode\n");

		/* HT1 mode, RPR 8.4.2 */
		/* set IBIAS code */
		set_nbcfg_enable_bits(clk_f1, 0xD8, 0x3FF, ibias);
		/* Optimizes chipset HT transmitter drive strength */
		set_htiu_enable_bits(rs780_f0, 0x2A, 0x3, 0x1);
	} else if ((cpu_ht_freq > 0x6) && (cpu_ht_freq < 0xf)) {
		printk(BIOS_INFO, "rs780_htinit: HT3 mode\n");

		#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
		/* HT3 mode, RPR 8.4.3 */
		set_nbcfg_enable_bits(rs780_f0, 0x9c, 0x3 << 16, 0);

		/* set IBIAS code */
		set_nbcfg_enable_bits(clk_f1, 0xD8, 0x3FF, ibias);
		/* Optimizes chipset HT transmitter drive strength */
		set_htiu_enable_bits(rs780_f0, 0x2A, 0x3, 0x1);
		/* Enables error-retry mode */
		set_nbcfg_enable_bits(rs780_f0, 0x44, 0x1, 0x1);
		/* Enables scrambling and Disables command throttling */
		set_nbcfg_enable_bits(rs780_f0, 0xac, (1 << 3) | (1 << 14), (1 << 3) | (1 << 14));
		/* Enables transmitter de-emphasis */
		set_nbcfg_enable_bits(rs780_f0, 0xa4, 1 << 31, 1 << 31);
		/* Enables transmitter de-emphasis level */
		/* Sets training 0 time */
		set_nbcfg_enable_bits(rs780_f0, 0xa0, 0x3F, 0x14);

		/* Enables strict TM4 detection */
		set_htiu_enable_bits(rs780_f0, 0x15, 0x1 << 22, 0x1 << 22);
		/* Enables proper DLL reset sequence */
		set_htiu_enable_bits(rs780_f0, 0x16, 0x1 << 10, 0x1 << 10);

		/* HyperTransport 3 Processor register settings to be done in northbridge */
		/* Enables error-retry mode */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x130, 1 << 0, 1 << 0);
		/* Enables scrambling */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x170, 1 << 3, 1 << 3);
		/* Enables transmitter de-emphasis
		 * This depends on the PCB design and the trace */
		/* TODO: */
		/* Disables command throttling */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x168, 1 << 10, 1 << 10);
		/* Sets Training 0 Time. See T0Time table for encodings */
		set_fam10_ext_cfg_enable_bits(cpu_f0, 0x16C, 0x3F, 0x20);
		/* TODO: */
		#endif	/* CONFIG_NORTHBRIDGE_AMD_AMDFAM10 */
	}
}

#if !IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
/*******************************************************
* Optimize k8 with UMA.
* See BKDG_NPT_0F guide for details.
* The processor node is addressed by its Node ID on the HT link and can be
* accessed with a device number in the PCI configuration space on Bus0.
* The Node ID 0 is mapped to Device 24 (0x18), the Node ID 1 is mapped
* to Device 25, and so on.
* The processor implements configuration registers in PCI configuration
* space using the following four headers
*	Function0: HT technology configuration
*	Function1: Address map configuration
*	Function2: DRAM and HT technology Trace mode configuration
*	Function3: Miscellaneous configuration
*******************************************************/
static void k8_optimization(void)
{
	pci_devfn_t k8_f0, k8_f2, k8_f3;
	msr_t msr;

	printk(BIOS_INFO, "k8_optimization()\n");
	k8_f0 = PCI_DEV(0, 0x18, 0);
	k8_f2 = PCI_DEV(0, 0x18, 2);
	k8_f3 = PCI_DEV(0, 0x18, 3);

	/* 8.6.6 K8 Buffer Allocation Settings */
	pci_write_config32(k8_f0, 0x90, 0x01700169);	/* CIM NPT_Optimization */
	set_nbcfg_enable_bits(k8_f0, 0x68, 1 << 28, 0 << 28);
	set_nbcfg_enable_bits(k8_f0, 0x68, 3 << 26, 3 << 26);
	set_nbcfg_enable_bits(k8_f0, 0x68, 1 << 11, 1 << 11);
	/* set_nbcfg_enable_bits(k8_f0, 0x84, 1 << 11 | 1 << 13 | 1 << 15, 1 << 11 | 1 << 13 | 1 << 15); */	/* TODO */

	pci_write_config32(k8_f3, 0x70, 0x51220111);
	pci_write_config32(k8_f3, 0x74, 0x50404021);
	pci_write_config32(k8_f3, 0x78, 0x08002A00);
	if (pci_read_config32(k8_f3, 0xE8) & 0x3<<12)
		pci_write_config32(k8_f3, 0x7C, 0x0000211A); /* dual core */
	else
		pci_write_config32(k8_f3, 0x7C, 0x0000212B); /* single core */
	set_nbcfg_enable_bits_8(k8_f3, 0xDC, 0xFF, 0x25);

	set_nbcfg_enable_bits(k8_f2, 0xA0, 1 << 5, 1 << 5);
	set_nbcfg_enable_bits(k8_f2, 0x94, 0xF << 24, 7 << 24);
	set_nbcfg_enable_bits(k8_f2, 0x90, 1 << 10, 0 << 10);
	set_nbcfg_enable_bits(k8_f2, 0xA0, 3 << 2, 3 << 2);
	set_nbcfg_enable_bits(k8_f2, 0xA0, 1 << 5, 1 << 5);

	msr = rdmsr(0xC001001F);
	msr.lo &= ~(1 << 9);
	msr.hi &= ~(1 << 4);
	wrmsr(0xC001001F, msr);
}
#else
#define k8_optimization() do {} while (0)
#endif	/* !CONFIG_NORTHBRIDGE_AMD_AMDFAM10 */

#if IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AMDFAM10)
static void fam10_optimization(void)
{
	pci_devfn_t cpu_f0, cpu_f2, cpu_f3;
	u32 val;

	printk(BIOS_INFO, "fam10_optimization()\n");

	cpu_f0 = PCI_DEV(0, 0x18, 0);
	cpu_f2 = PCI_DEV(0, 0x18, 2);
	cpu_f3 = PCI_DEV(0, 0x18, 3);

	/* 8.6.4.1 */
	/* Table 8-13 */
	pci_write_config32(cpu_f0, 0x90, 0x808502D0);
	/* Table 8-14 */
	pci_write_config32(cpu_f0, 0x94, 0x00000000);

	/* Table 8-15 */
	val = pci_read_config32(cpu_f0, 0x68);
	val |= 1 << 24;
	pci_write_config32(cpu_f0, 0x68, val);

	/* Table 8-16 */
	val = pci_read_config32(cpu_f0, 0x84);
	val &= ~(1 << 12);
	pci_write_config32(cpu_f0, 0x84, val);

	/* Table 8-17 */
	val = pci_read_config32(cpu_f2, 0x90);
	val &= ~(1 << 10);
	pci_write_config32(cpu_f2, 0x90, val);

	/* Table 8-18 */
	pci_write_config32(cpu_f3, 0x6C, 0x60018051);
	/* Table 8-19 */
	pci_write_config32(cpu_f3, 0x70, 0x60321151);
	/* Table 8-20 */
	pci_write_config32(cpu_f3, 0x74, 0x00980101);
	/* Table 8-21 */
	pci_write_config32(cpu_f3, 0x78, 0x00200C14);
	/* Table 8-22 */
	pci_write_config32(cpu_f3, 0x7C, 0x00070811); /* TODO: Check if L3 Cache is enabled. */

	/* Table 8-23 */
	Set_NB32(cpu_f3, 0x140, 0x00D33656);
	/* Table 8-24 */
	Set_NB32(cpu_f3, 0x144, 0x00000036);
	/* Table 8-25 */
	Set_NB32(cpu_f3, 0x148, 0x8000832A);
	/* Table 8-26 */
	Set_NB32(cpu_f3, 0x158, 0);
	/*          L3 Disabled:  L3 Enabled: */
	/* cores:    2   3    4    2   3    4  */
	/* bit8:4   28  26   24   24  20   16 */
	if (!l3_cache()) {
		Set_NB32(cpu_f3, 0x1A0, 4 << 12 | (24 + 2*(4-cpu_core_number())) << 4 | 2);
	} else {
		Set_NB32(cpu_f3, 0x1A0, 4 << 12 | (16 + 4*(4-cpu_core_number())) << 4 | 4);
	}
}
#else
#define fam10_optimization() do {} while (0)
#endif	/* CONFIG_NORTHBRIDGE_AMD_AMDFAM10 */

/*****************************************
* rs780_por_pcicfg_init()
*****************************************/
static void rs780_por_pcicfg_init(pci_devfn_t nb_dev)
{
	/* enable PCI Memory Access */
	set_nbcfg_enable_bits_8(nb_dev, 0x04, (u8)(~0xFD), 0x02);
	/* Set RCRB Enable */
	set_nbcfg_enable_bits_8(nb_dev, 0x84, (u8)(~0xFF), 0x1);
	/* allow decode of 640k-1MB */
	set_nbcfg_enable_bits_8(nb_dev, 0x84, (u8)(~0xEF), 0x10);
	/* Enable PM2_CNTL(BAR2) IO mapped cfg write access to be broadcast to both NB and SB */
	set_nbcfg_enable_bits_8(nb_dev, 0x84, (u8)(~0xFF), 0x4);
	/* Power Management Register Enable */
	set_nbcfg_enable_bits_8(nb_dev, 0x84, (u8)(~0xFF), 0x80);

	/* Reg4Ch[1]=1 (APIC_ENABLE) force CPU request with address 0xFECx_xxxx to south-bridge
	 * Reg4Ch[6]=1 (BMMsgEn) enable BM_Set message generation
	 * BMMsgEn */
	set_nbcfg_enable_bits_8(nb_dev, 0x4C, (u8)(~0x00), 0x42 | 1);

	/* Reg4Ch[16]=1 (WakeC2En) enable Wake_from_C2 message generation.
	 * Reg4Ch[18]=1 (P4IntEnable) Enable north-bridge to accept MSI with address 0xFEEx_xxxx from south-bridge */
	set_nbcfg_enable_bits_8(nb_dev, 0x4E, (u8)(~0xFF), 0x05);
	/* Reg94h[4:0] = 0x0  P drive strength offset 0
	 * Reg94h[6:5] = 0x2  P drive strength additive adjust */
	set_nbcfg_enable_bits_8(nb_dev, 0x94, (u8)(~0x80), 0x40);

	/* Reg94h[20:16] = 0x0  N drive strength offset 0
	 * Reg94h[22:21] = 0x2  N drive strength additive adjust */
	set_nbcfg_enable_bits_8(nb_dev, 0x96, (u8)(~0x80), 0x40);

	/* Reg80h[4:0] = 0x0  Termination offset
	 * Reg80h[6:5] = 0x2  Termination additive adjust */
	set_nbcfg_enable_bits_8(nb_dev, 0x80, (u8)(~0x80), 0x40);

	/* Reg80h[14] = 0x1   Enable receiver termination control */
	set_nbcfg_enable_bits_8(nb_dev, 0x81, (u8)(~0xFF), 0x40);

	/* Reg94h[15] = 0x1 Enables HT transmitter advanced features to be turned on
	 * Reg94h[14] = 0x1  Enable drive strength control */
	set_nbcfg_enable_bits_8(nb_dev, 0x95, (u8)(~0x3F), 0xC4);

	/* Reg94h[31:29] = 0x7 Enables HT transmitter de-emphasis */
	set_nbcfg_enable_bits_8(nb_dev, 0x97, (u8)(~0x1F), 0xE0);

	/* Reg8Ch[9] enables Gfx Debug BAR programming
	 * Reg8Ch[10] enables Gfx Debug BAR operation
	 * Enable programming of the debug bar now, but enable
	 * operation only after it has been programmed */
	set_nbcfg_enable_bits_8(nb_dev, 0x8D, (u8)(~0xFF), 0x02);
}

static void rs780_por_mc_index_init(pci_devfn_t nb_dev)
{
	set_nbmc_enable_bits(nb_dev, 0x7A, ~0xFFFFFF80, 0x0000005F);
	set_nbmc_enable_bits(nb_dev, 0xD8, ~0x00000000, 0x00600060);
	set_nbmc_enable_bits(nb_dev, 0xD9, ~0x00000000, 0x00600060);
	set_nbmc_enable_bits(nb_dev, 0xE0, ~0x00000000, 0x00000000);
	set_nbmc_enable_bits(nb_dev, 0xE1, ~0x00000000, 0x00000000);
	set_nbmc_enable_bits(nb_dev, 0xE8, ~0x00000000, 0x003E003E);
	set_nbmc_enable_bits(nb_dev, 0xE9, ~0x00000000, 0x003E003E);
}

static void rs780_por_misc_index_init(pci_devfn_t nb_dev)
{
	/* NB_MISC_IND_WR_EN + IOC_PCIE_CNTL
	 * Block non-snoop DMA request if PMArbDis is set.
	 * Set BMSetDis */
	set_nbmisc_enable_bits(nb_dev, 0x0B, ~0xFFFF0000, 0x00000180);
	set_nbmisc_enable_bits(nb_dev, 0x01, ~0xFFFFFFFF, 0x00000040);

	/* NBCFG (NBMISCIND 0x0): NB_CNTL -
	 *   HIDE_NB_AGP_CAP  ([0], default=1)HIDE
	 *   HIDE_P2P_AGP_CAP ([1], default=1)HIDE
	 *   HIDE_NB_GART_BAR ([2], default=1)HIDE
	 *   AGPMODE30        ([4], default=0)DISABLE
	 *   AGP30ENCHANCED   ([5], default=0)DISABLE
	 *   HIDE_AGP_CAP     ([8], default=1)ENABLE */
	set_nbmisc_enable_bits(nb_dev, 0x00, ~0xFFFF0000, 0x00000506);	/* set bit 10 for MSI */

	/* NBMISCIND:0x6A[16]= 1 SB link can get a full swing
	 *      set_nbmisc_enable_bits(nb_dev, 0x6A, 0ffffffffh, 000010000);
	 * NBMISCIND:0x6A[17]=1 Set CMGOOD_OVERRIDE. */
	set_nbmisc_enable_bits(nb_dev, 0x6A, ~0xffffffff, 0x00020000);

	/* NBMISIND:0x40 Bit[8]=1 and Bit[10]=1 following bits are required to set in order to allow LVDS or PWM features to work. */
	set_nbmisc_enable_bits(nb_dev, 0x40, ~0xffffffff, 0x00000500);

	/* NBMISIND:0xC Bit[13]=1 Enable GSM mode for C1e or C3 with pop-up. */
	set_nbmisc_enable_bits(nb_dev, 0x0C, ~0xffffffff, 0x00002000);

	/* Set NBMISIND:0x1F[3] to map NB F2 interrupt pin to INTB# */
	set_nbmisc_enable_bits(nb_dev, 0x1F, ~0xffffffff, 0x00000008);

	/*
	 * Enable access to DEV8
	 * Enable setPower message for all ports
	 */
	set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 6, 1 << 6);
	set_nbmisc_enable_bits(nb_dev, 0x0b, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x51, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x53, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x55, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x57, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x59, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x5B, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x5D, 1 << 20, 1 << 20);
	set_nbmisc_enable_bits(nb_dev, 0x5F, 1 << 20, 1 << 20);

	set_nbmisc_enable_bits(nb_dev, 0x00, 1 << 7, 1 << 7);
	set_nbmisc_enable_bits(nb_dev, 0x07, 0x000000f0, 0x30);

	set_nbmisc_enable_bits(nb_dev, 0x01, 0xFFFFFFFF, 0x48);
	/* Disable bus-master trigger event from SB and Enable set_slot_power message to SB */
	set_nbmisc_enable_bits(nb_dev, 0x0B, 0xffffffff, 0x500180);
}

/*****************************************
* Some setting is from rpr. Some is from CIMx.
*****************************************/
static void rs780_por_htiu_index_init(pci_devfn_t nb_dev)
{
#if 0				/* get from rpr. */
	set_htiu_enable_bits(nb_dev, 0x1C, 0x1<<17, 0x1<<17);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<0,  0x0<<0);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<1,  0x1<<1);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<9,  0x1<<9);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<13, 0x1<<13);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<17, 0x1<<17);
	set_htiu_enable_bits(nb_dev, 0x06, 0x3<<15, 0x3<<15);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<25, 0x1<<25);
	set_htiu_enable_bits(nb_dev, 0x06, 0x1<<30, 0x1<<30);

	set_htiu_enable_bits(nb_dev, 0x07, 0x1<<0,  0x1<<0);
	set_htiu_enable_bits(nb_dev, 0x07, 0x1<<1,  0x0<<1);
	set_htiu_enable_bits(nb_dev, 0x07, 0x1<<2,  0x0<<2);
	set_htiu_enable_bits(nb_dev, 0x07, 0x1<<15, 0x1<<15);

	set_htiu_enable_bits(nb_dev, 0x0C, 0x3<<0,  0x1<<0);
	set_htiu_enable_bits(nb_dev, 0x0C, 0x3<<2,  0x2<<2);
	set_htiu_enable_bits(nb_dev, 0x0C, 0x3<<4,  0x0<<4);

	/* A12 only */
	set_htiu_enable_bits(nb_dev, 0x2D, 0x1<<4,  0x1<<4);
	set_htiu_enable_bits(nb_dev, 0x2D, 0x1<<6,  0x1<<6);
	set_htiu_enable_bits(nb_dev, 0x05, 0x1<<2,  0x1<<2);

	set_htiu_enable_bits(nb_dev, 0x1E, 0xFFFFFFFF, 0xFFFFFFFF);
#else  /* get from CIM. It is more reliable than above. */
	set_htiu_enable_bits(nb_dev, 0x05, (1<<10|1<<9), 1<<10 | 1<<9);
	set_htiu_enable_bits(nb_dev, 0x06, ~0xFFFFFFFE, 0x04203A202);

	set_htiu_enable_bits(nb_dev, 0x07, ~0xFFFFFFF9, 0x8001/*  | 7 << 8 */); /* fam 10 */

	set_htiu_enable_bits(nb_dev, 0x15, ~0xFFFFFFFF, 1<<31| 1<<30 | 1<<27);
	set_htiu_enable_bits(nb_dev, 0x1C, ~0xFFFFFFFF, 0xFFFE0000);

	set_htiu_enable_bits(nb_dev, 0x4B, (1<<11), 1<<11);

	set_htiu_enable_bits(nb_dev, 0x0C, ~0xFFFFFFC0, 1<<0|1<<3);

	set_htiu_enable_bits(nb_dev, 0x17, (1<<27|1<<1), 0x1<<1);
	set_htiu_enable_bits(nb_dev, 0x17, 0x1 << 30, 0x1<<30);

	set_htiu_enable_bits(nb_dev, 0x19, (0xFFFFF+(1<<31)), 0x186A0+(1<<31));

	set_htiu_enable_bits(nb_dev, 0x16, (0x3F<<10), 0x7<<10);

	set_htiu_enable_bits(nb_dev, 0x23, 0xFFFFFFF, 1<<28);

	set_htiu_enable_bits(nb_dev, 0x1E, 0xFFFFFFFF, 0xFFFFFFFF);
#endif
}

/*****************************************
* Configure RS780 registers to power-on default RPR.
* POR: Power On Reset
* RPR: Register Programming Requirements
*****************************************/
static void rs780_por_init(pci_devfn_t nb_dev)
{
	printk(BIOS_INFO, "rs780_por_init\n");
	/* ATINB_PCICFG_POR_TABLE, initialize the values for rs780 PCI Config registers */
	rs780_por_pcicfg_init(nb_dev);

	/* ATINB_MCIND_POR_TABLE */
	rs780_por_mc_index_init(nb_dev);

	/* ATINB_MISCIND_POR_TABLE */
	rs780_por_misc_index_init(nb_dev);

	/* ATINB_HTIUNBIND_POR_TABLE */
	rs780_por_htiu_index_init(nb_dev);

	/* ATINB_CLKCFG_PORT_TABLE */
	/* rs780 A11 SB Link full swing? */

	/* SET NB_MISC_REG01 BIT8 to Enable HDMI, reference CIMX_5_9_3 NBPOR_InitPOR(),
	 * then the accesses to internal graphics IO space 0x60/0x64, are forwarded to
	 * nbconfig:0x60/0x64
	 */

	set_nbmisc_enable_bits(nb_dev, 0x01, ~(1 << 8), (1 << 8));
}

/* enable CFG access to Dev8, which is the SB P2P Bridge */
void enable_rs780_dev8(void)
{
	set_nbmisc_enable_bits(PCI_DEV(0, 0, 0), 0x00, 1 << 6, 1 << 6);
}

void rs780_early_setup(void)
{
	pci_devfn_t nb_dev = PCI_DEV(0, 0, 0);
	printk(BIOS_INFO, "rs780_early_setup()\n");

	/* The printk(BIOS_INFO, s) below cause the system unstable. */
	switch (get_nb_rev(nb_dev)) {
	case REV_RS780_A11:
		/* printk(BIOS_INFO, "NB Revision is A11.\n"); */
		break;
	case REV_RS780_A12:
		/* printk(BIOS_INFO, "NB Revision is A12.\n"); */
		break;
	case REV_RS780_A13:
		/* printk(BIOS_INFO, "NB Revision is A13.\n"); */
		break;
	}

	if (is_famly10())
		fam10_optimization();
	else
		k8_optimization();

	rs780_por_init(nb_dev);
}
