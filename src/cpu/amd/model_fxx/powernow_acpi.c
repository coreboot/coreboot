/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
				u32 pcontrol_blk, u8 plen, u8 onlyBSP)  {
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
		u32 control, status;

		control =
			    (0x3 << 30) | /* IRT */
			    (0x2 << 28) | /* RVO */
			    (0x1 << 27) | /* ExtType */
			    (0x2 << 20) | /* PLL_LOCK_TIME */
			    (0x0 << 18) | /* MVS */
			    (0x5 << 11) | /* VST */
			    (pstate_vid[i] << 6) |
			    pstate_fid[i];
		status =
			    (pstate_vid[i] << 6) |
			    pstate_fid[i];

		lenp += acpigen_write_PSS_package(pstate_feq[i],
						pstate_power[i],
						0x64,
						0x7,
						control,
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
/*
* Details about this algorithm , refert to BDKG 10.5.1
* Two parts are included, the another is the DSDT reconstruction process
*/

static int pstates_algorithm(u32 pcontrol_blk, u8 plen, u8 onlyBSP)
{
	int len;
	u8 processor_brand[49];
	u32 *v;
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
	printk_info("processor_brand=%s\n", processor_brand);

	/*
	 * Based on the CPU socket type,cmp_cap and pwr_lmt , get the power limit.
	 * socket_type : 0x10 SocketF; 0x11 AM2/ASB1 ; 0x12 S1G1
	 * cmp_cap : 0x0 SingleCore ; 0x1 DualCore
	 */
	printk_info("Pstates Algorithm ...\n");
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
		printk_info("No valid set of P-states\n");
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
		printk_info("No intermediate P-states are supported\n");
		goto write_pstates;
	}

	/* Get the multipier of the fid frequency */
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

	/* Constuct P[Min] State */
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
		printk_info("Pstate_freq[%d] = %dMHz\t", index,
			    Pstate_feq[index]);
		printk_info("Pstate_vid[%d] = %d\t", index, Pstate_vid[index]);
		printk_info("Pstate_volt[%d] = %dmv\t", index,
			    Pstate_volt[index]);
		printk_info("Pstate_power[%d] = %dmw\n", index,
			    Pstate_power[index]);
	}


write_pstates:

	len = 0;

	for (index = 0; index < (cmp_cap + 1); index++) {
		len += write_pstates_for_core(Pstate_num, Pstate_feq, Pstate_vid,
				Pstate_fid, Pstate_power, index,
				pcontrol_blk, plen, onlyBSP);
	}

	return len;
}

int amd_model_fxx_generate_powernow(u32 pcontrol_blk, u8 plen, u8 onlyBSP) {
	int lens;
	char pscope[] = "\\_PR";

	lens = acpigen_write_scope(pscope);
	lens += pstates_algorithm(pcontrol_blk, plen, onlyBSP);
	//minus opcode
	acpigen_patch_len(lens - 1);
	return lens;
}
