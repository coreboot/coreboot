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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <string.h>
#include <arch/acpi.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdk8_sysconf.h>
#include <../../../northbridge/amd/amdk8/amdk8_acpi.h>
#include <arch/cpu.h>

#define DUMP_ACPI_TABLES 0

/*
* Assume the max pstate number is 8
* 0x21(33 bytes) is one package length of _PSS package
*/

#define Maxpstate 8
#define Defpkglength 0x21

#if DUMP_ACPI_TABLES == 1
static void dump_mem(u32 start, u32 end)
{

	u32 i;
	print_debug("dump_mem:");
	for (i = start; i < end; i++) {
		if ((i & 0xf) == 0) {
			printk_debug("\n%08x:", i);
		}
		printk_debug(" %02x", (u8)*((u8 *)i));
	}
	print_debug("\n");
}
#endif

extern u8 AmlCode[];
extern u8 AmlCode_ssdt[];

#if ACPI_SSDTX_NUM >= 1
extern u8 AmlCode_ssdt2[];
extern u8 AmlCode_ssdt3[];
extern u8 AmlCode_ssdt4[];
extern u8 AmlCode_ssdt5[];
#endif

#define IO_APIC_ADDR	0xfec00000UL

unsigned long acpi_fill_mcfg(unsigned long current)
{
	/* Just a dummy */
	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* create all subtables for processors */
	current = acpi_create_madt_lapics(current);

	/* Write SB600 IOAPIC, only one */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current, 2,
					   IO_APIC_ADDR, 0);

	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
						current, 0, 9, 9, 0xF);
	/* 0: mean bus 0--->ISA */
	/* 0: PIC 0 */
	/* 2: APIC 2 */
	/* 5 mean: 0101 --> Edige-triggered, Active high */

	/* create all subtables for processors */
	/* current = acpi_create_madt_lapic_nmis(current, 5, 1); */
	/* 1: LINT1 connect to NMI */

	return current;
}

extern void get_bus_conf(void);

void update_ssdtx(void *ssdtx, int i)
{
	uint8_t *PCI;
	uint8_t *HCIN;
	uint8_t *UID;

	PCI = ssdtx + 0x32;
	HCIN = ssdtx + 0x39;
	UID = ssdtx + 0x40;

	if (i < 7) {
		*PCI = (uint8_t) ('4' + i - 1);
	} else {
		*PCI = (uint8_t) ('A' + i - 1 - 6);
	}
	*HCIN = (uint8_t) i;
	*UID = (uint8_t) (i + 3);

	/* FIXME: need to update the GSI id in the ssdtx too */

}

/*
* Details about this algorithm , refert to BDKG 10.5.1
* Two parts are included, the another is the DSDT reconstruction process
*/
u32 pstates_algorithm(acpi_header_t * dsdt)
{
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
	u32 i, j;
	u32 processor_length, scope_length;
	msr_t msr;
	u8 *dsdt_pointer;
	u8 *pointer1;
	u8 *pointer2;
	u8 byte_index;
	u32 old_dsdt_length, new_dsdt_length;
	u32 corefeq, power, transitionlatency, busmasterlatency, control,
	    status;
	u32 new_package_length;
	u8 sum, checksum;
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
		if (TDP[index].socket_type == CPU_SOCKET_TYPE &&
		    TDP[index].cmp_cap == cmp_cap &&
		    TDP[index].pwr_lmt == pwr_lmt) {
			power_limit = TDP[index].power_limit;
		}

	/* See if the CPUID(0x80000007) returned EDX[2:1]==11b */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x6) != 0x6) {
		printk_info("No valid set of P-states\n");
		return 0;
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
		return 0;
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
	Pstate_num = 0;
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

	/*
	 * Modify the DSDT Table to put the actural _PSS package
	 * corefeq-->Pstate_feq[index]
	 * power-->Pstate_power[index]
	 * transitionlatency-->0x64
	 * busmasterlatency-->0x7,
	 * control--> 0xE8202800| Pstate_vid[index]<<6 | Pstate_fid[index]
	 * status --> Pstate_vid[index]<<6 | Pstate_fid[index]
	 * Get the _PSS control method Sig.
	 */

	dsdt_pointer = (u8 *) dsdt;
	old_dsdt_length = dsdt->length;
	new_dsdt_length = old_dsdt_length;
	printk_info("DSDT reconstruction...\n");
	for (i = 0x20; i < new_dsdt_length; i++)
		if ((*(dsdt_pointer + i) == '_')
		    && (*(dsdt_pointer + i + 1) == 'P')
		    && (*(dsdt_pointer + i + 2) == 'S')
		    && (*(dsdt_pointer + i + 3) == 'S')) {

			if ((*(dsdt_pointer + i + 4) !=
			     0x12) | (*(dsdt_pointer + i + 5) !=
				      0x4B) | (*(dsdt_pointer + i + 6) !=
					       0x10)) {
				printk_info
				    ("Error:No _PSS package leader byte!\n");
			} else {
				new_package_length =
				    0x10B - Defpkglength * (Maxpstate -
							    Pstate_num);
				/* two Pstates length will larger than 63, so we need not worry about the length */
				if (new_package_length > 63) {
					*(dsdt_pointer + i + 5) =
					    0x40 | (new_package_length & 0xf);
					*(dsdt_pointer + i + 6) =
					    (new_package_length & 0xff0) >> 4;
				}
				*(dsdt_pointer + i + 7) = Pstate_num;
			}

			if ((*(dsdt_pointer + i + 8) !=
			     0x12) | (*(dsdt_pointer + i + 9) !=
				      0x20) | (*(dsdt_pointer + i + 10) != 0x6))
				printk_info
				    ("Error:No package leader for the first Pstate!\n");
			for (index = 0; index < Pstate_num; index++) {
				corefeq = Pstate_feq[index];
				power = Pstate_power[index];
				transitionlatency = 0x64;
				busmasterlatency = 0x7;
				control =
				    (0x3 << 30) | /* IRT */
				    (0x2 << 28) | /* RVO */
				    (0x1 << 27) | /* ExtType */
				    (0x2 << 20) | /* PLL_LOCK_TIME */
				    (0x0 << 18) | /* MVS */
				    (0x5 << 11) | /* VST */
				    (Pstate_vid[index] << 6) |
				    Pstate_fid[index];
				status =
				    (Pstate_vid[index] << 6) |
				    Pstate_fid[index];
				for (byte_index = 0; byte_index < 4;
				     byte_index++) {
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + byte_index) =
		   corefeq >> (8 * byte_index);
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + 0x5 +
					  byte_index) =
		   power >> (8 * byte_index);
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + 0x5 * 2 +
					  byte_index) =
		   transitionlatency >> (8 * byte_index);
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + 0x5 * 3 +
					  byte_index) =
		   busmasterlatency >> (8 * byte_index);
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + 0x5 * 4 +
					  byte_index) =
		   control >> (8 * byte_index);
					*(dsdt_pointer + i + 0xC +
					  Defpkglength * index + 0x5 * 5 +
					  byte_index) =
		   status >> (8 * byte_index);
				}
			}
			pointer1 =
			    dsdt_pointer + i + 8 + Pstate_num * Defpkglength;
			pointer2 =
			    dsdt_pointer + i + 8 + Maxpstate * Defpkglength;
			while (pointer2 < dsdt_pointer + new_dsdt_length) {
				*pointer1 = *pointer2;
				pointer1++;
				pointer2++;
			}
			/* Recalcute the DSDT length */
			new_dsdt_length =
			    new_dsdt_length - Defpkglength * (Maxpstate -
							      Pstate_num);

			/* Search the first processor(CPUx) item and recalculate the processor length */
			for (j = 0; (dsdt_pointer + i - j) > dsdt_pointer; j++) {
				if ((*(dsdt_pointer + i - j) == 'C')
				    && (*(dsdt_pointer + i - j + 1) == 'P')
				    && (*(dsdt_pointer + i - j + 2) == 'U')) {
					processor_length =
					    ((*(dsdt_pointer + i - j - 1) << 4)
					     | (*(dsdt_pointer + i - j - 2) &
						0xf));
					processor_length =
					    processor_length -
					    Defpkglength * (Maxpstate -
							    Pstate_num);
					*(dsdt_pointer + i - j - 2) =
					    (processor_length & 0xf) | 0x40;
					*(dsdt_pointer + i - j - 1) =
					    (processor_length & 0xff0) >> 4;
					break;
				}
			}

			/* Search the first scope(_PR_) item and recalculate the scope length */
			for (j = 0; (dsdt_pointer + i - j) > dsdt_pointer; j++) {
				if ((*(dsdt_pointer + i - j) == '_')
				    && (*(dsdt_pointer + i - j + 1) == 'P')
				    && (*(dsdt_pointer + i - j + 2) == 'R')
				    && (*(dsdt_pointer + i - j + 3) == '_')) {
					scope_length =
					    ((*(dsdt_pointer + i - j - 1) << 4)
					     | (*(dsdt_pointer + i - j - 2) &
						0xf));
					scope_length =
					    scope_length -
					    Defpkglength * (Maxpstate -
							    Pstate_num);
					*(dsdt_pointer + i - j - 2) =
					    (scope_length & 0xf) | 0x40;
					*(dsdt_pointer + i - j - 1) =
					    (scope_length & 0xff0) >> 4;
					break;
				}
			}

		}

	/* Recalculate the DSDT length and fill back to the table */
	*(dsdt_pointer + 0x4) = new_dsdt_length;
	*(dsdt_pointer + 0x5) = new_dsdt_length >> 8;

	/*
	 * Recalculate the DSDT checksum and fill back to the table
	 * We must make sure the sum of the whole table is 0
	 */
	sum = 0;
	for (i = 0; i < new_dsdt_length; i++)
		if (i != 9)
			sum = sum + *(dsdt_pointer + i);
	checksum = 0x100 - sum;
	*(dsdt_pointer + 0x9) = checksum;

	/*Check the DSDT Table */
	/*
	 * printk_info("The new DSDT table length is %x\n", new_dsdt_length);
	 * printk_info("Details is as below:\n");
	 * for(i=0; i< new_dsdt_length; i++){
	 *      printk_info("%x\t",(unsigned char)*(dsdt_pointer+i));
	 *      if( ((i+1)&0x7) == 0x0)
	 *              printk_info("**0x%x**\n",i-7);
	 *}
	 */

	return 1;

}

unsigned long acpi_fill_ssdt_generator(unsigned long current, char *oem_table_id) {
	k8acpi_write_vars();
	return (unsigned long) (acpigen_get_current());
}

unsigned long write_acpi_tables(unsigned long start)
{
	unsigned long current;
	acpi_rsdp_t *rsdp;
	acpi_rsdt_t *rsdt;
	acpi_hpet_t *hpet;
	acpi_madt_t *madt;
	acpi_fadt_t *fadt;
	acpi_facs_t *facs;
	acpi_header_t *dsdt;
	acpi_header_t *ssdt;

	get_bus_conf();		/* it will get sblk, pci1234, hcdn, and sbdn */

	/* Align ACPI tables to 16byte */
	start = (start + 0x0f) & -0x10;
	current = start;

	printk_info("ACPI: Writing ACPI tables at %lx...\n", start);

	/* We need at least an RSDP and an RSDT Table */
	rsdp = (acpi_rsdp_t *) current;
	current += sizeof(acpi_rsdp_t);
	rsdt = (acpi_rsdt_t *) current;
	current += sizeof(acpi_rsdt_t);

	/* clear all table memory */
	memset((void *)start, 0, current - start);

	acpi_write_rsdp(rsdp, rsdt);
	acpi_write_rsdt(rsdt);

	/*
	 * We explicitly add these tables later on:
	 */
	/* If we want to use HPET Timers Linux wants an MADT */
	printk_debug("ACPI:    * HPET\n");
	hpet = (acpi_hpet_t *) current;
	current += sizeof(acpi_hpet_t);
	acpi_create_hpet(hpet);
	acpi_add_table(rsdt, hpet);

	printk_debug("ACPI:    * MADT\n");
	madt = (acpi_madt_t *) current;
	acpi_create_madt(madt);
	current += madt->header.length;
	acpi_add_table(rsdt, madt);

#if 0
	/* SRAT */
	printk_debug("ACPI:    * SRAT\n");
	srat = (acpi_srat_t *) current;
	acpi_create_srat(srat);
	current += srat->header.length;
	acpi_add_table(rsdt, srat);

	/* SLIT */
	printk_debug("ACPI:    * SLIT\n");
	slit = (acpi_slit_t *) current;
	acpi_create_slit(slit);
	current += slit->header.length;
	acpi_add_table(rsdt, slit);
#endif

	/* SSDT */
	printk_debug("ACPI:    * SSDT\n");
	ssdt = (acpi_header_t *)current;

	acpi_create_ssdt_generator(ssdt, "DYNADATA");
	current += ssdt->length;
	acpi_add_table(rsdt, ssdt);

#if ACPI_SSDTX_NUM >= 1

	/* same htio, but different position? We may have to copy, change HCIN, and recalculate the checknum and add_table */

	for (i = 1; i < sysconf.hc_possible_num; i++) {	/* 0: is hc sblink */
		if ((sysconf.pci1234[i] & 1) != 1)
			continue;
		uint8_t c;
		if (i < 7) {
			c = (uint8_t) ('4' + i - 1);
		} else {
			c = (uint8_t) ('A' + i - 1 - 6);
		}
		printk_debug("ACPI:    * SSDT for PCI%c Aka hcid = %d\n", c, sysconf.hcid[i]);	/* pci0 and pci1 are in dsdt */
		current = (current + 0x07) & -0x08;
		ssdtx = (acpi_header_t *) current;
		switch (sysconf.hcid[i]) {
		case 1:	/* 8132 */
			p = AmlCode_ssdt2;
			break;
		case 2:	/* 8151 */
			p = AmlCode_ssdt3;
			break;
		case 3:	/* 8131 */
			p = AmlCode_ssdt4;
			break;
		default:
			/* HTX no io apic */
			p = AmlCode_ssdt5;
			break;
		}
		current += ((acpi_header_t *) p)->length;
		memcpy((void *)ssdtx, (void *)p, ((acpi_header_t *) p)->length);
		update_ssdtx((void *)ssdtx, i);
		ssdtx->checksum = 0;
		ssdtx->checksum =
		    acpi_checksum((u8 *)ssdtx, ssdtx->length);
		acpi_add_table(rsdt, ssdtx);
	}
#endif

	/* FACS */
	printk_debug("ACPI:    * FACS\n");
	facs = (acpi_facs_t *) current;
	current += sizeof(acpi_facs_t);
	acpi_create_facs(facs);

	/* DSDT */
	printk_debug("ACPI:    * DSDT\n");
	dsdt = (acpi_header_t *) current;
	memcpy((void *)dsdt, (void *)AmlCode,
	       ((acpi_header_t *) AmlCode)->length);
	if (!pstates_algorithm(dsdt))
		printk_debug("pstates_algorithm error!\n");
	else
		printk_debug("pstates_algorithm success.\n");

	current += dsdt->length;
	printk_debug("ACPI:    * DSDT @ %08x Length %x\n", dsdt, dsdt->length);
	/* FADT */
	printk_debug("ACPI:    * FADT\n");
	fadt = (acpi_fadt_t *) current;
	current += sizeof(acpi_fadt_t);

	acpi_create_fadt(fadt, facs, dsdt);
	acpi_add_table(rsdt, fadt);

#if DUMP_ACPI_TABLES == 1
	printk_debug("rsdp\n");
	dump_mem(rsdp, ((void *)rsdp) + sizeof(acpi_rsdp_t));

	printk_debug("rsdt\n");
	dump_mem(rsdt, ((void *)rsdt) + sizeof(acpi_rsdt_t));

	printk_debug("madt\n");
	dump_mem(madt, ((void *)madt) + madt->header.length);

	printk_debug("srat\n");
	dump_mem(srat, ((void *)srat) + srat->header.length);

	printk_debug("slit\n");
	dump_mem(slit, ((void *)slit) + slit->header.length);

	printk_debug("ssdt\n");
	dump_mem(ssdt, ((void *)ssdt) + ssdt->length);

	printk_debug("fadt\n");
	dump_mem(fadt, ((void *)fadt) + fadt->header.length);
#endif

	printk_info("ACPI: done.\n");
	return current;
}
