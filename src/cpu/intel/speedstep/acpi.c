/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <types.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/acpi.h>
#include <cpu/intel/speedstep.h>
#include <device/device.h>

// XXX: PSS table values for power consumption are for Merom only

static int determine_total_number_of_cores(void)
{
	device_t cpu;
	int count = 0;
	for(cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_APIC_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled) {
			continue;
		}
		count++;
	}
	return count;
}

static int get_fsb(void)
{
	u32 fsbcode=(rdmsr(0xcd).lo >> 4) & 7;
	switch (fsbcode) {
		case 0: return 266;
		case 1: return 133;
		case 2: return 200;
		case 3: return 166;
		case 5: return 100;
	}
	printk(BIOS_DEBUG, "Warning: No supported FSB frequency. Assuming 200MHz\n");
	return 200;
}

void generate_cpu_entries(void)
{
	int len_pr, len_ps;
	int coreID, cpuID, pcontrol_blk = PMB0_BASE, plen = 6;
	msr_t msr;
	int totalcores = determine_total_number_of_cores();
	int cores_per_package = (cpuid_ebx(1)>>16) & 0xff;
	int numcpus = totalcores/cores_per_package; // this assumes that all CPUs share the same layout
	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n", numcpus, cores_per_package);

	for (cpuID=1; cpuID <=numcpus; cpuID++) {
		for (coreID=1; coreID<=cores_per_package; coreID++) {
		if (coreID>1) {
			pcontrol_blk = 0;
			plen = 0;
		}
		len_pr = acpigen_write_processor((cpuID-1)*cores_per_package+coreID-1, pcontrol_blk, plen);
			len_pr += acpigen_write_empty_PCT();
			len_pr += acpigen_write_PSD_package(cpuID-1,cores_per_package,SW_ANY);
			len_pr += acpigen_write_name("_PSS");

			int max_states=8;
			int busratio_step=2;
			msr = rdmsr(IA32_PERF_STS);
			int busratio_min=(msr.lo >> 24) & 0x1f;
			int busratio_max=(msr.hi >> (40-32)) & 0x1f;
			int vid_min=msr.lo & 0x3f;
			msr = rdmsr(IA32_PLATFORM_ID);
			int vid_max=msr.lo & 0x3f;
			int clock_max=get_fsb()*busratio_max;
			int clock_min=get_fsb()*busratio_min;
			printk(BIOS_DEBUG, "clocks between %d and %d MHz.\n", clock_min, clock_max);
#define MEROM_MIN_POWER 16000
#define MEROM_MAX_POWER 35000
			int power_max=MEROM_MAX_POWER;
			int power_min=MEROM_MIN_POWER;

			int num_states=(busratio_max-busratio_min)/busratio_step;
			while (num_states > max_states-1) {
				busratio_step <<= 1;
				num_states >>= 1;
			}
			printk(BIOS_DEBUG, "adding %x P-States between busratio %x and %x, incl. P0\n", num_states+1, busratio_min, busratio_max);
			int vid_step=(vid_max-vid_min)/num_states;
			int power_step=(power_max-power_min)/num_states;
			int clock_step=(clock_max-clock_min)/num_states;
			len_ps = acpigen_write_package(num_states+1); // for Super LFM, this must be increases by another one
			len_ps += acpigen_write_PSS_package(clock_max /*mhz*/, power_max /*mW*/, 0 /*lat1*/, 0 /*lat2*/, (busratio_max<<8)|(vid_max) /*control*/, (busratio_max<<8)|(vid_max) /*status*/);
			int current_busratio=busratio_min+((num_states-1)*busratio_step);
			int current_vid=vid_min+((num_states-1)*vid_step);
			int current_power=power_min+((num_states-1)*power_step);
			int current_clock=clock_min+((num_states-1)*clock_step);
			int i;
			for (i=0;i<num_states; i++) {
				len_ps += acpigen_write_PSS_package(current_clock /*mhz*/, current_power /*mW*/, 0 /*lat1*/, 0 /*lat2*/, (current_busratio<<8)|(current_vid) /*control*/, (current_busratio<<8)|(current_vid) /*status*/);
				current_busratio -= busratio_step;
				current_vid -= vid_step;
				current_power -= power_step;
				current_clock -= clock_step;
			}
			len_ps--;
			acpigen_patch_len(len_ps);
			len_pr += acpigen_write_PPC(0);
		len_pr += len_ps;
		len_pr--;
		acpigen_patch_len(len_pr);
		}
	}
}

