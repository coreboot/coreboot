/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
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
#include <stdint.h>
#include <cpu/x86/msr.h>
#include <arch/acpigen.h>
#include <cpu/amd/model_fxx_powernow.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include <arch/cpu.h>

static int write_pstates_for_core(u8 pstate_num, u16 *pstate_feq, u8 *pstate_vid,
				u8 *pstate_fid, u32 *pstate_power, int coreID,
				u32 pcontrol_blk, u8 plen, u8 onlyBSP, u32 control)
{
	int lenp, lenpr, i;

	if ((onlyBSP) && (coreID != 0)) {
	    plen = 0;
	    pcontrol_blk = 0;
	}

	lenpr = acpigen_write_processor(coreID, pcontrol_blk, plen);
	lenpr += acpigen_write_empty_PCT();
	lenpr += acpigen_write_name("_PSS");

	/* add later to total sum */
	lenp = acpigen_write_package(pstate_num);

	for (i = 0;i < pstate_num;i++) {
		u32 status, c2;
		c2 = control | (pstate_vid[i] << 6) |
			    pstate_fid[i];
		status =
			    (pstate_vid[i] << 6) |
			    pstate_fid[i];

		lenp += acpigen_write_PSS_package(pstate_feq[i],
						pstate_power[i],
						0x64,
						0x7,
						c2,
						status);
	}
	/* update the package  size */
	acpigen_patch_len(lenp - 1);

	lenpr += lenp;
	lenpr += acpigen_write_PPC(pstate_num);
	/* patch the whole Processor token length */
	acpigen_patch_len(lenpr - 2);
	return lenpr;
}

#if CONFIG_K8_REV_F_SUPPORT
/*
* Details about this algorithm , refer to BDKG 10.5.1
* Two parts are included, the another is the DSDT reconstruction process
*/

static int pstates_algorithm(u32 pcontrol_blk, u8 plen, u8 onlyBSP)
{
	int len;
	u8 processor_brand[49];
	u32 *v, control;
	struct cpuid_result cpuid1;

	struct power_limit_encoding {
		u8 socket_type;
		u8 cmp_cap;
		u8 pwr_lmt;
		u32 power_limit;
	};
	u8 Max_fid, Max_vid, Start_fid, Start_vid, Min_fid, Min_vid;
	u16 Max_feq;
	u8 Pstate_fid[10];
	u16 Pstate_feq[10];
	u8 Pstate_vid[10];
	u32 Pstate_power[10];
	u32 Pstate_volt[10];
	u8 PstateStep, PstateStep_coef;
	u8 IntPstateSup;
	u8 Pstate_num;
	u16 Cur_feq;
	u8 Cur_fid;
	u8 cmp_cap, pwr_lmt;
	u32 power_limit = 0;
	u8 index;
	msr_t msr;
	u32 fid_multiplier;
	static struct power_limit_encoding TDP[20] = {
		{0x11, 0x0, 0x8, 62},
		{0x11, 0x1, 0x8, 89},
		{0x11, 0x1, 0xa, 103},
		{0x11, 0x1, 0xc, 125},
		{0x11, 0x0, 0x2, 15},
		{0x11, 0x0, 0x4, 35},
		{0x11, 0x1, 0x2, 35},
		{0x11, 0x0, 0x5, 45},
		{0x11, 0x1, 0x7, 76},
		{0x11, 0x1, 0x6, 65},
		{0x11, 0x1, 0x8, 89},
		{0x11, 0x0, 0x1, 8},
		{0x11, 0x1, 0x1, 22},
		{0x12, 0x0, 0x6, 25},
		{0x12, 0x0, 0x1, 8},
		{0x12, 0x0, 0x2, 9},
		{0x12, 0x0, 0x4, 15},
		{0x12, 0x0, 0xc, 35},
		{0x12, 0x1, 0xc, 35},
		{0x12, 0x1, 0x4, 20}
	};

	/* Get the Processor Brand String using cpuid(0x8000000x) command x=2,3,4 */
	cpuid1 = cpuid(0x80000002);
	v = (u32 *) processor_brand;
	v[0] = cpuid1.eax;
	v[1] = cpuid1.ebx;
	v[2] = cpuid1.ecx;
	v[3] = cpuid1.edx;
	cpuid1 = cpuid(0x80000003);
	v[4] = cpuid1.eax;
	v[5] = cpuid1.ebx;
	v[6] = cpuid1.ecx;
	v[7] = cpuid1.edx;
	cpuid1 = cpuid(0x80000004);
	v[8] = cpuid1.eax;
	v[9] = cpuid1.ebx;
	v[10] = cpuid1.ecx;
	v[11] = cpuid1.edx;
	processor_brand[48] = 0;
	printk(BIOS_INFO, "processor_brand=%s\n", processor_brand);

	/*
	 * Based on the CPU socket type,cmp_cap and pwr_lmt , get the power limit.
	 * socket_type : 0x10 SocketF; 0x11 AM2/ASB1 ; 0x12 S1G1
	 * cmp_cap : 0x0 SingleCore ; 0x1 DualCore
	 */
	printk(BIOS_INFO, "Pstates Algorithm ...\n");
	cmp_cap =
	    (pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xE8) &
	     0x3000) >> 12;
	cpuid1 = cpuid(0x80000001);
	pwr_lmt = ((cpuid1.ebx & 0x1C0) >> 5) | ((cpuid1.ebx & 0x4000) >> 14);
	for (index = 0; index <= sizeof(TDP) / sizeof(TDP[0]); index++)
		if (TDP[index].socket_type == CONFIG_CPU_SOCKET_TYPE &&
		    TDP[index].cmp_cap == cmp_cap &&
		    TDP[index].pwr_lmt == pwr_lmt) {
			power_limit = TDP[index].power_limit;
		}

	Pstate_num = 0;

	/* See if the CPUID(0x80000007) returned EDX[2:1]==11b */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) != 0x6) {
		printk(BIOS_INFO, "No valid set of P-states\n");
		goto write_pstates;
	}

	msr = rdmsr(0xc0010042);
	Max_fid = (msr.lo & 0x3F0000) >> 16;
	Start_fid = (msr.lo & 0x3F00) >> 8;
	Max_vid = (msr.hi & 0x3F0000) >> 16;
	Start_vid = (msr.hi & 0x3F00) >> 8;
	PstateStep = (msr.hi & 0x1000000) >> 24;
	IntPstateSup = (msr.hi & 0x20000000) >> 29;

	/*
	 * The P1...P[Min+1] VID need PstateStep to calculate
	 * P[N] = P[N-1]VID + 2^PstateStep
	 * PstateStep_coef = 2^PstateStep
	 */
	if (PstateStep == 0)
		PstateStep_coef = 1;
	else
		PstateStep_coef = 2;

	if (IntPstateSup == 0) {
		printk(BIOS_INFO, "No intermediate P-states are supported\n");
		goto write_pstates;
	}

	/* Get the multiplier of the fid frequency */
	/*
	 * Fid multiplier is always 100 revF and revG.
	 */
	fid_multiplier = 100;

	/*
	 * Formula1:    CPUFreq = FID * fid_multiplier + 800
	 * Formula2:       CPUVolt = 1550 - VID * 25 (mv)
	 * Formula3:       Power = (PwrLmt * P[N]Frequency*(P[N]Voltage^2))/(P[0]Frequency * P[0]Voltage^2))
	 */

	/* Construct P0(P[Max]) state */
	Max_feq = Max_fid * fid_multiplier + 800;
	if (Max_fid == 0x2A && Max_vid != 0x0) {
		Min_fid = 0x2;
		Pstate_fid[0] = Start_fid + 0xA;	/* Start Frequency + 1GHz */
		Pstate_feq[0] = Pstate_fid[0] * fid_multiplier + 800;
		Min_vid = Start_vid;
		Pstate_vid[0] = Max_vid + 0x2;	/* Maximum Voltage - 50mV */
		Pstate_volt[0] = 1550 - Pstate_vid[0] * 25;
		Pstate_power[0] = power_limit * 1000;	/* mw */
		Pstate_num++;
	} else {
		Min_fid = Start_fid;
		Pstate_fid[0] = Max_fid;
		Pstate_feq[0] = Max_feq;
		Min_vid = Start_vid;
		Pstate_vid[0] = Max_vid + 0x2;
		Pstate_volt[0] = 1550 - Pstate_vid[0] * 25;
		Pstate_power[0] = power_limit * 1000;	/* mw */
		Pstate_num++;
	}

	Cur_feq = Max_feq;
	Cur_fid = Max_fid;
	/* Construct P1 state */
	if (((Max_fid & 0x1) != 0) && ((Max_fid - 0x1) >= (Min_fid + 0x8))) {	/* odd value */
		Pstate_fid[1] = Max_fid - 0x1;
		Pstate_feq[1] = Pstate_fid[1] * fid_multiplier + 800;
		Cur_fid = Pstate_fid[1];
		Cur_feq = Pstate_feq[1];
		if (((Pstate_vid[0] & 0x1) != 0) && ((Pstate_vid[0] - 0x1) < Min_vid)) {	/* odd value */
			Pstate_vid[1] = Pstate_vid[0] + 0x1;
			Pstate_volt[1] = 1550 - Pstate_vid[1] * 25;
			Pstate_power[1] =
			    (unsigned long long)Pstate_power[0] *
			    Pstate_feq[1] * Pstate_volt[1] * Pstate_volt[1] /
			    (Pstate_feq[0] * Pstate_volt[0] * Pstate_volt[0]);
		}
		if (((Pstate_vid[0] & 0x1) == 0) && ((Pstate_vid[0] - 0x1) < Min_vid)) {	/* even value */
			Pstate_vid[1] = Pstate_vid[0] + PstateStep_coef;
			Pstate_volt[1] = 1550 - Pstate_vid[1] * 25;
			Pstate_power[1] =
			    (unsigned long long)Pstate_power[0] *
			    Pstate_feq[1] * Pstate_volt[1] * Pstate_volt[1] /
			    (Pstate_feq[0] * Pstate_volt[0] * Pstate_volt[0]);
		}
		Pstate_num++;
	}

	if (((Max_fid & 0x1) == 0) && ((Max_fid - 0x2) >= (Min_fid + 0x8))) {	/* even value */
		Pstate_fid[1] = Max_fid - 0x2;
		Pstate_feq[1] = Pstate_fid[1] * fid_multiplier + 800;
		Cur_fid = Pstate_fid[1];
		Cur_feq = Pstate_feq[1];
		if (((Pstate_vid[0] & 0x1) != 0) && ((Pstate_vid[0] - 0x1) < Min_vid)) {	/* odd value */
			Pstate_vid[1] = Pstate_vid[0] + 0x1;
			Pstate_volt[1] = 1550 - Pstate_vid[1] * 25;
			Pstate_power[1] =
			    (unsigned long long)Pstate_power[0] *
			    Pstate_feq[1] * Pstate_volt[1] * Pstate_volt[1] /
			    (Pstate_feq[0] * Pstate_volt[0] * Pstate_volt[0]);
		}
		if (((Pstate_vid[0] & 0x1) == 0) && ((Pstate_vid[0] - 0x1) < Min_vid)) {	/* even value */
			Pstate_vid[1] = Pstate_vid[0] + PstateStep_coef;
			Pstate_volt[1] = 1550 - Pstate_vid[1] * 25;
			Pstate_power[1] =
			    (unsigned long long)Pstate_power[0] *
			    Pstate_feq[1] * Pstate_volt[1] * Pstate_volt[1] /
			    (Pstate_feq[0] * Pstate_volt[0] * Pstate_volt[0]);
		}

		Pstate_num++;
	}

	/* Construct P2...P[Min-1] state */
	Cur_fid = Cur_fid - 0x2;
	Cur_feq = Cur_fid * fid_multiplier + 800;
	while (Cur_feq >= ((Min_fid * fid_multiplier) + 800) * 2) {
		Pstate_fid[Pstate_num] = Cur_fid;
		Pstate_feq[Pstate_num] =
		    Pstate_fid[Pstate_num] * fid_multiplier + 800;
		Cur_fid = Cur_fid - 0x2;
		Cur_feq = Cur_fid * fid_multiplier + 800;
		if (Pstate_vid[Pstate_num - 1] >= Min_vid) {
			Pstate_vid[Pstate_num] = Pstate_vid[Pstate_num - 1];
			Pstate_volt[Pstate_num] = Pstate_volt[Pstate_num - 1];
			Pstate_power[Pstate_num] = Pstate_power[Pstate_num - 1];
		} else {
			Pstate_vid[Pstate_num] =
			    Pstate_vid[Pstate_num - 1] + PstateStep_coef;
			Pstate_volt[Pstate_num] =
			    1550 - Pstate_vid[Pstate_num] * 25;
			Pstate_power[Pstate_num] =
			    (unsigned long long)Pstate_power[0] *
			    Pstate_feq[Pstate_num] * Pstate_volt[Pstate_num] *
			    Pstate_volt[Pstate_num] / (Pstate_feq[0] *
						       Pstate_volt[0] *
						       Pstate_volt[0]);
		}
		Pstate_num++;
	}

	/* Construct P[Min] State */
	if (Max_fid == 0x2A && Max_vid != 0x0) {
		Pstate_fid[Pstate_num] = 0x2;
		Pstate_feq[Pstate_num] =
		    Pstate_fid[Pstate_num] * fid_multiplier + 800;
		Pstate_vid[Pstate_num] = Min_vid;
		Pstate_volt[Pstate_num] = 1550 - Pstate_vid[Pstate_num] * 25;
		Pstate_power[Pstate_num] =
		    (unsigned long long)Pstate_power[0] *
		    Pstate_feq[Pstate_num] * Pstate_volt[Pstate_num] *
		    Pstate_volt[Pstate_num] / (Pstate_feq[0] * Pstate_volt[0] *
					       Pstate_volt[0]);
		Pstate_num++;
	} else {
		Pstate_fid[Pstate_num] = Start_fid;
		Pstate_feq[Pstate_num] =
		    Pstate_fid[Pstate_num] * fid_multiplier + 800;
		Pstate_vid[Pstate_num] = Min_vid;
		Pstate_volt[Pstate_num] = 1550 - Pstate_vid[Pstate_num] * 25;
		Pstate_power[Pstate_num] =
		    (unsigned long long)Pstate_power[0] *
		    Pstate_feq[Pstate_num] * Pstate_volt[Pstate_num] *
		    Pstate_volt[Pstate_num] / (Pstate_feq[0] * Pstate_volt[0] *
					       Pstate_volt[0]);
		Pstate_num++;
	}

	/* Print Pstate freq,vid,volt,power */

	for (index = 0; index < Pstate_num; index++) {
		printk(BIOS_INFO, "Pstate_freq[%d] = %dMHz\t", index,
			    Pstate_feq[index]);
		printk(BIOS_INFO, "Pstate_vid[%d] = %d\t", index, Pstate_vid[index]);
		printk(BIOS_INFO, "Pstate_volt[%d] = %dmv\t", index,
			    Pstate_volt[index]);
		printk(BIOS_INFO, "Pstate_power[%d] = %dmw\n", index,
			    Pstate_power[index]);
	}


write_pstates:

	len = 0;

	control = (0x3 << 30) | /* IRT */
		  (0x2 << 28) | /* RVO */
		  (0x1 << 27) | /* ExtType */
		  (0x2 << 20) | /* PLL_LOCK_TIME */
		  (0x0 << 18) | /* MVS */
		  (0x5 << 11); /* VST */

	for (index = 0; index < (cmp_cap + 1); index++) {
		len += write_pstates_for_core(Pstate_num, Pstate_feq, Pstate_vid,
				Pstate_fid, Pstate_power, index,
				pcontrol_blk, plen, onlyBSP, control);
	}

	return len;
}

#else


static uint8_t vid_to_reg(uint32_t vid)
{
	return (1550 - vid) / 25;
}

static uint32_t vid_from_reg(uint8_t val)
{
	return (val == 0x1f ? 0 : 1550 - val * 25);
}

static uint8_t freq_to_fid(uint32_t freq)
{
	return (freq - 800) / 100;
}
/* Return a frequency in MHz, given an input fid */
static uint32_t fid_to_freq(uint32_t fid)
{
	return 800 + (fid * 100);
}

#define MAXP 7

struct pstate {
	uint16_t freqMhz; /* in MHz */
	uint16_t voltage; /* in mV */
	uint16_t tdp; /* in W * 10 */
};

struct cpuentry {
	uint16_t modelnr; /* numeric model value, unused in code */
	uint8_t brandID; /* CPUID 8000_0001h EBX [11:6] (BrandID) */
	uint32_t cpuid; /* CPUID 8000_0001h EAX [31:0] (CPUID) */
	uint8_t maxFID; /* FID/VID Status MaxFID Field */
	uint8_t startFID; /* FID/VID Status StartFID Field */
	uint16_t pwr:12; /* Thermal Design Power of Max P-State  *10 (fixed point) */
	/* Other MAX P state are read from CPU, other P states in following table */
	struct pstate pstates[MAXP];
};

struct cpuentry entr[] = {
	/* rev E single core, check OSA152FAA5BK */
	{152, 0xc, 0x20f51, 0x12, 0x12, 926,
	 {{2400, 1350, 900}, {2200, 1300, 766},
	  {2000, 1250, 651}, {1800, 1200, 522},
	  {1000, 1100, 320}}},
	{252, 0x10, 0x20f51, 0x12, 0x12, 926,
	 {{2400, 1350, 900}, {2200, 1300, 766},
	  {2000, 1250, 651}, {1800, 1200, 522},
	  {1000, 1100, 320}}},
	{852, 0x14, 0x20f51, 0x12, 0x12, 926,
	 {{2400, 1350, 900}, {2200, 1300, 766},
	  {2000, 1250, 651}, {1800, 1200, 522},
	  {1000, 1100, 320}}},
	{254, 0x10, 0x20f51, 0x14, 0x14, 926,
	 {{2600, 1350, 902}, {2400, 1300, 770},
	  {2200, 1250, 657}, {2000, 1200, 559},
	  {1800, 1150, 476}, {1000, 1100, 361}}},
	{854, 0x14, 0x20f51, 0x14, 0x14, 926,
	 {{2600, 1350, 902}, {2400, 1300, 770},
	  {2200, 1250, 657}, {2000, 1200, 559},
	  {1800, 1150, 476}, {1000, 1100, 361}}},
	{242, 0x10, 0x20f51, 0x8, 0x8, 853,
	 {}},
	{842, 0x10, 0x20f51, 0x8, 0x8, 853,
	 {}},
	{244, 0x10, 0x20f51, 0xa, 0xa, 853,
	 {{1000, 1100, 378}}},
	{844, 0x14, 0x20f51, 0xa, 0xa, 853,
	 {{1000, 1100, 378}}},
	{246, 0x10, 0x20f51, 0xc, 0xc, 853,
	 {{1800, 1350, 853},
	 {1000, 1100, 378}}},
	{846, 0x14, 0x20f51, 0xc, 0xc, 853,
	 {{1800, 1350, 853},
	 {1000, 1100, 378}}},
	{242, 0x10, 0x20f51, 0x8, 0x8, 853,
	 {}},
	{842, 0x14, 0x20f51, 0x8, 0x8, 853,
	 {}},
	{244, 0x10, 0x20f51, 0xa, 0xa, 853,
	 {{1000, 1100, 378}}},
	{844, 0x14, 0x20f51, 0xa, 0xa, 853,
	 {{1000, 1100, 378}}},
	{246, 0x10, 0x20f51, 0xc, 0xc, 853,
	 {{1800, 1350, 827}, {1000, 1100, 366}}},
	{846, 0x14, 0x20f51, 0xc, 0xc, 853,
	 {{1800, 1350, 827}, {1000, 1100, 366}}},
	{248, 0x10, 0x20f51, 0xe, 0xe, 853,
	 {{2000, 1350, 827}, {1800, 1300, 700},
	  {1000, 1100, 366}}},
	{848, 0x14, 0x20f51, 0xe, 0xe, 853,
	 {{2000, 1350, 827}, {1800, 1300, 700},
	  {1000, 1100, 366}}},
	{250, 0x10, 0x20f51, 0x10, 0x10, 853,
	 {{2200, 1350, 853}, {2000, 1300, 827},
	  {1800, 1250, 702}, {1000, 1100, 301}}},
	{850, 0x14, 0x20f51, 0x10, 0x10, 853,
	 {{2200, 1350, 853}, {2000, 1300, 827},
	  {1800, 1250, 702}, {1000, 1100, 301}}},
/* begin OSK246FAA5BL */
	{246, 0x12, 0x20f51, 0xc, 0xc, 547,
	 {{1800, 1350, 461}, {1000, 1100, 223}}},
	{846, 0x16, 0x20f51, 0xc, 0xc, 547,
	 {{1800, 1350, 461}, {1000, 1100, 223}}},
	{148, 0xe, 0x20f51, 0xe, 0xe, 547,
	 {{2000, 1350, 521}, {1800, 1300, 459},
	  {1000, 1100, 211}}},
	{248, 0x12, 0x20f51, 0xe, 0xe, 547,
	 {{2000, 1350, 521}, {1800, 1300, 459},
	  {1000, 1100, 211}}},
	{848, 0x16, 0x20f51, 0xe, 0xe, 547,
	 {{2000, 1350, 521}, {1800, 1300, 459},
	  {1000, 1100, 211}}},
	{250, 0x12, 0x20f51, 0x10, 0x10, 547,
	 {{2200, 1350, 521}, {2000, 1300, 440},
	  {1800, 1250, 379}, {1000, 1100, 199}}},
	{850, 0x16, 0x20f51, 0x10, 0x10, 547,
	 {{2200, 1350, 521}, {2000, 1300, 440},
	  {1800, 1250, 379}, {1000, 1100, 199}}},
	{144, 0xc, 0x20f71, 0xa, 0xa, 670,
	 {{1000, 1100, 296}}},
	{148, 0xc, 0x20f71, 0xe, 0xe, 853,
	 {{2000, 1350, 830}, {1800, 1300, 704},
	 {1000, 1100, 296}}},
	{152, 0xc, 0x20f71, 0x12, 0x12, 104,
	 {{2400, 1350, 1016}, {2200, 1300, 863},
	 {2000, 1250, 732}, {1800, 1200, 621},
	  {1000, 1100, 419}}},
	{146, 0xc, 0x20f71, 0xc, 0xc, 670,
	 {{1800, 1350, 647}, {1000, 1100, 286}}},
	{150, 0xc, 0x20f71, 0x10, 0x10, 853,
	{{2200, 1350, 830}, {2000, 1300, 706},
	{1800, 1250, 596}, {1000, 1100, 350}}},
	{154, 0xc, 0x20f71, 0x14, 0x14, 1040,
	{{2600, 1350, 1017}, {2400, 1300, 868},
	{2200, 1250, 740}, {2000, 1200, 630},
	{1800, 1150, 537}, {1000, 1100, 416}}},
	/* rev E dualcore */
	{165, 0x2c, 0x20f12, 0xa, 0xa, 950,
	 {{1000, 1100, 406}}},
	{265, 0x30, 0x20f12, 0xa, 0xa, 950,
	 {{1000, 1100, 406}}},
	{865, 0x34, 0x20f12, 0xa, 0xa, 950,
	 {{1000, 1100, 406}}},
	{270, 0x30, 0x20f12, 0xc, 0xc, 950,
	 {{1800, 1300, 903}, {1000, 1100, 383}}},
	{870, 0x34, 0x20f12, 0xc, 0xc, 950,
	 {{1800, 1300, 903}, {1000, 1100, 383}}},
	{275, 0x30, 0x20f12, 0xe, 0xe, 950,
	 {{2000, 1300, 903}, {1800, 1250, 759},
	 {1000, 1100, 361}}},
	{875, 0x34, 0x20f12, 0xe, 0xe, 950,
	 {{2000, 1300, 903}, {1800, 1250, 759},
	 {1000, 1100, 361}}},
	{280, 0x30, 0x20f12, 0x10, 0x10, 926,
	 {{2400, 1350, 900}, {2200, 1300, 766},
	 {1800, 1200, 552}, {1000, 1100, 320}}},
	{880, 0x34, 0x20f12, 0x10, 0x10, 926,
	 {{2400, 1350, 900}, {2200, 1300, 766},
	 {1800, 1200, 552}, {1000, 1100, 320}}},
	{170, 0x2c, 0x20f32, 0xc, 0xc, 1100,
	 {{1800, 1300, 1056}, {1000, 1100, 514}}},
	{175, 0x2c, 0x20f32, 0xe, 0xe, 1100,
	 {{2000, 1300, 1056}, {1800, 1250, 891},
	  {1000, 1100, 490}}},
	{260, 0x32, 0x20f32, 0x8, 0x8, 550,
	 {}},
	{860, 0x36, 0x20f32, 0x8, 0x8, 550,
	 {}},
	{165, 0x2e, 0x20f32, 0xa, 0xa, 550,
	 {{1000, 1100, 365}}},
	{265, 0x32, 0x20f32, 0xa, 0xa, 550,
	 {{1000, 1100, 365}}},
	{865, 0x36, 0x20f32, 0xa, 0xa, 550,
	 {{1000, 1100, 365}}},
	{270, 0x32, 0x20f12, 0xc, 0xc, 550,
	 {{1800, 1150, 520}, {1000, 1100, 335}}},
	{870, 0x36, 0x20f12, 0xc, 0xc, 550,
	 {{1800, 1150, 520}, {1000, 1100, 335}}},
	{180, 0x2c, 0x20f32, 0x10, 0x10, 1100,
	 {{2200, 1300, 1056}, {2000, 1250, 891},
	  {1800, 1200, 748}, {1000, 1100, 466}}},
	/* AMA3000BEX5AR */
	{3000, 0x4, 0xf4a, 0xa, 0x0, 815,
	 {{1600, 1400, 570}, {800, 1100, 190}}},
	/* TMDML34BKX5LD, needs real TDP info */
	{34, 0xa, 0x20f42, 0xa, 0x0, 350,
	 {{1600, 1400, 340}, {800, 1000, 330}}},
	/* ADA3200AIO4BX */
	{3200, 0x4, 0x20fc2, 0xe, 0xe, 670,
	 {{2000, 1350, 647}, {1800, 1300, 548}, {1000, 1100, 275}}},
	/* ADA2800AEP4AP */
	{2800, 0x4, 0xf48, 0xa, 0xa, 890,
	 {{800, 1300, 350}}},
	/* ADA3000AEP4AP */
	{3000, 0x4, 0xf48, 0xc, 0xc, 890,
	 {{1800, 1400, 660}, {800, 1300, 350}}},
	/* ADA3200AEP5AP */
	{3200, 0x4, 0xf48, 0xc, 0xc, 890,
	 {{1800, 1400, 660}, {800, 1300, 350}}},
	/* ADA3400AEP5AP */
	{3400, 0x4, 0xf48, 0xe, 0xe, 890,
	 {{2000, 1400, 700}, {800, 1300, 350}}},
	/* ADA2800AEP4AR */
	{2800, 0x4, 0xf4a, 0xa, 0xa, 890,
	 {{1000, 1100, 220}}},
	/* ADA3000AEP4AR */
	{3000, 0x4, 0xf4a, 0xc, 0xc, 890,
	 {{1800, 1400, 660}, {1000, 1100, 220}}},
	/* ADA3700AEP5AR */
	{3700, 0x4, 0xf4a, 0x10, 0x10, 890,
	 {{2200, 1400, 720}, {2000, 1300, 530}, {1800, 1200, 390}, {1000, 1100, 220}}},
	/* ADA2800AEP4AX */
	{2800, 0x4, 0xfc0, 0xa, 0xa, 890,
	 {{1000, 1100, 220}}},
	/* ADA3000AEP4AX */
	{3000, 0x4, 0xfc0, 0xc, 0xc, 890,
	 {{1800, 1400, 670}, {1000, 1100, 220}}},
	/* ADA3200AEP4AX */
	{3200, 0x4, 0xfc0, 0xe, 0xe, 890,
	 {{2000, 1400, 690}, {1800, 1300, 500}, {1000, 1100, 220}}},
	/* ADA3400AEP4AX */
	{3400, 0x4, 0xfc0, 0x10, 0x10, 890,
	 {{2200, 1400, 720}, {2000, 1300, 530}, {1800, 1200, 390}, {1000, 1100, 220}}},
	/* ADA3500DEP4AS */
	{3500, 0x4, 0xf7a, 0xe, 0xe, 890,
	 {{2000, 1400, 690}, {1800, 1300, 500}, {1000, 1100, 220}}},
	/* ADA3500DEP4AW */
	{3500, 0x4, 0xff0, 0xe, 0xe, 890,
	 {{2000, 1400, 690}, {1800, 1300, 500}, {1000, 1100, 220}}},
	/* ADA3800DEP4AW */
	{3800, 0x4, 0xff0, 0x10, 0x10, 890,
	 {{2200, 1400, 720}, {2000, 1300, 530}, {1800, 1200, 390}, {1000, 1100, 220}}},
	/* ADA4000DEP5AS */
	{4000, 0x4, 0xf7a, 0x10, 0x10, 890,
	 {{2200, 1400, 720}, {2000, 1300, 530}, {1800, 1200, 390}, {1000, 1100, 220}}},
	/* ADA3500DAA4BN */
	{3500, 0x4, 0x20f71, 0xe, 0xe, 670,
	 {{2000, 1350, 647}, {1800, 1300, 548}, {1000, 1100, 275}}},
	/* ADA3700DAA5BN */
	{3700, 0x4, 0x20f71, 0xe, 0xe, 853,
	 {{2000, 1350, 830}, {1800, 1300, 704}, {1000, 1100, 361}}},
	/* ADA4000DAA5BN */
	{4000, 0x4, 0x20f71, 0x10, 0x10, 853,
	 {{2200, 1350, 830}, {2000, 1300, 706}, {1800, 1250, 596}, {1000, 1100, 350}}},
	/* ADA3700DKA5CF */
	{3700, 0x4, 0x30f72, 0xe, 0xe, 853,
	 {{2000, 1350, 830}, {1800, 1300, 704}, {1000, 1100, 361}}},
	/* ADA4000DKA5CF */
	{4000, 0x4, 0x30f72, 0x10, 0x10, 853,
	 {{2200, 1350, 830}, {2000, 1300, 706}, {1800, 1250, 596}, {1000, 1100, 350}}},
	/* ADA3800DAA4BP */
	{3800, 0x4, 0x20ff0, 0x10, 0x10, 853,
	 {{2200, 1350, 830}, {2000, 1300, 706}, {1800, 1250, 596}, {1000, 1100, 350}}},
	/* ADA3000DIK4BI */
	{3000, 0x4, 0x10ff0, 0xa, 0xa, 670,
	 {{1000, 1100, 210}}},
	/* ADA3200DIK4BI */
	{3200, 0x4, 0x10ff0, 0xc, 0xc, 670,
	 {{1800, 1350, 560}, {1000, 1100, 210}}},
	/* ADA3500DIK4BI */
	{3500, 0x4, 0x10ff0, 0xe, 0xe, 670,
	 {{2000, 1350, 560}, {1800, 1300, 460}, {1000, 1100, 200}}},
	/* ADA3000DAA4BP */
	{3000, 0x4, 0x20ff0, 0xa, 0xa, 670,
	 {{1000, 1100, 296}}},
	/* ADA3200DAA4BP */
	{3200, 0x4, 0x20ff0, 0xc, 0xc, 670,
	 {{1800, 1350, 647}, {1000, 1100, 286}}},
	/* ADA3500DAA4BP */
	{3500, 0x4, 0x20ff0, 0xe, 0xe, 670,
	 {{2000, 1350, 647}, {1800, 1300, 548}, {1000, 1100, 275}}},
	/* ADA3000DAA4BW */
	{3000, 0x4, 0x20ff2, 0xa, 0xa, 670,
	 {{1000, 1100, 296}}},
	/* ADA3200DAA4BW */
	{3200, 0x4, 0x20ff2, 0xc, 0xc, 670,
	 {{1800, 1350, 647}, {1000, 1100, 286}}},
	/* ADA3500DAA4BW */
	{3500, 0x4, 0x20ff2, 0xe, 0xe, 670,
	 {{2000, 1350, 647}, {1800, 1300, 548}, {1000, 1100, 275}}},
	/* ADA3200DKA4CG */
	{3200, 0x4, 0x30ff2, 0xc, 0xc, 670,
	 {{1800, 1350, 647}, {1000, 1100, 286}}},
	/* ADA3800DAA4BW */
	{3800, 0x4, 0x20ff2, 0x10, 0x10, 853,
	 {{2200, 1350, 830}, {2000, 1300, 706}, {1800, 1250, 596}, {1000, 1100, 350}}},
	/* ADA3000AIK4BX */
	{3000, 0x4, 0x20fc2, 0xc, 0xc, 510,
	 {{1800, 1350, 428}, {1000, 1100, 189}}},
	/* ADAFX53DEP5AS */
	{53, 0x24, 0xf7a, 0x2a, 0x10, 890,
	 {{1200, 1100, 250}}},
	/* ADAFX55DEI5AS */
	{55, 0x24, 0xf7a, 0x2a, 0x12, 1040,
	 {{1200, 1100, 250}}},
	/* ADAFX55DAA5BN */
	{55, 0x24, 0x20f71, 0x2a, 0x12, 1040,
	 {{1200, 1100, 422}}},
	/* ADAFX57DAA5BN */
	{57, 0x24, 0x20f71, 0x2a, 0x14, 1040,
	 {{1200, 1100, 434}}},
	/* SDA3100AIP3AX */
	{3100, 0x22, 0xfc0, 0xa, 0xa, 620,
	 {{1000, 1100, 200}}},
	/* SDA2600AIO2BA */
	{2600, 0x22, 0x10fc0, 0x8, 0x8, 590,
	 {}},
	/* SDA2800AIO3BA */
	{2800, 0x22, 0x10fc0, 0x8, 0x8, 590,
	 {}},
	/* SDA3000AIO2BA */
	{3000, 0x22, 0x10fc0, 0xa, 0xa, 590,
	 {{1000, 1100, 190}}},
	/* SDA3100AIO3BA */
	{3100, 0x22, 0x10fc0, 0xa, 0xa, 590,
	 {{1000, 1100, 190}}},
	/* SDA3300AIO2BA */
	{3300, 0x22, 0x10fc0, 0xc, 0xc, 590,
	 {{1800, 1350, 488}, {1000, 1100, 180}}},
	/* SDA2500AIO3BX */
	{2500, 0x26, 0x20fc2, 0x6, 0x6, 590,
	 {}},
	/* SDA2600AIO2BX */
	{2600, 0x26, 0x20fc2, 0x8, 0x8, 590,
	 {}},
	/* SDA2800AIO3BX */
	{2800, 0x26, 0x20fc2, 0x8, 0x8, 590,
	 {}},
	/* SDA3000AIO2BX */
	{3000, 0x26, 0x20fc2, 0xa, 0xa, 590,
	 {{1000, 1100, 217}}},
	/* SDA3100AIO3BX */
	{3100, 0x26, 0x20fc2, 0xa, 0xa, 590,
	 {{1000, 1100, 217}}},
	/* SDA3300AIO2BX */
	{3300, 0x26, 0x20fc2, 0xc, 0xc, 590,
	 {{1800, 1350, 496}, {1000, 1100, 207}}},
	/* SDA3400AIO3BX */
	{3400, 0x26, 0x20fc2, 0xc, 0xc, 590,
	 {{1800, 1350, 496}, {1000, 1100, 207}}},
	/* TMSMT32BQX4LD */
	{32, 0xb, 0x20f42, 0xa, 0x0, 240,
	 {{1600, 1150, 199}, {800, 900, 77}}},
	/* TMSMT34BQX5LD */
	{34, 0xb, 0x20f42, 0xa, 0x0, 240,
	 {{1600, 1150, 199}, {800, 900, 79}}},
	/* TMSMT37BQX5LD */
	{37, 0xb, 0x20f42, 0xc, 0x0, 250,
	 {{1800, 1150, 209}, {1600, 1100, 175}, {800, 900, 79}}},
	/* ADA4400DAA6CD */
	{4400, 0x5, 0x20f32, 0xe, 0xe, 1100,
	 {{2000, 1300, 1056}, {1800, 1250, 891}, {1000, 1100, 490}}},
	/* ADA4800DAA6CD */
	{4800, 0x5, 0x20f32, 0x10, 0x10, 1100,
	 {{2200, 1300, 1056}, {2000, 1250, 891}, {1800, 1200, 748}, {1000, 1100, 466}}},
	/* ADA3800DAA5BV */
	{3800, 0x5, 0x20fb1, 0xc, 0xc, 890,
	 {{1800, 1300, 846}, {1000, 1100, 401}}},
	/* ADA4200DAA5BV */
	{4200, 0x5, 0x20fb1, 0xe, 0xe, 890,
	 {{2000, 1300, 846}, {1800, 1250, 709}, {1000, 1100, 376}}},
	/* ADA4600DAA5BV */
	{4600, 0x5, 0x20fb1, 0x10, 0x10, 1100,
	 {{2200, 1300, 1056}, {2000, 1250, 891}, {1800, 1200, 748}, {1000, 1100, 466}}},
};

static int pstates_algorithm(u32 pcontrol_blk, u8 plen, u8 onlyBSP)
{

	u8 cmp_cap;
	struct cpuentry *data = NULL;
	uint32_t control;
	int i = 0, index = 0, len = 0, Pstate_num = 0, dev = 0;
	msr_t msr;
	u8 Pstate_fid[MAXP+1];
	u16 Pstate_feq[MAXP+1];
	u8 Pstate_vid[MAXP+1];
	u32 Pstate_power[MAXP+1];
	u8 Max_fid, Start_fid, Start_vid, Max_vid;
	struct cpuid_result cpuid1;

	/* See if the CPUID(0x80000007) returned EDX[2:1]==11b */
	cpuid1 = cpuid(0x80000007);
	if((cpuid1.edx & 0x6)!=0x6) {
		printk(BIOS_INFO, "Processor not capable of performing P-state transitions\n");
		return 0;
	}

	cpuid1 = cpuid(0x80000001);

	/* It has been said that we can safely assume that all CPU's
	 * in the system have the same SYSCONF values
	 */
	msr = rdmsr(0xc0010042);
	Max_fid = (msr.lo & 0x3F0000) >> 16;
	Max_vid = (msr.hi & 0x3F0000) >> 16;
	Start_fid = (msr.lo & 0x3F00) >> 8;
	Start_vid = (msr.hi & 0x3F00) >> 8;

	cmp_cap =
	    (pci_read_config16(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xE8) &
	     0x3000) >> 12;

	for (i = 0; i < ARRAY_SIZE(entr); i++) {
		if ((entr[i].cpuid == cpuid1.eax)
		    && (entr[i].startFID == Start_fid)
		    && (entr[i].maxFID == Max_fid)
		    && (entr[i].brandID == ((u8 )((cpuid1.ebx >> 6) & 0xff)))) {
			data = &entr[i];
			break;
		}
	}

	if (data == NULL) {
		printk(BIOS_WARNING, "Unknown CPU, please update the powernow_acpi.c\n");
		return 0;
	}

#if CONFIG_MAX_PHYSICAL_CPUS
	/* IRT 80us RVO = 50mV PLL_LOCK_TIME 2us, MVS 25mv, VST 100us */
	control = (3 << 30) | (2 << 28) | (2 << 20) | (0 << 18) | (5 << 11);
#else
	/* MP-systems should default to RVO=0mV (no ramp voltage) */
	/* IRT 80us RVO = 0mV PLL_LOCK_TIME 2us, MVS 25mv, VST 100us */

	control = (3 << 30) | (0 << 28) | (2 << 20) | (0 << 18) | (5 << 11);
#endif
	/* RVO (Ramp Voltage Offset)
	 *   00   0mV (default for MP-systems)
	 *   01  25mV
	 *   10  50mV (default)
	 *   11  75mV
	 * IRT (Isochronous Release Time)
	 *   00  10uS
	 *   01  20uS
	 *   10  40uS
	 *   11  80uS (default)
	 * MVS (Maximum Voltage Step)
	 *   00  25mV (default)
	 *   01  50mV (reserved)
	 *   10 100mV (reserved)
	 *   11 200mV (reserved)
	 * VST (Voltage Stabilization Time)
	 *   time = value*20uS  (default value: 5 => 100uS)
	 * PLL_LOCK_TIME
	 *   time = value*1uS (often seen value: 2uS)
	 */

	len = 0;

	Pstate_fid[0] = Max_fid;
	Pstate_feq[0] = fid_to_freq(Max_fid);
	Pstate_vid[0] = Max_vid;
	Pstate_power[0] = data->pwr * 100;

	for(Pstate_num = 1;
	    (Pstate_num <= MAXP) && (data->pstates[Pstate_num - 1].freqMhz != 0);
	    Pstate_num++) {
		Pstate_fid[Pstate_num] = freq_to_fid(data->pstates[Pstate_num - 1].freqMhz) & 0x3f;
		Pstate_feq[Pstate_num] = data->pstates[Pstate_num - 1].freqMhz;
		Pstate_vid[Pstate_num] = vid_to_reg(data->pstates[Pstate_num - 1].voltage);
		Pstate_power[Pstate_num] = data->pstates[Pstate_num - 1].tdp * 100;
	}

	for (i=0;i<Pstate_num;i++)
		printk(BIOS_DEBUG, "P#%d freq %d [MHz] voltage %d [mV] TDP %d [mW]\n", i,
		       Pstate_feq[i],
		       vid_from_reg(Pstate_vid[i]),
		       Pstate_power[i]);

	/* Loop over all CPU's */
	for (dev = 0x18; dev < 0x1c; dev++) {
		if(dev_find_slot(0, PCI_DEVFN(dev, 0)) == NULL)
			continue;

		for (i = 0; i < (cmp_cap + 1); i++) {
			len += write_pstates_for_core(Pstate_num, Pstate_feq, Pstate_vid,
					Pstate_fid, Pstate_power, index+i,
					pcontrol_blk, plen, onlyBSP, control);
		}
		index += i;
	}
	printk(BIOS_DEBUG,"%d Processor objects emitted to SSDT\n",index);

	return len;
}

#endif


int amd_model_fxx_generate_powernow(u32 pcontrol_blk, u8 plen, u8 onlyBSP)
{
	int lens;
	char pscope[] = "\\_PR";

	lens = acpigen_write_scope(pscope);
	lens += pstates_algorithm(pcontrol_blk, plen, onlyBSP);
	//minus opcode
	acpigen_patch_len(lens - 1);
	return lens;
}

