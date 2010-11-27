/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "inteltool.h"

int fd_msr;

unsigned int cpuid(unsigned int op)
{
	uint32_t ret;

#if defined(__DARWIN__) && !defined(__LP64__)
	asm volatile (
		"pushl %%ebx\n"
		"cpuid\n"
		"popl %%ebx\n"
		: "=a" (ret) : "a" (op) : "%ecx", "%edx"
	);
#else
	asm ("cpuid" : "=a" (ret) : "a" (op) : "%ebx", "%ecx", "%edx");
#endif

	return ret;
}

#ifndef __DARWIN__
int msr_readerror = 0;

msr_t rdmsr(int addr)
{
	uint8_t buf[8];
	msr_t msr = { 0xffffffff, 0xffffffff };

	if (lseek(fd_msr, (off_t) addr, SEEK_SET) == -1) {
		perror("Could not lseek() to MSR");
		close(fd_msr);
		exit(1);
	}

	if (read(fd_msr, buf, 8) == 8) {
		msr.lo = *(uint32_t *)buf;
		msr.hi = *(uint32_t *)(buf + 4);

		return msr;
	}

	if (errno == 5) {
		printf(" (*)"); // Not all bits of the MSR could be read
		msr_readerror = 1;
	} else {
		// A severe error.
		perror("Could not read() MSR");
		close(fd_msr);
		exit(1);
	}

	return msr;
}
#endif

int print_intel_core_msrs(void)
{
	unsigned int i, core, id;
	msr_t msr;

#define IA32_PLATFORM_ID		0x0017
#define EBL_CR_POWERON			0x002a
#define FSB_CLK_STS			0x00cd
#define IA32_TIME_STAMP_COUNTER		0x0010
#define IA32_APIC_BASE			0x001b

	typedef struct {
		int number;
		char *name;
	} msr_entry_t;

	/* Pentium III */
	static const msr_entry_t model67x_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x002a, "EBL_CR_POWERON" },
		{ 0x0033, "TEST_CTL" },
		//{ 0x0079, "IA32_BIOS_UPDT_TRIG" }, // Seems to be RO
		{ 0x0088, "BBL_CR_D0" },
		{ 0x0089, "BBL_CR_D1" },
		{ 0x008a, "BBL_CR_D2" },
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00c1, "PERFCTR0" },
		{ 0x00c2, "PERFCTR1" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0116, "BBL_CR_ADDR" },
		{ 0x0118, "BBL_CR_DECC" },
		{ 0x0119, "BBL_CR_CTL" },
		//{ 0x011a, "BBL_CR_TRIG" },
		{ 0x011b, "BBL_CR_BUSY" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x017b, "IA32_MCG_CTL" },
		{ 0x0186, "IA32_PERF_EVNTSEL0" },
		{ 0x0187, "IA32_PERF_EVNTSEL1" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x01db, "MSR_LASTBRANCHFROMIP" },
		{ 0x01dc, "MSR_LASTBRANCHTOIP" },
		{ 0x01dd, "MSR_LASTINTFROMIP" },
		{ 0x01de, "MSR_LASTINTTOIP" },
		{ 0x01e0, "MSR_ROB_CR_BKUPTMPDR6" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		//{ 0x0403, "IA32_MC0_MISC" }, // Seems to be RO
		{ 0x0404, "IA32_MC1_CTL" },
		{ 0x0405, "IA32_MC1_STATUS" },
		{ 0x0406, "IA32_MC1_ADDR" },
		//{ 0x0407, "IA32_MC1_MISC" }, // Seems to be RO
		{ 0x0408, "IA32_MC2_CTL" },
		{ 0x0409, "IA32_MC2_STATUS" },
		{ 0x040a, "IA32_MC2_ADDR" },
		//{ 0x040b, "IA32_MC2_MISC" }, // Seems to be RO
		{ 0x040c, "IA32_MC4_CTL" },
		{ 0x040d, "IA32_MC4_STATUS" },
		{ 0x040e, "IA32_MC4_ADDR" },
		//{ 0x040f, "IA32_MC4_MISC" } // Seems to be RO
		{ 0x0410, "IA32_MC3_CTL" },
		{ 0x0411, "IA32_MC3_STATUS" },
		{ 0x0412, "IA32_MC3_ADDR" },
		//{ 0x0413, "IA32_MC3_MISC" }, // Seems to be RO
	};

	static const msr_entry_t model6bx_global_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x002a, "EBL_CR_POWERON" },
		{ 0x0033, "TEST_CTL" },
		{ 0x003f, "THERM_DIODE_OFFSET" },
		//{ 0x0079, "IA32_BIOS_UPDT_TRIG" }, // Seems to be RO
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00c1, "PERFCTR0" },
		{ 0x00c2, "PERFCTR1" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		//{ 0x0403, "IA32_MC0_MISC" }, // Seems to be RO
		{ 0x040c, "IA32_MC4_CTL" },
		{ 0x040d, "IA32_MC4_STATUS" },
		{ 0x040e, "IA32_MC4_ADDR" },
		//{ 0x040f, "IA32_MC4_MISC" } // Seems to be RO
	};

	static const msr_entry_t model6ex_global_msrs[] = {
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "EBL_CR_POWERON" },
		{ 0x00cd, "FSB_CLOCK_STS" },
		{ 0x00ce, "FSB_CLOCK_VCC" },
		{ 0x00e2, "CLOCK_CST_CONFIG_CONTROL" },
		{ 0x00e3, "PMG_IO_BASE_ADDR" },
		{ 0x00e4, "PMG_IO_CAPTURE_ADDR" },
		{ 0x00ee, "EXT_CONFIG" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0194, "CLOCK_FLEX_MAX" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01aa, "PIC_SENS_CFG" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		//{ 0x0403, "IA32_MC0_MISC" }, // Seems to be RO
		{ 0x040c, "IA32_MC4_CTL" },
		{ 0x040d, "IA32_MC4_STATUS" },
		{ 0x040e, "IA32_MC4_ADDR" },
		//{ 0x040f, "IA32_MC4_MISC" } // Seems to be RO
	};

	static const msr_entry_t model6ex_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x003f, "IA32_TEMPERATURE_OFFSET" },
		//{ 0x0079, "IA32_BIOS_UPDT_TRIG" }, // Seems to be RO
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x015f, "DTS_CAL_CTRL" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x019d, "GV_THERM" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		//{ 0x00c000080, "IA32_CR_EFER" }, // Seems to be RO
	};

	static const msr_entry_t model6fx_global_msrs[] = {
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "EBL_CR_POWERON" },
		{ 0x003f, "IA32_TEMPERATURE_OFFSET" },
		{ 0x00a8, "EMTTM_CR_TABLE0" },
		{ 0x00a9, "EMTTM_CR_TABLE1" },
		{ 0x00aa, "EMTTM_CR_TABLE2" },
		{ 0x00ab, "EMTTM_CR_TABLE3" },
		{ 0x00ac, "EMTTM_CR_TABLE4" },
		{ 0x00ad, "EMTTM_CR_TABLE5" },
		{ 0x00cd, "FSB_CLOCK_STS" },
		{ 0x00e2, "PMG_CST_CONFIG_CONTROL" },
		{ 0x00e3, "PMG_IO_BASE_ADDR" },
		{ 0x00e4, "PMG_IO_CAPTURE_ADDR" },
		{ 0x00ee, "EXT_CONFIG" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0194, "CLOCK_FLEX_MAX" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01aa, "PIC_SENS_CFG" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		//{ 0x0403, "IA32_MC0_MISC" }, // Seems to be RO
		{ 0x040c, "IA32_MC4_CTL" },
		{ 0x040d, "IA32_MC4_STATUS" },
		{ 0x040e, "IA32_MC4_ADDR" },
		//{ 0x040f, "IA32_MC4_MISC" } // Seems to be RO
	};

	static const msr_entry_t model6fx_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		//{ 0x0079, "IA32_BIOS_UPDT_TRIG" }, // Seems to be RO
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00e1, "SMM_CST_MISC_INFO" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_THERM_CTL" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x019d, "MSR_THERM2_CTL" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		//{ 0x00c000080, "IA32_CR_EFER" }, // Seems to be RO
	};

	/* Pentium 4 and XEON */
	/*
	 * All MSRs per
	 *
	 * Intel® 64 and IA-32 Architectures
	 * Software Developer.s Manual
	 * Volume 3B:
	 * System Programming Guide, Part 2
	 *
	 * Table B-5
	 */
	static const msr_entry_t modelf4x_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0006, "IA32_MONITOR_FILTER_LINE_SIZE" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x002b, "MSR_EBC_SOFT_POWRON" },
		{ 0x002c, "MSR_EBC_FREQUENCY_ID" },
// WRITE ONLY	{ 0x0079, "IA32_BIOS_UPDT_TRIG" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x019d, "MSR_THERM2_CTL" },
		{ 0x01a0, "IA32_MISC_ENABLE" },
		{ 0x01a1, "MSR_PLATFORM_BRV" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSMASK0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSMASK1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSMASK2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSMASK3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSMASK4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSMASK5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSMASK6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSMASK7" },
		{ 0x0250, "IA32_MTRR_FIX64K_00000" },
		{ 0x0258, "IA32_MTRR_FIX16K_80000" },
		{ 0x0259, "IA32_MTRR_FIX16K_A0000" },
		{ 0x0268, "IA32_MTRR_FIX4K_C0000" },
		{ 0x0269, "IA32_MTRR_FIX4K_C8000" },
		{ 0x026a, "IA32_MTRR_FIX4K_D0000" },
		{ 0x026b, "IA32_MTRR_FIX4K_D8000" },
		{ 0x026c, "IA32_MTRR_FIX4K_E0000" },
		{ 0x026d, "IA32_MTRR_FIX4K_E8000" },
		{ 0x026e, "IA32_MTRR_FIX4K_F0000" },
		{ 0x026f, "IA32_MTRR_FIX4K_F8000" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		{ 0x0300, "MSR_BPU_COUNTER0" },
		{ 0x0301, "MSR_BPU_COUNTER1" },
		{ 0x0302, "MSR_BPU_COUNTER2" },
		{ 0x0303, "MSR_BPU_COUNTER3" },
		/* Skipped through 0x3ff  for now*/

	/* All MCX_ADDR AND MCX_MISC MSRs depend on a bit being
	 * set in MCX_STATUS */
		{ 0x400, "IA32_MC0_CTL" },
		{ 0x401, "IA32_MC0_STATUS" },
		{ 0x402, "IA32_MC0_ADDR" },
		{ 0x403, "IA32_MC0_MISC" },
		{ 0x404, "IA32_MC1_CTL" },
		{ 0x405, "IA32_MC1_STATUS" },
		{ 0x406, "IA32_MC1_ADDR" },
		{ 0x407, "IA32_MC1_MISC" },
		{ 0x408, "IA32_MC2_CTL" },
		{ 0x409, "IA32_MC2_STATUS" },
		{ 0x40a, "IA32_MC2_ADDR" },
		{ 0x40b, "IA32_MC2_MISC" },
		{ 0x40c, "IA32_MC3_CTL" },
		{ 0x40d, "IA32_MC3_STATUS" },
		{ 0x40e, "IA32_MC3_ADDR" },
		{ 0x40f, "IA32_MC3_MISC" },
		{ 0x410, "IA32_MC4_CTL" },
		{ 0x411, "IA32_MC4_STATUS" },
		{ 0x412, "IA32_MC4_ADDR" },
		{ 0x413, "IA32_MC4_MISC" },
	};

	static const msr_entry_t modelf4x_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x009b, "IA32_SMM_MONITOR_CTL" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0180, "MSR_MCG_RAX" },
		{ 0x0181, "MSR_MCG_RBX" },
		{ 0x0182, "MSR_MCG_RCX" },
		{ 0x0183, "MSR_MCG_RDX" },
		{ 0x0184, "MSR_MCG_RSI" },
		{ 0x0185, "MSR_MCG_RDI" },
		{ 0x0186, "MSR_MCG_RBP" },
		{ 0x0187, "MSR_MCG_RSP" },
		{ 0x0188, "MSR_MCG_RFLAGS" },
		{ 0x0189, "MSR_MCG_RIP" },
		{ 0x018a, "MSR_MCG_MISC" },
		// 0x18b-f Reserved
		{ 0x0190, "MSR_MCG_R8" },
		{ 0x0191, "MSR_MCG_R9" },
		{ 0x0192, "MSR_MCG_R10" },
		{ 0x0193, "MSR_MCG_R11" },
		{ 0x0194, "MSR_MCG_R12" },
		{ 0x0195, "MSR_MCG_R13" },
		{ 0x0196, "MSR_MCG_R14" },
		{ 0x0197, "MSR_MCG_R15" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x0199, "IA32_PERF_CTL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x01a0, "IA32_MISC_ENABLE" }, // Bit 34 is Core Specific
		{ 0x01d7, "MSR_LER_FROM_LIP" },
		{ 0x01d8, "MSR_LER_TO_LIP" },
		{ 0x01d9, "MSR_DEBUGCTLA" },
		{ 0x01da, "MSR_LASTBRANCH_TOS" },
		{ 0x0277, "IA32_PAT" },
		/** Virtualization
		{ 0x480, "IA32_VMX_BASIC" },
		  through
		{ 0x48b, "IA32_VMX_PROCBASED_CTLS2" },
		  Not implemented in my CPU
		*/
		{ 0x0600, "IA32_DS_AREA" },
		/* 0x0680 - 0x06cf Branch Records Skipped */

	};



	typedef struct {
		unsigned int model;
		const msr_entry_t *global_msrs;
		unsigned int num_global_msrs;
		const msr_entry_t *per_core_msrs;
		unsigned int num_per_core_msrs;
	} cpu_t;

	cpu_t cpulist[] = {
		{ 0x00670, model67x_global_msrs, ARRAY_SIZE(model67x_global_msrs), NULL, 0 },
		{ 0x006b0, model6bx_global_msrs, ARRAY_SIZE(model6bx_global_msrs), NULL, 0 },
		{ 0x006e0, model6ex_global_msrs, ARRAY_SIZE(model6ex_global_msrs), model6ex_per_core_msrs, ARRAY_SIZE(model6ex_per_core_msrs) },
		{ 0x006f0, model6fx_global_msrs, ARRAY_SIZE(model6fx_global_msrs), model6fx_per_core_msrs, ARRAY_SIZE(model6fx_per_core_msrs) },
		{ 0x00f40, modelf4x_global_msrs, ARRAY_SIZE(modelf4x_global_msrs), modelf4x_per_core_msrs, ARRAY_SIZE(modelf4x_per_core_msrs) },
	};

	cpu_t *cpu = NULL;

	/* Get CPU family and model, not the stepping
	 * (TODO: extended family/model)
	 */
	id = cpuid(1) & 0xfffff0;
	for (i = 0; i < ARRAY_SIZE(cpulist); i++) {
		if(cpulist[i].model == id) {
			cpu = &cpulist[i];
			break;
		}
	}

	if (!cpu) {
		printf("Error: Dumping MSRs on this CPU (0x%06x) is not (yet) supported.\n", id);
		return -1;
	}

#ifndef __DARWIN__
	fd_msr = open("/dev/cpu/0/msr", O_RDWR);
	if (fd_msr < 0) {
		perror("Error while opening /dev/cpu/0/msr");
		printf("Did you run 'modprobe msr'?\n");
		return -1;
	}
#endif

	printf("\n===================== SHARED MSRs (All Cores) =====================\n");

	for (i = 0; i < cpu->num_global_msrs; i++) {
		msr = rdmsr(cpu->global_msrs[i].number);
		printf(" MSR 0x%08X = 0x%08X:0x%08X (%s)\n",
		       cpu->global_msrs[i].number, msr.hi, msr.lo,
		       cpu->global_msrs[i].name);
	}

	close(fd_msr);

	for (core = 0; core < 8; core++) {
#ifndef __DARWIN__
		char msrfilename[64];
		memset(msrfilename, 0, 64);
		sprintf(msrfilename, "/dev/cpu/%d/msr", core);

		fd_msr = open(msrfilename, O_RDWR);

		/* If the file is not there, we're probably through. No error,
		 * since we successfully opened /dev/cpu/0/msr before.
		 */
		if (fd_msr < 0)
			break;
#endif
		if (cpu->num_per_core_msrs)
			printf("\n====================== UNIQUE MSRs  (core %d) ======================\n", core);

		for (i = 0; i < cpu->num_per_core_msrs; i++) {
			msr = rdmsr(cpu->per_core_msrs[i].number);
			printf(" MSR 0x%08X = 0x%08X:0x%08X (%s)\n",
			       cpu->per_core_msrs[i].number, msr.hi, msr.lo,
			       cpu->per_core_msrs[i].name);
		}
#ifndef __DARWIN__
		close(fd_msr);
#endif
	}

#ifndef __DARWIN__
	if (msr_readerror)
		printf("\n(*) Some MSRs could not be read. The marked values are unreliable.\n");
#endif
	return 0;
}


