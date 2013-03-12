/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#ifndef REAL
#define REAL 1
#endif

#if !REAL
#define I386_STDINT_H 1
       #include <stdint.h>

/* Exact integral types */
typedef unsigned char      uint8_t;
typedef signed char        int8_t;

typedef unsigned short     uint16_t;
typedef signed short       int16_t;

typedef unsigned int       uint32_t;
typedef signed int         int32_t;

typedef unsigned long long uint64_t;


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

       #include <sys/types.h>
#endif
#if REAL
#include <types.h>
#include <console/console.h>
#endif

#if !REAL

#define CONFIG_GENERATE_ACPI_TABLES 1

#include <stdio.h>
#define printk(cnf, fmt, args...) printf (fmt, ##args)
#endif

#include <arch/acpi.h>
#include <arch/acpigen.h>
#include <arch/cpu.h>
#if REAL
#include <cpu/x86/msr.h>
#else
typedef struct msr_struct
{
	unsigned lo;
	unsigned hi;
} msr_t;

static int fd_msr;
#include <errno.h>
#include <stdlib.h>

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>


msr_t rdmsr(int addr)
{
	uint32_t buf[2];
	msr_t msr = { 0xffffffff, 0xffffffff };

	if (lseek(fd_msr, (off_t) addr, SEEK_SET) == -1) {
		perror("Could not lseek() to MSR");
		close(fd_msr);
		exit(1);
	}

	if (read(fd_msr, buf, 8) == 8) {
		msr.lo = buf[0];
		msr.hi = buf[1];
		return msr;
	}
		// A severe error.
	fprintf(stderr, "addr = %x", addr);
	perror("Could not read MSR");
		close(fd_msr);
		exit(1);

	return msr;
}

#endif
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#if REAL
#include <device/device.h>
#include <device/pci.h>
#endif

#include "model_206ax.h"
#include "chip.h"

#if !REAL
int cpu_config_tdp_levels(void)
{
	msr_t platform_info;

	/* Minimum CPU revision */
	if (cpuid_eax(1) < IVB_CONFIG_TDP_MIN_CPUID)
		return 0;

	/* Bits 34:33 indicate how many levels supported */
	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (platform_info.hi >> 1) & 3;
}
#endif

static int get_cores_per_package(void)
{
	struct cpuinfo_x86 c;
	struct cpuid_result result;
	int cores = 1;

	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return 1;

	result = cpuid_ext(0xb, 1);
	cores = result.ebx & 0xff;

	printk (BIOS_DEBUG, "0x%x cores\n", cores);

	return cores;
}

static int generate_cstate_entries(acpi_cstate_t *cstates,
				   int c1, int c2, int c3)
{
	int length, cstate_count = 0;

	/* Count number of active C-states */
	if (c1 > 0)
		++cstate_count;
	if (c2 > 0)
		++cstate_count;
	if (c3 > 0)
		++cstate_count;
	if (!cstate_count)
		return 0;

	length = acpigen_write_package(cstate_count + 1);
	length += acpigen_write_byte(cstate_count);

	/* Add an entry if the level is enabled */
	if (c1 > 0) {
		cstates[c1].ctype = 1;
		length += acpigen_write_CST_package_entry(&cstates[c1]);
	}
	if (c2 > 0) {
		cstates[c2].ctype = 2;
		length += acpigen_write_CST_package_entry(&cstates[c2]);
	}
	if (c3 > 0) {
		cstates[c3].ctype = 3;
		length += acpigen_write_CST_package_entry(&cstates[c3]);
	}

	acpigen_patch_len(length - 1);
	return length;
}

#if REAL
static int generate_C_state_entries(void)
{
	struct cpu_info *info;
	struct cpu_driver *cpu;
	int len, lenif;
	device_t lapic;
	struct cpu_intel_model_206ax_config *conf = NULL;

	/* Find the SpeedStep CPU in the device tree using magic APIC ID */
	lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);
	if (!lapic)
		return 0;
	conf = lapic->chip_info;
	if (!conf)
		return 0;

	/* Find CPU map of supported C-states */
	info = cpu_info();
	if (!info)
		return 0;
	cpu = find_cpu_driver(info->cpu);
	if (!cpu || !cpu->cstates)
		return 0;

	len = acpigen_emit_byte(0x14);		/* MethodOp */
	len += acpigen_write_len_f();		/* PkgLength */
	len += acpigen_emit_namestring("_CST");
	len += acpigen_emit_byte(0x00);		/* No Arguments */

	/* If running on AC power */
	len += acpigen_emit_byte(0xa0);		/* IfOp */
	lenif = acpigen_write_len_f();		/* PkgLength */
	lenif += acpigen_emit_namestring("PWRS");
	lenif += acpigen_emit_byte(0xa4);	/* ReturnOp */
	lenif += generate_cstate_entries(cpu->cstates, conf->c1_acpower,
					 conf->c2_acpower, conf->c3_acpower);
	acpigen_patch_len(lenif - 1);
	len += lenif;

	/* Else on battery power */
	len += acpigen_emit_byte(0xa4);	/* ReturnOp */
	len += generate_cstate_entries(cpu->cstates, conf->c1_battery,
					conf->c2_battery, conf->c3_battery);
	acpigen_patch_len(len - 1);
	return len;
}
#endif
static acpi_tstate_t tss_table_fine[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 94, 940, 0, 0x1f, 0 },
	{ 88, 880, 0, 0x1e, 0 },
	{ 82, 820, 0, 0x1d, 0 },
	{ 75, 760, 0, 0x1c, 0 },
	{ 69, 700, 0, 0x1b, 0 },
	{ 63, 640, 0, 0x1a, 0 },
	{ 57, 580, 0, 0x19, 0 },
	{ 50, 520, 0, 0x18, 0 },
	{ 44, 460, 0, 0x17, 0 },
	{ 38, 400, 0, 0x16, 0 },
	{ 32, 340, 0, 0x15, 0 },
	{ 25, 280, 0, 0x14, 0 },
	{ 19, 220, 0, 0x13, 0 },
	{ 13, 160, 0, 0x12, 0 },
};

static acpi_tstate_t tss_table_coarse[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 88, 875, 0, 0x1f, 0 },
	{ 75, 750, 0, 0x1e, 0 },
	{ 63, 625, 0, 0x1d, 0 },
	{ 50, 500, 0, 0x1c, 0 },
	{ 38, 375, 0, 0x1b, 0 },
	{ 25, 250, 0, 0x1a, 0 },
	{ 13, 125, 0, 0x19, 0 },
};

static int generate_T_state_entries(int core, int cores_per_package)
{
	int len;

	/* Indicate SW_ALL coordination for T-states */
	len = acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FFixedHW so OS will use MSR */
	len += acpigen_write_empty_PTC();

	/* Set a T-state limit that can be modified in NVS */
	len += acpigen_write_TPC("\\TLVL");

	/*
	 * CPUID.(EAX=6):EAX[5] indicates support
	 * for extended throttle levels.
	 */
	if (cpuid_eax(6) & (1 << 5))
		len += acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_fine), tss_table_fine);
	else
		len += acpigen_write_TSS_package(
			ARRAY_SIZE(tss_table_coarse), tss_table_coarse);

	return len;
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m;
	u32 power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 *
	 * Power = (ratio / p1_ratio) * m * tdp
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static int generate_P_state_entries(int core, int cores_per_package)
{
	int len, len_pss;
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * SANDYBRIDGE_BCLK + ratio_max / 3;

	/* Calculate CPU TDP in mW */
#if 0
	{
	  int power_unit;
	  msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	  power_unit = 2 << ((msr.lo & 0xf) - 1);
	  msr = rdmsr(MSR_PKG_POWER_SKU);
	  power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;
	}
#else
	power_max = 25000;
#endif
	/* Write _PCT indicating use of FFixedHW */
	len = acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	len += acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	len += acpigen_write_PSD_package(core, cores_per_package, coord_type);

	/* Add P-state entries in _PSS table */
	len += acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = (ratio_max - ratio_min) / ratio_step;
	while (num_entries > PSS_MAX_ENTRIES-1) {
		ratio_step <<= 1;
		num_entries >>= 1;
		printk (BIOS_DEBUG, "too much\n");
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		len_pss = acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		len_pss += acpigen_write_PSS_package(
			clock_max + 1,		/*MHz*/
			power_max,		/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio_turbo,	/*control*/
			ratio_turbo);	/*status*/
	} else {
		/* _PSS package count without Turbo */
		len_pss = acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	len_pss += acpigen_write_PSS_package(
		clock_max,		/*MHz*/
		power_max,		/*mW*/
		PSS_LATENCY_TRANSITION,	/*lat1*/
		PSS_LATENCY_BUSMASTER,	/*lat2*/
		ratio_max,		/*control*/
		ratio_max);	/*status*/

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * SANDYBRIDGE_BCLK + ratio / 3;

		len_pss += acpigen_write_PSS_package(
			clock,			/*MHz*/
			power,			/*mW*/
			PSS_LATENCY_TRANSITION,	/*lat1*/
			PSS_LATENCY_BUSMASTER,	/*lat2*/
			ratio,		/*control*/
			ratio);		/*status*/
	}

	/* Fix package length */
	len_pss--;
	acpigen_patch_len(len_pss);

	return len + len_pss;
}

void generate_cpu_entries(void)
{
	int len_pr;
	int coreID, cpuID, pcontrol_blk = PMB0_BASE, plen = 6;
#if REAL
	int totalcores = dev_count_cpu();
#else
	int totalcores = 4;
#endif
	int cores_per_package = get_cores_per_package();
	int numcpus = totalcores/cores_per_package;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each.\n",
	       numcpus, cores_per_package);

	for (cpuID=1; cpuID <=numcpus; cpuID++) {
		for (coreID=1; coreID<=cores_per_package; coreID++) {
			if (coreID>1) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_PR.CPUx */
			len_pr = acpigen_write_processor(
				(cpuID-1)*cores_per_package+coreID-1,
				pcontrol_blk, plen);

			/* Generate P-state tables */
			len_pr += generate_P_state_entries(
				cpuID-1, cores_per_package);

#if REAL
			/* Generate C-state tables */
			len_pr += generate_C_state_entries();
#endif
			/* Generate T-state tables */
			len_pr += generate_T_state_entries(
				cpuID-1, cores_per_package);

			len_pr--;
			acpigen_patch_len(len_pr);
		}
	}
}

struct chip_operations cpu_intel_model_206ax_ops = {
	CHIP_NAME("Intel SandyBridge/IvyBridge CPU")
};

#if !REAL

extern char result[1048576];
extern char *gencurrent;

u8 acpi_checksum(u8 *table, u32 length)
{
	u8 ret = 0;
	while (length--) {
		ret += *table;
		table++;
	}
	return -ret;
}

int
main (void)
{
  FILE *out;
  fd_msr = open("/dev/cpu/0/msr", O_RDWR);

	acpi_header_t *ssdt;
	ssdt = gencurrent;
	gencurrent = ssdt + 1;
	memset((void *)ssdt, 0, sizeof(acpi_header_t));

	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = 2; /* ACPI 1.0/2.0: ?, ACPI 3.0/4.0: 2 */
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, "CORE       ", 8);
	ssdt->oem_revision = 42;
	memcpy(&ssdt->asl_compiler_id, ASLC, 4);
	ssdt->asl_compiler_revision = 42;
	ssdt->length = sizeof(acpi_header_t);

	generate_cpu_entries();

	/* (Re)calculate length and checksum. */
	ssdt->length = gencurrent - (unsigned long)ssdt;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);

  out = fopen ("acpi.aml", "wb");
  fwrite (result, 1, gencurrent -result, out);
  return 0;
}
#endif
