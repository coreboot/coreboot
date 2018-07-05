/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
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
#include <stdint.h>
#include <option.h>
#include <cpu/x86/msr.h>
#include <arch/acpigen.h>
#include <cpu/amd/powernow.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/amdfam10_sysconf.h>
#include <arch/cpu.h>
#include <northbridge/amd/amdht/AsPsDefs.h>
#include <northbridge/amd/amdmct/mct/mct.h>
#include <northbridge/amd/amdmct/amddefs.h>

static inline uint8_t is_fam15h(void)
{
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	return fam15h;
}

static void write_pstates_for_core(u8 pstate_num, u16 *pstate_feq, u32 *pstate_power,
				u32 *pstate_latency, u32 *pstate_control,
				u32 *pstate_status, int coreID,
				uint8_t single_link)
{
	int i;
	struct cpuid_result cpuid1;

	acpigen_write_empty_PCT();
	acpigen_write_name("_PSS");

	/* add later to total sum */
	acpigen_write_package(pstate_num);

	for (i = 0;i < pstate_num; i++)
		acpigen_write_PSS_package(pstate_feq[i],
					  pstate_power[i],
					  pstate_latency[i],
					  pstate_latency[i],
					  pstate_control[i],
					  pstate_status[i]);

	/* update the package size */
	acpigen_pop_len();

	/* Write PPC object */
	acpigen_write_PPC(pstate_num);

	/* Write PSD indicating coordination type */
	if ((single_link) && (mctGetLogicalCPUID(0) & AMD_DR_GT_Bx)) {
		/* Revision C or greater single-link processor */
		cpuid1 = cpuid(0x80000008);
		acpigen_write_PSD_package(0, (cpuid1.ecx & 0xff) + 1, SW_ALL);
	} else {
		/* Find the local APIC ID for the specified core ID */
		struct device* cpu;
		int cpu_index = 0;
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			if ((cpu->path.type != DEVICE_PATH_APIC) ||
				(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER))
				continue;
			if (!cpu->enabled)
				continue;
			if (cpu_index == coreID)
				break;
			cpu_index++;
		}

		if (cpu)
			acpigen_write_PSD_package(cpu->path.apic.apic_id, 1, SW_ANY);
	}
}

static void write_cstates_for_core(int coreID)
{
	/* Generate C state entries */
	uint8_t cstate_count = 1;
	acpi_cstate_t cstate;

	if (is_fam15h()) {
		cstate.ctype = 2;
		cstate.latency = 100;
		cstate.power = 0;
		cstate.resource.space_id = ACPI_ADDRESS_SPACE_IO;
		cstate.resource.bit_width = 8;
		cstate.resource.bit_offset = 0;
		cstate.resource.addrl = rdmsr(0xc0010073).lo + 1;
		cstate.resource.addrh = 0;
		cstate.resource.resv = 1;
	} else {
		cstate.ctype = 2;
		cstate.latency = 75;
		cstate.power = 0;
		cstate.resource.space_id = ACPI_ADDRESS_SPACE_IO;
		cstate.resource.bit_width = 8;
		cstate.resource.bit_offset = 0;
		cstate.resource.addrl = rdmsr(0xc0010073).lo;
		cstate.resource.addrh = 0;
		cstate.resource.resv = 1;
	}

	acpigen_write_CST_package(&cstate, cstate_count);

	/* Find the local APIC ID for the specified core ID */
	if (is_fam15h()) {
		struct device* cpu;
		int cpu_index = 0;
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			if ((cpu->path.type != DEVICE_PATH_APIC) ||
				(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER))
				continue;
			if (!cpu->enabled)
				continue;
			if (cpu_index == coreID)
				break;
			cpu_index++;
		}

		if (cpu) {
			/* TODO
			 * Detect dual core status and skip CSD generation if dual core is disabled
			 */

			/* Generate C state dependency entries */
			acpigen_write_CSD_package((cpu->path.apic.apic_id >> 1) & 0x7f, 2, CSD_HW_ALL, 0);
		}
	}
}

/*
* For details of this algorithm, please refer to:
* Family 10h BDKG 3.62 page 69
* Family 15h BDKG 3.14 page 74
*
* WARNING: The core count algorithm below assumes that all processors
* are identical, with the same number of active cores.  While the BKDG
* states the BIOS must enforce this coreboot does not currently do so.
* As a result it is possible that this code may break if an illegal
* processor combination is installed.  If it does break please fix the
* code in the proper locations!
*/
void amd_generate_powernow(u32 pcontrol_blk, u8 plen, u8 onlyBSP)
{
	u8 processor_brand[49];
	u32 *v;
	struct cpuid_result cpuid1;

	u16 Pstate_feq[10];
	u32 Pstate_power[10];
	u32 Pstate_latency[10];
	u32 Pstate_control[10];
	u32 Pstate_status[10];
	u8 Pstate_num;
	u8 cmp_cap;
	u8 index;
	msr_t msr;

	uint8_t nvram;
	uint8_t enable_c_states;

	enable_c_states = 0;
#if IS_ENABLED(CONFIG_HAVE_ACPI_TABLES)
	if (get_option(&nvram, "cpu_c_states") == CB_SUCCESS)
		enable_c_states = !!nvram;
#endif

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

	uint32_t dtemp;
	uint8_t node_index;
	uint8_t node_count;
	uint8_t cores_per_node;
	uint8_t total_core_count;
	uint8_t fam15h;
	uint8_t fam10h_rev_e = 0;

	/* Detect Revision E processors via method used in fidvid.c */
	if ((cpuid_edx(0x80000007) & CPB_MASK)
		&&  ((cpuid_ecx(0x80000008) & NC_MASK) == 5))
			fam10h_rev_e = 1;

	/*
	 * Based on the CPU socket type,cmp_cap and pwr_lmt , get the power limit.
	 * socket_type : 0x10 SocketF; 0x11 AM2/ASB1; 0x12 S1G1
	 * cmp_cap : 0x0 SingleCore; 0x1 DualCore; 0x2 TripleCore; 0x3 QuadCore; 0x4 QuintupleCore; 0x5 HexCore
	 */
	printk(BIOS_INFO, "Pstates algorithm ...\n");
	fam15h = !!(mctGetLogicalCPUID(0) & AMD_FAM15_ALL);
	/* Get number of cores */
	if (fam15h) {
		cmp_cap = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 5)), 0x84) & 0xff;
	} else {
		dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xe8);
		cmp_cap = (dtemp & 0x3000) >> 12;
		if (mctGetLogicalCPUID(0) & (AMD_FAM10_REV_D | AMD_FAM15_ALL))	/* revision D or higher */
			cmp_cap |= (dtemp & 0x8000) >> 13;
	}

	/* Get number of nodes */
	dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 0)), 0x60);
	node_count = ((dtemp & 0x70) >> 4) + 1;
	cores_per_node = cmp_cap + 1;

	/* Compute total number of cores installed in system */
	total_core_count = cores_per_node * node_count;

	/* Get number of boost states */
	uint8_t boost_count = 0;
	dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 4)), 0x15c);
	if (fam10h_rev_e)
		boost_count = (dtemp >> 2) & 0x1;
	else if (mctGetLogicalCPUID(0) & AMD_FAM15_ALL)
		boost_count = (dtemp >> 2) & 0x7;

	Pstate_num = 0;

	/* See if the CPUID(0x80000007) returned EDX[7]==1b */
	cpuid1 = cpuid(0x80000007);
	if ((cpuid1.edx & 0x80) != 0x80) {
		printk(BIOS_INFO, "No valid set of P-states\n");
		return;
	}

	if (fam15h)
		/* Set P_LVL2 P_BLK entry */
		*(((uint8_t *)pcontrol_blk) + 0x04) = (rdmsr(0xc0010073).lo + 1) & 0xff;

	uint8_t pviModeFlag;
	uint8_t Pstate_max;
	uint8_t cpufid;
	uint8_t cpudid;
	uint8_t cpuvid;
	uint8_t cpuidd;
	uint8_t cpuidv;
	uint8_t power_step_up;
	uint8_t power_step_down;
	uint8_t pll_lock_time;
	uint32_t expanded_cpuidv;
	uint32_t core_frequency;
	uint32_t core_power;
	uint32_t core_latency;
	uint32_t core_voltage;	/* multiplied by 10000 */
	uint8_t single_link;

	/* Determine if this is a PVI or SVI system */
	dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xA0);

	if (dtemp & PVI_MODE)
		pviModeFlag = 1;
	else
		pviModeFlag = 0;

	/* Get PSmax's index */
	msr = rdmsr(0xC0010061);
	Pstate_max = (uint8_t) ((msr.lo >> PS_MAX_VAL_SHFT) & ((fam15h)?BIT_MASK_7:BIT_MASK_3));

	/* Determine if all enabled Pstates have the same fidvid */
	uint8_t i;
	uint8_t cpufid_prev = (rdmsr(0xC0010064).lo & 0x3f);
	uint8_t all_enabled_cores_have_same_cpufid = 1;
	for (i = 1; i < Pstate_max; i++) {
		cpufid = rdmsr(0xC0010064 + i).lo & 0x3f;
		if (cpufid != cpufid_prev) {
			all_enabled_cores_have_same_cpufid = 0;
			break;
		}
	}

	/* Family 15h uses slightly different PSmax numbering */
	if (fam15h)
		Pstate_max++;

	/* Populate tables with all Pstate information */
	for (Pstate_num = 0; Pstate_num < Pstate_max; Pstate_num++) {
		/* Get power state information */
		msr = rdmsr(0xC0010064 + Pstate_num + boost_count);
		cpufid = (msr.lo & 0x3f);
		cpudid = (msr.lo & 0x1c0) >> 6;
		cpuvid = (msr.lo & 0xfe00) >> 9;
		cpuidd = (msr.hi & 0xff);
		cpuidv = (msr.hi & 0x300) >> 8;
		core_frequency = (100 * (cpufid + 0x10)) / (0x01 << cpudid);
		if (pviModeFlag) {
			if (cpuvid >= 0x20) {
				core_voltage = 7625 - (((cpuvid - 0x20) * 10000) / 80);
			} else {
				core_voltage = 15500 - ((cpuvid * 10000) / 40);
			}
		} else {
			cpuvid = cpuvid & 0x7f;
			if (cpuvid >= 0x7c)
				core_voltage = 0;
			else
				core_voltage = 15500 - ((cpuvid * 10000) / 80);
		}
		switch (cpuidv) {
			case 0x0:
				expanded_cpuidv = 1;
				break;
			case 0x1:
				expanded_cpuidv = 10;
				break;
			case 0x2:
				expanded_cpuidv = 100;
				break;
			case 0x3:
				expanded_cpuidv = 1000;
				break;
			default:
				printk(BIOS_ERR, "%s:%s:%d: Invalid cpuidv, "
					"not generating pstate tables.\n",
					__FILE__, __func__, __LINE__);
				return;
		}
		core_power = (core_voltage * cpuidd) / (expanded_cpuidv * 10);

		/* Calculate transition latency */
		dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xD4);
		power_step_up = (dtemp & 0xf000000) >> 24;
		power_step_down = (dtemp & 0xf00000) >> 20;
		dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x18, 3)), 0xA0);
		pll_lock_time = (dtemp & 0x3800) >> 11;
		if (all_enabled_cores_have_same_cpufid)
			core_latency = ((12 * power_step_down) + power_step_up) / 1000;
		else
			core_latency = (12 * (power_step_down + power_step_up) / 1000)
						 + pll_lock_time;

		Pstate_feq[Pstate_num] = core_frequency;
		Pstate_power[Pstate_num] = core_power;
		Pstate_latency[Pstate_num] = core_latency;
		Pstate_control[Pstate_num] = Pstate_num;
		Pstate_status[Pstate_num] = Pstate_num;
	}

	/* Print Pstate frequency, power, and latency */
	for (index = 0; index < Pstate_num; index++) {
		printk(BIOS_INFO, "Pstate_freq[%d] = %dMHz\t", index,
			    Pstate_feq[index]);
		printk(BIOS_INFO, "Pstate_power[%d] = %dmw\n", index,
			    Pstate_power[index]);
		printk(BIOS_INFO, "Pstate_latency[%d] = %dus\n", index,
			    Pstate_latency[index]);
	}

	/* Enter processor block scope */
	char pscope[] = "\\_PR";
	acpigen_write_scope(pscope);

	for (index = 0; index < total_core_count; index++) {
		/* Determine if this is a single-link processor */
		node_index = 0x18 + (index / cores_per_node);
		dtemp = pci_read_config32(dev_find_slot(0, PCI_DEVFN(node_index, 0)), 0x80);
		single_link = !!(((dtemp & 0xff00) >> 8) == 0);

		/* Enter processor core scope */
		uint8_t plen_cur = plen;
		uint32_t pcontrol_blk_cur = pcontrol_blk;
		if ((onlyBSP) && (index != 0)) {
			plen_cur = 0;
			pcontrol_blk_cur = 0;
		}
		acpigen_write_processor(index, pcontrol_blk_cur, plen_cur);

		/* Write P-state status and dependency objects */
		write_pstates_for_core(Pstate_num, Pstate_feq, Pstate_power,
				Pstate_latency, Pstate_control, Pstate_status,
				index, single_link);

		/* Write C-state status and dependency objects */
		if (fam15h && enable_c_states)
			write_cstates_for_core(index);

		/* Exit processor core scope */
		acpigen_pop_len();
	}

	/* Exit processor block scope */
	acpigen_pop_len();
}

void amd_powernow_update_fadt(acpi_fadt_t * fadt)
{
	if (is_fam15h()) {
		fadt->p_lvl2_lat = 101;		/* NOTE: While the BKDG states this should
						 * be set to 100, there is no way to meet
						 * the other FADT requirements.  I suspect
						 * there is an error in the BKDG for ACPI
						 * 1.x support; disable all FADT-based C
						 * states > 2... */
		fadt->p_lvl3_lat = 1001;
		fadt->flags |= 0x1 << 2;	/* FLAGS.PROC_C1 = 1 */
		fadt->flags |= 0x1 << 3;	/* FLAGS.P_LVL2_UP = 1 */
	} else {
		fadt->cst_cnt = 0;
	}
	fadt->pstate_cnt = 0;
}
