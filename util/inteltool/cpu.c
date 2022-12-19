/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "inteltool.h"

#ifdef __x86_64__
# define BREG	"%%rbx"
#else
# define BREG	"%%ebx"
#endif

#define CPUID_STRUCT_EXTENDED_FEATURE_FLAGS	0x7
#define IA32_FEATURE_CONTROL			0x3a
#define SGX_GLOBAL_ENABLED			(1 << 18)
#define FEATURE_CONTROL_LOCKED			(1)
#define MTRR_CAP_MSR				0xfe
#define PRMRR_SUPPORTED				(1 << 12)
#define SGX_SUPPORTED				(1 << 2)
#define IA32_TME_ACTIVATE			0x982
#define TME_SUPPORTED				(1 << 13)
#define TME_LOCKED				(1)
#define TME_ENABLED				(1 << 1)
#define CPUID_KEYLOCKER_FEATURE_FLAGS 		0x19
#define KEYLOCKER_SUPPORTED			(1<<23)
#define KEYLOCKER_AESKL				(1)

int fd_msr;

unsigned int cpuid(unsigned int op)
{
	uint32_t ret;

#if defined(__PIC__) || defined(__DARWIN__) && !defined(__LP64__)
	asm volatile (
		"push " BREG "\n\t"
		"cpuid\n\t"
		"pop " BREG "\n\t"
		: "=a" (ret) : "a" (op) : "%ecx", "%edx"
	);
#else
	asm ("cpuid" : "=a" (ret) : "a" (op) : "%ebx", "%ecx", "%edx");
#endif

	return ret;
}

inline cpuid_result_t cpuid_ext(int op, unsigned int ecx)
{
	cpuid_result_t result;

#ifndef __DARWIN__
	asm volatile (
			"mov %%ebx, %%edi;"
			"cpuid;"
			"mov %%ebx, %%esi;"
			"mov %%edi, %%ebx;"
			: "=a" (result.eax),
			"=S" (result.ebx),
			"=c" (result.ecx),
			"=d" (result.edx)
			: "0" (op), "2" (ecx)
			: "edi");
#endif
	return result;
}

#ifndef __DARWIN__
int msr_readerror = 0;

static msr_t rdmsr(int addr)
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

static int open_and_seek(int cpu, unsigned long msr, int mode, int *fd)
{
	char dev[32];
	char temp_string[50];

	snprintf(dev, sizeof(dev), "/dev/cpu/%d/msr", cpu);
	*fd = open(dev, mode);

	if (*fd < 0) {
		snprintf(temp_string, sizeof(temp_string), "open(\"%s\")", dev);
		perror(temp_string);
		return -1;
	}

	if (lseek(*fd, msr, SEEK_SET) == (off_t)-1) {
		snprintf(temp_string, sizeof(temp_string), "lseek(%lu)", msr);
		perror(temp_string);
		close(*fd);
		return -1;
	}

	return 0;
}

static msr_t rdmsr_from_cpu(int cpu, unsigned long addr)
{
	int fd;
	msr_t msr = { 0xffffffff, 0xffffffff };
	uint32_t buf[2];
	char temp_string[50];

	if (open_and_seek(cpu, addr, O_RDONLY, &fd) < 0) {
		snprintf(temp_string, sizeof(temp_string),
			"Could not read MSR for CPU#%d", cpu);
		perror(temp_string);
	}

	if (read(fd, buf, 8) == 8) {
		msr.lo = buf[0];
		msr.hi = buf[1];
	}

	close(fd);

	return msr;
}

static int get_number_of_cpus(void)
{
	return sysconf(_SC_NPROCESSORS_ONLN);
}

static int get_number_of_cores(void)
{
	return sysconf(_SC_NPROCESSORS_CONF);
}

static int is_sgx_supported(int cpunum)
{
	cpuid_result_t cpuid_regs;
	msr_t msr;

	/* CPUID leaf 0x7 subleaf 0x0 to detect SGX support
	details are mentioned in Intel SDM Chap.36- section 36.7 */
	cpuid_regs = cpuid_ext(0x7, 0x0);
	msr = rdmsr_from_cpu(cpunum, MTRR_CAP_MSR);
	return ((cpuid_regs.ebx & SGX_SUPPORTED) && (msr.lo & PRMRR_SUPPORTED));
}

static int is_sgx_enabled(int cpunum)
{
	msr_t data;
	data = rdmsr_from_cpu(cpunum, IA32_FEATURE_CONTROL);
	return (data.lo & SGX_GLOBAL_ENABLED);
}

static int is_sgx_locked(int cpunum)
{
	msr_t data;
	data = rdmsr_from_cpu(cpunum, IA32_FEATURE_CONTROL);
	return (data.lo & FEATURE_CONTROL_LOCKED);
}

#endif

int print_sgx(void)
{
	int error = -1;
#ifndef __DARWIN__
	int ncpus = get_number_of_cpus();
	int i = 0;

	printf("\n============= Dumping INTEL SGX status =============");

	if (ncpus < 1) {
		perror("Failed to get number of CPUs");
		error = -1;
	} else {
		printf("\nNumber of CPUs = %d\n", ncpus);
		for (i = 0; i < ncpus ; i++) {

			printf("------------- CPU %d ----------------\n", i);
			printf("SGX supported             : %s\n",
					is_sgx_supported(i) ? "YES" : "NO");
			printf("SGX enabled               : %s\n",
					is_sgx_enabled(i) ? "YES" : "NO");
			printf("Feature Control locked    : %s\n",
					is_sgx_locked(i) ? "YES" : "NO");
		}
		error = 0;
	}
	printf("====================================================\n");
#endif
	return error;
}

static int is_tme_supported()
{
	cpuid_result_t cpuid_regs;

	/*
	 * CPUID leaf 0x7 subleaf 0x0 to detect TME support
	 * https://software.intel.com/sites/default/files/managed/a5/16/Multi-Key
	 * -Total-Memory-Encryption-Spec.pdf
	 */

	cpuid_regs = cpuid_ext(0x7, 0x0);
        return (cpuid_regs.ecx & TME_SUPPORTED);
}

static msr_t read_tme_activate_msr(){
	return rdmsr_from_cpu(0, IA32_TME_ACTIVATE);
}

static int is_tme_locked()
{
	msr_t data = read_tme_activate_msr();
	return (data.lo & TME_LOCKED);
}

static int is_tme_enabled()
{
	msr_t data = read_tme_activate_msr();
	return (data.lo & TME_ENABLED);
}

void print_tme(void)
{
#ifndef __DARWIN__
	int tme_supported = is_tme_supported();

	printf("\n============= Dumping INTEL TME status =============\n");

	printf("TME supported : %s\n", tme_supported ? "YES" : "NO");

	if (tme_supported) {
		printf("TME locked    : %s\n", is_tme_locked() ? "YES" : "NO");
		printf("TME enabled   : %s\n", is_tme_enabled() ? "YES" : "NO");
	}
	printf("====================================================\n");
#else
	printf("Not Implemented\n");
#endif
}

static bool is_keylocker_supported()
{
	cpuid_result_t cpuid_regs;
	msr_t msr;

	/*
	 * CPUID leaf 0x7 subleaf 0x0 to detect Intel Key Locker support.
	 * The specification of Key Locker can be found at: https://www.intel.com/
	 * content/www/us/en/develop/download/intel-key-locker-specification.html
	 * The spec can also be found via document #343965 on Intel's site.
	 */
	cpuid_regs = cpuid_ext(CPUID_STRUCT_EXTENDED_FEATURE_FLAGS, 0x0);
	msr = rdmsr_from_cpu(0, MTRR_CAP_MSR);
	return ((cpuid_regs.ecx & KEYLOCKER_SUPPORTED) && (msr.lo & PRMRR_SUPPORTED));
}

static bool is_aeskl_enabled()
{
	cpuid_result_t cpuid_regs;

	/* CPUID leaf 0x19 subleaf 0x0 to detect details of Intel Key Locker feature */
	cpuid_regs = cpuid_ext(CPUID_KEYLOCKER_FEATURE_FLAGS, 0x0);
	return (cpuid_regs.ebx & KEYLOCKER_AESKL);
}

void print_keylocker(void)
{
#ifndef __DARWIN__
	int keylocker_supported = is_keylocker_supported();

	printf("\n============= Dumping INTEL Key Locker status =============\n");

	printf("Key Locker supported : %s\n", keylocker_supported ? "YES" : "NO");

	if (keylocker_supported) {
		printf("AESKL instructions enabled : %s\n", is_aeskl_enabled() ? "YES" : "NO");
	}
	printf("===========================================================\n");
#else
	printf("Not Implemented\n");
#endif
}

int print_intel_msrs(unsigned int range_start, unsigned int range_end)
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
	 * Intel 64 and IA-32 Architectures Software Developer's Manual
	 * Volume 3B: System Programming Guide, Part 2
	 *
	 * Table B-5, B-7
	 */
	static const msr_entry_t modelf2x_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		/* 0x6: Not available in model 2. */
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x002b, "MSR_EBC_SOFT_POWERON" },
		/* 0x2c: Not available in model 2. */
// WRITE ONLY	{ 0x0079, "IA32_BIOS_UPDT_TRIG" },
		{ 0x019c, "IA32_THERM_STATUS" },
		/* 0x19d: Not available in model 2. */
		{ 0x01a0, "IA32_MISC_ENABLE" },
		/* 0x1a1: Not available in model 2. */
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
		{ 0x0304, "MSR_MS_COUNTER0" },
		{ 0x0305, "MSR_MS_COUNTER1" },
		{ 0x0306, "MSR_MS_COUNTER2" },
		{ 0x0307, "MSR_MS_COUNTER3" },
		{ 0x0308, "MSR_FLAME_COUNTER0" },
		{ 0x0309, "MSR_FLAME_COUNTER1" },
		{ 0x030a, "MSR_FLAME_COUNTER2" },
		{ 0x030b, "MSR_FLAME_COUNTER3" },
		{ 0x030c, "MSR_IQ_COUNTER0" },
		{ 0x030d, "MSR_IQ_COUNTER1" },
		{ 0x030e, "MSR_IQ_COUNTER2" },
		{ 0x030f, "MSR_IQ_COUNTER3" },
		{ 0x0310, "MSR_IQ_COUNTER4" },
		{ 0x0311, "MSR_IQ_COUNTER5" },
		{ 0x0360, "MSR_BPU_CCCR0" },
		{ 0x0361, "MSR_BPU_CCCR1" },
		{ 0x0362, "MSR_BPU_CCCR2" },
		{ 0x0363, "MSR_BPU_CCCR3" },
		{ 0x0364, "MSR_MS_CCCR0" },
		{ 0x0365, "MSR_MS_CCCR1" },
		{ 0x0366, "MSR_MS_CCCR2" },
		{ 0x0367, "MSR_MS_CCCR3" },
		{ 0x0368, "MSR_FLAME_CCCR0" },
		{ 0x0369, "MSR_FLAME_CCCR1" },
		{ 0x036a, "MSR_FLAME_CCCR2" },
		{ 0x036b, "MSR_FLAME_CCCR3" },
		{ 0x036c, "MSR_IQ_CCCR0" },
		{ 0x036d, "MSR_IQ_CCCR1" },
		{ 0x036e, "MSR_IQ_CCCR2" },
		{ 0x036f, "MSR_IQ_CCCR3" },
		{ 0x0370, "MSR_IQ_CCCR4" },
		{ 0x0371, "MSR_IQ_CCCR5" },
		{ 0x03a0, "MSR_BSU_ESCR0" },
		{ 0x03a1, "MSR_BSU_ESCR1" },
		{ 0x03a2, "MSR_FSB_ESCR0" },
		{ 0x03a3, "MSR_FSB_ESCR1" },
		{ 0x03a4, "MSR_FIRM_ESCR0" },
		{ 0x03a5, "MSR_FIRM_ESCR1" },
		{ 0x03a6, "MSR_FLAME_ESCR0" },
		{ 0x03a7, "MSR_FLAME_ESCR1" },
		{ 0x03a8, "MSR_DAC_ESCR0" },
		{ 0x03a9, "MSR_DAC_ESCR1" },
		{ 0x03aa, "MSR_MOB_ESCR0" },
		{ 0x03ab, "MSR_MOB_ESCR1" },
		{ 0x03ac, "MSR_PMH_ESCR0" },
		{ 0x03ad, "MSR_PMH_ESCR1" },
		{ 0x03ae, "MSR_SAAT_ESCR0" },
		{ 0x03af, "MSR_SAAT_ESCR1" },
		{ 0x03b0, "MSR_U2L_ESCR0" },
		{ 0x03b1, "MSR_U2L_ESCR1" },
		{ 0x03b2, "MSR_BPU_ESCR0" },
		{ 0x03b3, "MSR_BPU_ESCR1" },
		{ 0x03b4, "MSR_IS_ESCR0" },
		{ 0x03b5, "MSR_BPU_ESCR1" },
		{ 0x03b6, "MSR_ITLB_ESCR0" },
		{ 0x03b7, "MSR_ITLB_ESCR1" },
		{ 0x03b8, "MSR_CRU_ESCR0" },
		{ 0x03b9, "MSR_CRU_ESCR1" },
		{ 0x03ba, "MSR_IQ_ESCR0" },
		{ 0x03bb, "MSR_IQ_ESCR1" },
		{ 0x03bc, "MSR_RAT_ESCR0" },
		{ 0x03bd, "MSR_RAT_ESCR1" },
		{ 0x03be, "MSR_SSU_ESCR0" },
		{ 0x03c0, "MSR_MS_ESCR0" },
		{ 0x03c1, "MSR_MS_ESCR1" },
		{ 0x03c2, "MSR_TBPU_ESCR0" },
		{ 0x03c3, "MSR_TBPU_ESCR1" },
		{ 0x03c4, "MSR_TC_ESCR0" },
		{ 0x03c5, "MSR_TC_ESCR1" },
		{ 0x03c8, "MSR_IX_ESCR0" },
		{ 0x03c9, "MSR_IX_ESCR1" },
		{ 0x03ca, "MSR_ALF_ESCR0" },
		{ 0x03cb, "MSR_ALF_ESCR1" },
		{ 0x03cc, "MSR_CRU_ESCR2" },
		{ 0x03cd, "MSR_CRU_ESCR3" },
		{ 0x03e0, "MSR_CRU_ESCR4" },
		{ 0x03e1, "MSR_CRU_ESCR5" },
		{ 0x03f0, "MSR_TC_PRECISE_EVENT" },
		{ 0x03f1, "MSR_PEBS_ENABLE" },
		{ 0x03f2, "MSR_PEBS_MATRIX_VERT" },

		/*
		 * All MCX_ADDR and MCX_MISC MSRs depend on a bit being
		 * set in MCX_STATUS.
		 */
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

	static const msr_entry_t modelf2x_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		/* 0x3a: Not available in model 2. */
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		/* 0x9b: Not available in model 2. */
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x017b, "IA32_MCG_CTL" },
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
		/* 0x18b-0x18f: Reserved */
		{ 0x0190, "MSR_MCG_R8" },
		{ 0x0191, "MSR_MCG_R9" },
		{ 0x0192, "MSR_MCG_R10" },
		{ 0x0193, "MSR_MCG_R11" },
		{ 0x0194, "MSR_MCG_R12" },
		{ 0x0195, "MSR_MCG_R13" },
		{ 0x0196, "MSR_MCG_R14" },
		{ 0x0197, "MSR_MCG_R15" },
		/* 0x198: Not available in model 2. */
		/* 0x199: Not available in model 2. */
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x01a0, "IA32_MISC_ENABLE" },
		{ 0x01d7, "MSR_LER_FROM_LIP" },
		{ 0x01d8, "MSR_LER_TO_LIP" },
		{ 0x01d9, "MSR_DEBUGCTLA" },
		{ 0x01da, "MSR_LASTBRANCH_TOS" },
		{ 0x01db, "MSR_LASTBRANCH_0" },
		{ 0x01dd, "MSR_LASTBRANCH_2" },
		{ 0x01de, "MSR_LASTBRANCH_3" },
		{ 0x0277, "IA32_PAT" },
		/* 0x480-0x48b : Not available in model 2. */
		{ 0x0600, "IA32_DS_AREA" },
		/* 0x0680 - 0x06cf Branch Records Skipped */
	};

	static const msr_entry_t modelf4x_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0006, "IA32_MONITOR_FILTER_LINE_SIZE" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x002b, "MSR_EBC_SOFT_POWERON" },
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

	/*
	 * 64-ia-32-architectures-software-developer-vol-3c-part-3-manual
	 * September 2016
	 */
	static const msr_entry_t modelf6x_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0006, "IA32_MONITOR_FILTER_LINE_SIZE" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x002b, "MSR_EBC_SOFT_POWERON" },
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
		{ 0x0304, "MSR_MS_COUNTER0" },
		{ 0x0305, "MSR_MS_COUNTER1" },
		{ 0x0306, "MSR_MS_COUNTER2" },
		{ 0x0307, "MSR_MS_COUNTER3" },
		{ 0x0308, "MSR_FLAME_COUNTER0" },
		{ 0x0309, "MSR_FLAME_COUNTER1" },
		{ 0x030a, "MSR_FLAME_COUNTER2" },
		{ 0x030b, "MSR_FLAME_COUNTER3" },
		{ 0x030c, "MSR_IQ_COUNTER0" },
		{ 0x030d, "MSR_IQ_COUNTER1" },
		{ 0x030e, "MSR_IQ_COUNTER2" },
		{ 0x030f, "MSR_IQ_COUNTER3" },
		{ 0x0310, "MSR_IQ_COUNTER4" },
		{ 0x0311, "MSR_IQ_COUNTER5" },
		{ 0x0360, "MSR_BPU_CCCR0" },
		{ 0x0361, "MSR_BPU_CCCR1" },
		{ 0x0362, "MSR_BPU_CCCR2" },
		{ 0x0363, "MSR_BPU_CCCR3" },
		{ 0x0364, "MSR_MS_CCCR0" },
		{ 0x0365, "MSR_MS_CCCR1" },
		{ 0x0366, "MSR_MS_CCCR2" },
		{ 0x0367, "MSR_MS_CCCR3" },
		{ 0x0368, "MSR_FLAME_CCCR0" },
		{ 0x0369, "MSR_FLAME_CCCR1" },
		{ 0x036A, "MSR_FLAME_CCCR2" },
		{ 0x036B, "MSR_FLAME_CCCR3" },
		{ 0x036C, "MSR_IQ_CCCR0" },
		{ 0x036D, "MSR_IQ_CCCR1" },
		{ 0x036E, "MSR_IQ_CCCR2" },
		{ 0x036F, "MSR_IQ_CCCR3" },
		{ 0x0370, "MSR_IQ_CCCR4" },
		{ 0x0371, "MSR_IQ_CCCR5" },
		{ 0x03A0, "MSR_BSU_ESCR0" },
		{ 0x03A1, "MSR_BSU_ESCR1" },
		{ 0x03A2, "MSR_FSB_ESCR0" },
		{ 0x03A3, "MSR_FSB_ESCR1" },
		{ 0x03A4, "MSR_FIRM_ESCR0" },
		{ 0x03A5, "MSR_FIRM_ESCR1" },
		{ 0x03A6, "MSR_FLAME_ESCR0" },
		{ 0x03A7, "MSR_FLAME_ESCR1" },
		{ 0x03A8, "MSR_DAC_ESCR0" },
		{ 0x03A9, "MSR_DAC_ESCR1" },
		{ 0x03AA, "MSR_MOB_ESCR0" },
		{ 0x03AB, "MSR_MOB_ESCR1" },
		{ 0x03AC, "MSR_PMH_ESCR0" },
		{ 0x03AD, "MSR_PMH_ESCR1" },
		{ 0x03AE, "MSR_SAAT_ESCR0" },
		{ 0x03AF, "MSR_SAAT_ESCR1" },
		{ 0x03B0, "MSR_U2L_ESCR0" },
		{ 0x03B1, "MSR_U2L_ESCR1" },
		{ 0x03B2, "MSR_BPU_ESCR0" },
		{ 0x03B3, "MSR_BPU_ESCR1" },
		{ 0x03B4, "MSR_IS_ESCR0" },
		{ 0x03B5, "MSR_IS_ESCR1" },
		{ 0x03B6, "MSR_ITLB_ESCR0" },
		{ 0x03B7, "MSR_ITLB_ESCR1" },
		{ 0x03B8, "MSR_CRU_ESCR0" },
		{ 0x03B9, "MSR_CRU_ESCR1" },
		{ 0x03BA, "MSR_IQ_ESCR0" },
		{ 0x03BB, "MSR_IQ_ESCR1" },
		{ 0x03BC, "MSR_RAT_ESCR0" },
		{ 0x03BD, "MSR_RAT_ESCR1" },
		{ 0x03BE, "MSR_SSU_ESCR0" },
		{ 0x03C0, "MSR_MS_ESCR0" },
		{ 0x03C1, "MSR_MS_ESCR1" },
		{ 0x03C2, "MSR_TBPU_ESCR0" },
		{ 0x03C3, "MSR_TBPU_ESCR1" },
		{ 0x03C4, "MSR_TC_ESCR0" },
		{ 0x03C5, "MSR_TC_ESCR1" },
		{ 0x03C8, "MSR_IX_ESCR0" },
		{ 0x03C9, "MSR_IX_ESCR1" },
		{ 0x03CA, "MSR_ALF_ESCR0" },
		{ 0x03CB, "MSR_ALF_ESCR1" },
		{ 0x03CC, "MSR_CRU_ESCR2" },
		{ 0x03CD, "MSR_CRU_ESCR3" },
		{ 0x03E0, "MSR_CRU_ESCR4" },
		{ 0x03E1, "MSR_CRU_ESCR5" },
		{ 0x03F0, "MSR_TC_PRECISE_EVENT" },
		{ 0x03F1, "MSR_PEBS_ENABLE" },
		{ 0x03F2, "MSR_PEBS_MATRIX_VERT" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		{ 0x0403, "IA32_MC0_MISC" },
		{ 0x0404, "IA32_MC1_CTL" },
		{ 0x0405, "IA32_MC1_STATUS" },
		{ 0x0406, "IA32_MC1_ADDR" },
		{ 0x0408, "IA32_MC2_CTL" },
		{ 0x0409, "IA32_MC2_STATUS" },
		{ 0x040b, "IA32_MC2_MISC" },
		{ 0x040c, "IA32_MC3_CTL" },
		{ 0x040d, "IA32_MC3_STATUS" },
		{ 0x040e, "IA32_MC3_ADDR" },
		{ 0x040f, "IA32_MC3_MISC" },
	};

	static const msr_entry_t modelf6x_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
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
		{ 0x01A2, "MSR_TEMPERATURE_TARGET" },
		{ 0x01d7, "MSR_LER_FROM_LIP" },
		{ 0x01d8, "MSR_LER_TO_LIP" },
		{ 0x01d9, "MSR_DEBUGCTLA" },
		{ 0x01da, "MSR_LASTBRANCH_TOS" },
		{ 0x0277, "IA32_PAT" },
		{ 0x0600, "IA32_DS_AREA" },
		{ 0x0680, "MSR_LASTBRANCH_0_FROM_IP" },
		{ 0x0681, "MSR_LASTBRANCH_1_FROM_IP" },
		{ 0x0682, "MSR_LASTBRANCH_2_FROM_IP" },
		{ 0x0683, "MSR_LASTBRANCH_3_FROM_IP" },
		{ 0x0684, "MSR_LASTBRANCH_4_FROM_IP" },
		{ 0x0685, "MSR_LASTBRANCH_5_FROM_IP" },
		{ 0x0686, "MSR_LASTBRANCH_6_FROM_IP" },
		{ 0x0687, "MSR_LASTBRANCH_7_FROM_IP" },
		{ 0x0688, "MSR_LASTBRANCH_8_FROM_IP" },
		{ 0x0689, "MSR_LASTBRANCH_9_FROM_IP" },
		{ 0x068a, "MSR_LASTBRANCH_10_FROM_IP" },
		{ 0x068b, "MSR_LASTBRANCH_11_FROM_IP" },
		{ 0x068c, "MSR_LASTBRANCH_12_FROM_IP" },
		{ 0x068d, "MSR_LASTBRANCH_13_FROM_IP" },
		{ 0x068e, "MSR_LASTBRANCH_14_FROM_IP" },
		{ 0x068f, "MSR_LASTBRANCH_15_FROM_IP" },
		{ 0x06c0, "MSR_LASTBRANCH_0_TO_IP" },
		{ 0x06c1, "MSR_LASTBRANCH_1_TO_IP" },
		{ 0x06c2, "MSR_LASTBRANCH_2_TO_IP" },
		{ 0x06c3, "MSR_LASTBRANCH_3_TO_IP" },
		{ 0x06c4, "MSR_LASTBRANCH_4_TO_IP" },
		{ 0x06c5, "MSR_LASTBRANCH_5_TO_IP" },
		{ 0x06c6, "MSR_LASTBRANCH_6_TO_IP" },
		{ 0x06c7, "MSR_LASTBRANCH_7_TO_IP" },
		{ 0x06c8, "MSR_LASTBRANCH_8_TO_IP" },
		{ 0x06c9, "MSR_LASTBRANCH_9_TO_IP" },
		{ 0x06ca, "MSR_LASTBRANCH_10_TO_IP" },
		{ 0x06cb, "MSR_LASTBRANCH_11_TO_IP" },
		{ 0x06cc, "MSR_LASTBRANCH_12_TO_IP" },
		{ 0x06cd, "MSR_LASTBRANCH_13_TO_IP" },
		{ 0x06ce, "MSR_LASTBRANCH_14_TO_IP" },
		{ 0x06cf, "MSR_LASTBRANCH_15_TO_IP" },
		/* Intel Xeon processor 7100 with L3 */
//		{ 0x107CC, "MSR_EMON_L3_CTR_CTL0" },
//		{ 0x107CD, "MSR_EMON_L3_CTR_CTL1" },
//		{ 0x107CE, "MSR_EMON_L3_CTR_CTL2" },
//		{ 0x107CF, "MSR_EMON_L3_CTR_CTL3" },
//		{ 0x107D0, "MSR_EMON_L3_CTR_CTL4" },
//		{ 0x107D1, "MSR_EMON_L3_CTR_CTL5" },
//		{ 0x107D2, "MSR_EMON_L3_CTR_CTL6" },
//		{ 0x107D3, "MSR_EMON_L3_CTR_CTL7" },
	};

	/* Atom N455
	 *
	 * This should apply to the following processors:
	 *  06_1CH
	 *  06_26H
	 *  06_27H
	 *  06_35
	 *  06_36
	 */
	/*
	 * All MSRs per
	 *
	 * Intel 64 and IA-32 Architectures Software Developer's Manual
	 * Volume 3C: System Programming Guide, Part 3
	 * Order Number 326019
	 * January 2013
	 *
	 * Table 35-4, 35-5
	 *
	 * For now it has only been tested with 06_1CH.
	 */
	static const msr_entry_t model6_atom_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x00cd, "MSR_FSB_FREQ" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x011e, "MSR_BBL_CR_CTL3" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x019d, "MSR_THERM2_CTL" },
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
		{ 0x0345, "IA32_PERF_CAPABILITIES" },
		{ 0x400, "IA32_MC0_CTL" },
		{ 0x401, "IA32_MC0_STATUS" },
		{ 0x402, "IA32_MC0_ADDR" },
		{ 0x404, "IA32_MC1_CTL" },
		{ 0x405, "IA32_MC1_STATUS" },
		{ 0x408, "IA32_MC2_CTL" },
		{ 0x409, "IA32_MC2_STATUS" },
		{ 0x40a, "IA32_MC2_ADDR" },
		{ 0x40c, "IA32_MC3_CTL" },
		{ 0x40d, "IA32_MC3_STATUS" },
		{ 0x40e, "IA32_MC3_ADDR" },
		{ 0x410, "IA32_MC4_CTL" },
		{ 0x411, "IA32_MC4_STATUS" },
		{ 0x412, "IA32_MC4_ADDR" },
		/*
		 * Only 06_27C has the following MSRs
		 */
		/*
		{ 0x03f8, "MSR_PKG_C2_RESIDENCY" },
		{ 0x03f9, "MSR_PKG_C4_RESIDENCY" },
		{ 0x03fa, "MSR_PKG_C6_RESIDENCY" },
		 */
	};

	static const msr_entry_t model6_atom_per_core_msrs[] = {
		{ 0x0006, "IA32_MONITOR_FILTER_SIZE" },
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x0040, "MSR_LASTBRANCH_0_FROM_IP" },
		{ 0x0041, "MSR_LASTBRANCH_1_FROM_IP" },
		{ 0x0042, "MSR_LASTBRANCH_2_FROM_IP" },
		{ 0x0043, "MSR_LASTBRANCH_3_FROM_IP" },
		{ 0x0044, "MSR_LASTBRANCH_4_FROM_IP" },
		{ 0x0045, "MSR_LASTBRANCH_5_FROM_IP" },
		{ 0x0046, "MSR_LASTBRANCH_6_FROM_IP" },
		{ 0x0047, "MSR_LASTBRANCH_7_FROM_IP" },
		{ 0x0060, "MSR_LASTBRANCH_0_TO_IP" },
		{ 0x0061, "MSR_LASTBRANCH_1_TO_IP" },
		{ 0x0062, "MSR_LASTBRANCH_2_TO_IP" },
		{ 0x0063, "MSR_LASTBRANCH_3_TO_IP" },
		{ 0x0064, "MSR_LASTBRANCH_4_TO_IP" },
		{ 0x0065, "MSR_LASTBRANCH_5_TO_IP" },
		{ 0x0066, "MSR_LASTBRANCH_6_TO_IP" },
		{ 0x0067, "MSR_LASTBRANCH_7_TO_IP" },
		/* Write register */
		/*
		{ 0x0079, "IA32_BIOS_UPDT_TRIG" },
		*/
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00c1, "IA32_PMC0" },
		{ 0x00c2, "IA32_PMC1" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0186, "IA32_PERF_EVNTSEL0" },
		{ 0x0187, "IA32_PERF_EVNTSEL1" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01c9, "MSR_LASTBRANCH_TOS" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x01dd, "MSR_LER_FROM_LIP" },
		{ 0x01de, "MSR_LER_TO_LIP" },
		{ 0x0277, "IA32_PAT" },
		{ 0x0309, "IA32_FIXED_CTR0" },
		{ 0x030a, "IA32_FIXED_CTR1" },
		{ 0x030b, "IA32_FIXED_CTR2" },
		{ 0x038d, "IA32_FIXED_CTR_CTRL" },
		{ 0x038e, "IA32_PERF_GLOBAL_STATUS" },
		{ 0x038f, "IA32_PERF_GLOBAL_CTRL" },
		{ 0x0390, "IA32_PERF_GLOBAL_OVF_CTRL" },
		{ 0x03f1, "MSR_PEBS_ENABLE" },
		{ 0x0480, "IA32_VMX_BASIC" },
		{ 0x0481, "IA32_VMX_PINBASED_CTLS" },
		{ 0x0482, "IA32_VMX_PROCBASED_CTLS" },
		{ 0x0483, "IA32_VMX_EXIT_CTLS" },
		{ 0x0484, "IA32_VMX_ENTRY_CTLS" },
		{ 0x0485, "IA32_VMX_MISC" },
		{ 0x0486, "IA32_VMX_CR0_FIXED0" },
		{ 0x0487, "IA32_VMX_CR0_FIXED1" },
		{ 0x0488, "IA32_VMX_CR4_FIXED0" },
		{ 0x0489, "IA32_VMX_CR4_FIXED1" },
		{ 0x048a, "IA32_VMX_VMCS_ENUM" },
		{ 0x048b, "IA32_VMX_PROCBASED_CTLS2" },
		{ 0x0600, "IA32_DS_AREA" },
	};

	static const msr_entry_t model20650_global_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0006, "IA32_MONITOR_FILTER_LINE_SIZE" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
// WRITE ONLY	{ 0x0079, "IA32_BIOS_UPDT_TRIG" },
		{ 0x00ce, "IA32_MSR_PLATFORM_INFO" },
		{ 0x00e2, "IA32_MSR_PMG_CST_CONFIG" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x019d, "MSR_THERM2_CTL" },
		{ 0x01a0, "IA32_MISC_ENABLE" },
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
		{ 0x40c, "IA32_MC3_CTL" },
		{ 0x40d, "IA32_MC3_STATUS" },
		{ 0x40e, "IA32_MC3_ADDR" },
		{ 0x410, "IA32_MC4_CTL" },
		{ 0x411, "IA32_MC4_STATUS" },
	};

	static const msr_entry_t model20650_per_core_msrs[] = {
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x009b, "IA32_SMM_MONITOR_CTL" },
		{ 0x00e4, "IA32_PMG_IO_CAPTURE_BASE" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0186, "MSR_MCG_RBP" },
		{ 0x0187, "MSR_MCG_RSP" },
		{ 0x0188, "MSR_MCG_RFLAGS" },
		{ 0x0189, "MSR_MCG_RIP" },
		{ 0x0194, "MSR_MCG_R12" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x0199, "IA32_PERF_CTL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x01a0, "IA32_MISC_ENABLE" }, // Bit 34 is Core Specific
		{ 0x01aa, "IA32_MISC_PWR_MGMT" },
		{ 0x01d9, "MSR_DEBUGCTLA" },
		{ 0x01fc, "MSR_POWER_CTL" },
		{ 0x0277, "IA32_PAT" },
		/** Virtualization
		{ 0x480, "IA32_VMX_BASIC" },
		  through
		{ 0x48b, "IA32_VMX_PROCBASED_CTLS2" },
		  Not implemented in my CPU
		*/
		{ 0x0600, "IA32_DS_AREA" },
		/* 0x0680 - 0x06cf Branch Records Skipped */

		{ 0x3a, "IA32_FEATURE_CONTROL" },
		{ 0x13c, "MSR_FEATURE_CONFIG" },
		{ 0x194, "MSR_FLEX_RATIO" },
		{ 0x1a0, "IA32_MISC_ENABLE" },
		{ 0x1a2, "MSR_TEMPERATURE_TARGET" },
		{ 0x199, "IA32_PERF_CTL" },
		{ 0x19b, "IA32_THERM_INTERRUPT" },
		{ 0x401, "IA32_MC0_STATUS" },
		{ 0x2e, "MSR_PIC_MSG_CONTROL" },
		{ 0xce, "MSR_PLATFORM_INFO" },
		{ 0xe2, "MSR_PMG_CST_CONFIG_CONTROL" },
		{ 0xe4, "MSR_PMG_IO_CAPTURE_BASE" },
		{ 0x1aa, "MSR_MISC_PWR_MGMT" },
		{ 0x1ad, "MSR_TURBO_RATIO_LIMIT" },
		{ 0x1fc, "MSR_POWER_CTL" },
	};

/*
 * The following two tables are the Silvermont registers listed in Table 35-6
 * Intel® 64 and IA-32 Architectures Software Developer's Manual
 * September 2014
 * Vol. 3C 35-59
 */
	static const msr_entry_t silvermont_per_core_msrs[] = {
		/*
		 * Per core MSRs in Intel Processors Based on the Silvermont Microarchitecture
		 * These are MSRs marked as "core"
		 *
		 */
		{ 0x0006, "IA32_MONITOR_FILTER_LINE_SIZE" },
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x0034, "MSR_SMI_COUNT" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x0040, "MSR_LASTBRANCH_0_FROM_IP" },
		{ 0x0041, "MSR_LASTBRANCH_1_FROM_IP" },
		{ 0x0042, "MSR_LASTBRANCH_2_FROM_IP" },
		{ 0x0043, "MSR_LASTBRANCH_3_FROM_IP" },
		{ 0x0044, "MSR_LASTBRANCH_4_FROM_IP" },
		{ 0x0045, "MSR_LASTBRANCH_5_FROM_IP" },
		{ 0x0046, "MSR_LASTBRANCH_6_FROM_IP" },
		{ 0x0047, "MSR_LASTBRANCH_7_FROM_IP" },
		{ 0x0060, "MSR_LASTBRANCH_0_TO_IP" },
		{ 0x0061, "MSR_LASTBRANCH_1_TO_IP" },
		{ 0x0062, "MSR_LASTBRANCH_2_TO_IP" },
		{ 0x0063, "MSR_LASTBRANCH_3_TO_IP" },
		{ 0x0064, "MSR_LASTBRANCH_4_TO_IP" },
		{ 0x0065, "MSR_LASTBRANCH_5_TO_IP" },
		{ 0x0066, "MSR_LASTBRANCH_6_TO_IP" },
		{ 0x0067, "MSR_LASTBRANCH_7_TO_IP" },
		/* Write register
		{ 0x0079, "IA32_BIOS_UPDT_TRIG" },
		*/
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00c1, "IA32_PMC0" },
		{ 0x00c2, "IA32_PMC1" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x0186, "IA32_PERF_EVNTSEL0" },
		{ 0x0187, "IA32_PERF_EVNTSEL1" },
		{ 0x0199, "IA32_PERF_CONTROL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x01a0, "IA32_MISC_ENABLES" },
		{ 0x01b0, "IA32_ENERGY_PERF_BIAS" },
		{ 0x01c9, "MSR_LASTBRANCH_TOS" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x01dd, "MSR_LER_FROM_LIP" },
		{ 0x01de, "MSR_LER_TO_LIP" },
		{ 0x01f2, "IA32_SMRR_PHYSBASE" },
		{ 0x01f3, "IA32_SMRR_PHYSMASK" },
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
		{ 0x0277, "IA32_PAT" },
		{ 0x02FF, "IA32_MTRR_DEF_TYPE" },
		{ 0x0309, "IA32_FIXED_CTR0" },
		{ 0x030a, "IA32_FIXED_CTR1" },
		{ 0x030b, "IA32_FIXED_CTR2" },
		{ 0x0345, "IA32_PERF_CAPABILITIES" },
		{ 0x038d, "IA32_FIXED_CTR_CTRL" },
		{ 0x038e, "IA32_PERF_GLOBAL_STATUS" },
		{ 0x038f, "IA32_PERF_GLOBAL_CTRL" },
		{ 0x0390, "IA32_PERF_GLOBAL_OVF_CTRL" },
		{ 0x03f1, "MSR_PEBS_ENABLE" },
		{ 0x03fd, "MSR_CORE_C6_RESIDENCY" },
		{ 0x40c, "IA32_MC3_CTL" },
		{ 0x40d, "IA32_MC3_STATUS" },
		{ 0x40e, "IA32_MC3_ADDR" },
		{ 0x410, "IA32_MC4_CTL" },
		{ 0x411, "IA32_MC4_STATUS" },
		{ 0x412, "IA32_MC4_ADDR" },
		{ 0x0480, "IA32_VMX_BASIC" },
		{ 0x0481, "IA32_VMX_PINBASED_CTLS" },
		{ 0x0482, "IA32_VMX_PROCBASED_CTLS" },
		{ 0x0483, "IA32_VMX_EXIT_CTLS" },
		{ 0x0484, "IA32_VMX_ENTRY_CTLS" },
		{ 0x0485, "IA32_VMX_MISC" },
		{ 0x0486, "IA32_VMX_CR0_FIXED0" },
		{ 0x0487, "IA32_VMX_CR0_FIXED1" },
		{ 0x0488, "IA32_VMX_CR4_FIXED0" },
		{ 0x0489, "IA32_VMX_CR4_FIXED1" },
		{ 0x048a, "IA32_VMX_VMCS_ENUM" },
		{ 0x048b, "IA32_VMX_PROCBASED_CTLS2" },
		{ 0x048c, "IA32_VMX_EPT_VPID_ENUM" },
		{ 0x048d, "IA32_VMX_TRUE_PINBASED_CTLS" },
		{ 0x048e, "IA32_VMX_TRUE_PROCBASED_CTLS" },
		{ 0x048f, "IA32_VMX_TRUE_EXIT_CTLS" },
		{ 0x0490, "IA32_VMX_TRUE_ENTRY_CTLS" },
		{ 0x0491, "IA32_VMX_FMFUNC" },
		{ 0x04c1, "IA32_A_PMC0" },
		{ 0x04c2, "IA32_A_PMC1" },
		{ 0x0600, "IA32_DS_AREA" },
		{ 0x0660, "MSR_CORE_C1_RESIDENCY" },
		{ 0x06e0, "IA32_TSC_DEADLINE" },
	};

	static const msr_entry_t silvermont_global_msrs[] = {
		/*
		 * Common MSRs in Intel Processors Based on the Silvermont Microarchitecture
		 * These are MSRs marked as "shared" or "package"
		 */
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x002a, "MSR_EBC_HARD_POWERON" },
		{ 0x00cd, "MSR_FSB_FREQ" },
		{ 0x00e2, "MSR_PKG_CST_CONFIG_CONTROL" },
		{ 0x00e4, "MSR_PMG_IO_CAPTURE_BASE" },
		{ 0x011e, "BBL_CR_CTL3" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x01A2, "MSR_TEMPERATURE_TARGET" },
		{ 0x01A6, "MSR_OFFCORE_RSP_0" },
		{ 0x01A7, "MSR_OFFCORE_RSP_1" },
		{ 0x01AD, "MSR_TURBO_RATIO_LIMIT" },
		{ 0x03fa, "MSR_PKG_C6_RESIDENCY" },
		{ 0x400, "IA32_MC0_CTL" },
		{ 0x401, "IA32_MC0_STATUS" },
		{ 0x402, "IA32_MC0_ADDR" },
		{ 0x404, "IA32_MC1_CTL" },
		{ 0x405, "IA32_MC1_STATUS" },
		{ 0x408, "IA32_MC2_CTL" },
		{ 0x409, "IA32_MC2_STATUS" },
		{ 0x40a, "IA32_MC2_ADDR" },
		{ 0x414, "MSR_MC5_CTL" },
		{ 0x415, "MSR_MC5_STATUS" },
		{ 0x416, "MSR_MC5_ADDR" },
	};

/*
 * Intel 64 and IA-32 Architectures Software Developers Manual Conbined Volumes
 * Page 4668
 *
 * The following two tables are for the Kaby Lake processors
 * 06_9EH.
 *
 */

	static const msr_entry_t model96ex_global_msrs[] = {
		{ 0x0017, "IA32_PLATFORM_ID"},
		{ 0x0080, "MSR_TRACE_HUB_STH_ACPIBAR_BASE"},
		{ 0x00CE, "MSR_PLATFORM_INFO"},
		{ 0x0198, "IA32_PERF_STATUS"},
		{ 0x01A2, "MSR_TEMPERATURE_TARGET"},
		{ 0x01AD, "MSR_TURBO_RATIO_LIMIT"},
		{ 0x0284, "IA32_MC4_CTL2"},
		{ 0x02F4, "MSR_UNCORE_PRMRR_PHYS_BASE"},
		{ 0x02F5, "MSR_UNCORE_PRMRR_PHYS_MASK"},
		{ 0x0394, "MSR_UNC_PERF_FIXED_CTRL"},
		{ 0x0395, "MSR_UNC_PERF_FIXED_CTR"},
		{ 0x060A, "MSR_PKGC3_IRTL"},
		{ 0x060B, "MSR_PKGC6_IRTL"},
		{ 0x060D, "MSR_PKG_C2_RESIDENCY"},
		{ 0x0610, "MSR_PKG_POWER_LIMIT"},
		{ 0x0614, "MSR_PKG_POWER_INFO"},
		{ 0x0620, "MSR_RING_RATIO_LIMIT"},
		{ 0x0638, "MSR_PP0_POWER_LIMIT"},
		{ 0x064F, "MSR_CORE_PERF_LIMIT_REASONS"},
		{ 0x0652, "MSR_PKG_HDC_CONFIG"},
		{ 0x065C, "MSR_PLATFORM_POWER_LIMIT"},
		{ 0x06B0, "MSR_GRAPHICS_PERF_LIMIT_REASONS"},
		{ 0x06B1, "MSR_RING_PERF_LIMIT_REASONS"},
		{ 0x0770, "IA32_PM_ENABLE"},
		{ 0x0DB0, "IA32_PKG_HDC_CTL"},
		{ 0x03B0, "MSR_UNC_ARB_PERFCTR0"},
		{ 0x03B1, "MSR_UNC_ARB_PERFCTR1"},
		{ 0x03B2, "MSR_UNC_ARB_PERFEVTSEL0"},
		{ 0x03B3, "MSR_UNC_ARB_PERFEVTSEL1"},
		{ 0x0700, "MSR_UNC_CBO_0_PERFCTR0"},
		{ 0x0701, "MSR_UNC_CBO_0_PERFCTR1"},
		{ 0x0706, "MSR_UNC_CBO_0_PERFEVTSEL0"},
		{ 0x0707, "MSR_UNC_CBO_0_PERFEVTSEL1"},
		{ 0x0710, "MSR_UNC_CBO_1_PERFCTR0"},
		{ 0x0711, "MSR_UNC_CBO_1_PERFCTR1"},
		{ 0x0716, "MSR_UNC_CBO_1_PERFEVTSEL0"},
		{ 0x0717, "MSR_UNC_CBO_1_PERFEVTSEL1"},
		{ 0x0720, "MSR_UNC_CBO_2_PERFCTR0"},
		{ 0x0721, "MSR_UNC_CBO_2_PERFCTR1"},
		{ 0x0726, "MSR_UNC_CBO_2_PERFEVTSEL0"},
		{ 0x0727, "MSR_UNC_CBO_2_PERFEVTSEL1"},
		{ 0x0730, "MSR_UNC_CBO_3_PERFCTR0"},
		{ 0x0731, "MSR_UNC_CBO_3_PERFCTR1"},
		{ 0x0736, "MSR_UNC_CBO_3_PERFEVTSEL0"},
		{ 0x0737, "MSR_UNC_CBO_3_PERFEVTSEL1"},
		{ 0x0E01, "MSR_UNC_PERF_GLOBAL_CTRL"},
		{ 0x0E02, "MSR_UNC_PERF_GLOBAL_STATUS"},
	};

	static const msr_entry_t model96ex_per_core_msrs[] = {
		/* Per core MSRs for Sandy Bridge and above */
		{ 0x0000, "IA32_P5_MC_ADDR"},
		{ 0x0001, "IA32_P5_MC_TYPE"},
		{ 0x0006, "IA32_MONITOR_FILTER_SIZE"},
		{ 0x0010, "IA32_TIME_STAMP_COUNTER"},
		{ 0x001B, "IA32_APIC_BASE"},
		{ 0x0034, "MSR_SMI_COUNT"},
		{ 0x003A, "IA32_FEATURE_CONTROL"},
		{ 0x008B, "IA32_BIOS_SIGN_ID"},
		{ 0x00C1, "IA32_PMC0" },
		{ 0x00C2, "IA32_PMC1" },
		{ 0x00C3, "IA32_PMC2" },
		{ 0x00C4, "IA32_PMC3" },
		{ 0x00E2, "MSR_PKG_CST_CONFIG_CONTROL" },
		{ 0x00E4, "MSR_PMG_IO_CAPTURE_BASE"},
		{ 0x00E7, "IA32_MPERF"},
		{ 0x00E8, "IA32_APERF"},
		{ 0x00FE, "IA32_MTRRCAP"},
		{ 0x013C, "MSR_FEATURE_CONFIG"},
		{ 0x0174, "IA32_SYSENTER_CS"},
		{ 0x0175, "IA32_SYSENTER_ESP"},
		{ 0x0176, "IA32_SYSENTER_EIP"},
		{ 0x0179, "IA32_MCG_CAP"},
		{ 0x017A, "IA32_MCG_STATUS"},
		{ 0x0186, "IA32_PERFEVTSEL0"},
		{ 0x0187, "IA32_PERFEVTSEL1"},
		{ 0x0188, "IA32_PERFEVTSEL2"},
		{ 0x0189, "IA32_PERFEVTSEL3"},
		{ 0x0199, "IA32_PERF_CTL"},
		{ 0x019A, "IA32_CLOCK_MODULATION"},
		{ 0x019B, "IA32_THERM_INTERRUPT"},
		{ 0x019C, "IA32_THERM_STATUS"},
		{ 0x01A0, "IA32_MISC_ENABLE"},
		{ 0x01A4, "IA32_MISC_FEATURE_CONTROL"},
		{ 0x01A6, "MSR_OFFCORE_RSP_0"},
		{ 0x01A7, "MSR_OFFCORE_RSP_1"},
		{ 0x01C8, "MSR_LBR_SELECT"},
		{ 0x01C9, "MSR_LASTBRANCH_TOS"},
		{ 0x01D9, "IA32_DEBUGCTL"},
		{ 0x01DD, "MSR_LER_FROM_LIP"},
		{ 0x01DE, "MSR_LER_TO_LIP"},
		{ 0x01F2, "IA32_SMRR_PHYSBASE"},
		{ 0x01F3, "IA32_SMRR_PHYSMASK"},
		{ 0x01F4, "MSR_PRMRR_PHYS_BASE"},
		{ 0x01F5, "MSR_PRMRR_PHYS_MASK"},
		{ 0x01FB, "MSR_PRMRR_VALID_CONFIG"},
		{ 0x01FC, "MSR_POWER_CTL"},
		{ 0x0200, "IA32_MTRR_PHYSBASE0"},
		{ 0x0201, "IA32_MTRR_PHYSBASE0"},
		{ 0x0202, "IA32_MTRR_PHYSBASE1"},
		{ 0x0203, "IA32_MTRR_PHYSBASE1"},
		{ 0x0204, "IA32_MTRR_PHYSBASE2"},
		{ 0x0205, "IA32_MTRR_PHYSBASE2"},
		{ 0x0206, "IA32_MTRR_PHYSBASE3"},
		{ 0x0207, "IA32_MTRR_PHYSBASE3"},
		{ 0x0208, "IA32_MTRR_PHYSBASE4"},
		{ 0x0209, "IA32_MTRR_PHYSBASE4"},
		{ 0x020A, "IA32_MTRR_PHYSBASE5"},
		{ 0x020B, "IA32_MTRR_PHYSBASE5"},
		{ 0x020C, "IA32_MTRR_PHYSBASE6"},
		{ 0x020D, "IA32_MTRR_PHYSBASE6"},
		{ 0x020E, "IA32_MTRR_PHYSBASE7"},
		{ 0x020F, "IA32_MTRR_PHYSBASE7"},
		{ 0x0210, "IA32_MTRR_PHYSBASE8"},
		{ 0x0211, "IA32_MTRR_PHYSBASE8"},
		{ 0x0212, "IA32_MTRR_PHYSBASE9"},
		{ 0x0213, "IA32_MTRR_PHYSBASE9"},
		{ 0x0250, "IA32_MTRR_FIX64K_00000"},
		{ 0x0258, "IA32_MTRR_FIX16K_80000"},
		{ 0x0259, "IA32_MTRR_FIX16K_A0000"},
		{ 0x0268, "IA32_MTRR_FIX4K_C0000"},
		{ 0x0269, "IA32_MTRR_FIX4K_C8000"},
		{ 0x026A, "IA32_MTRR_FIX4K_D0000"},
		{ 0x026B, "IA32_MTRR_FIX4K_D8000"},
		{ 0x026C, "IA32_MTRR_FIX4K_E0000"},
		{ 0x026D, "IA32_MTRR_FIX4K_E8000"},
		{ 0x026E, "IA32_MTRR_FIX4K_F0000"},
		{ 0x026F, "IA32_MTRR_FIX4K_F8000"},
		{ 0x0277, "IA32_PAT"},
		{ 0x0280, "IA32_MC0_CTL2"},
		{ 0x0281, "IA32_MC1_CTL2"},
		{ 0x0282, "IA32_MC2_CTL2"},
		{ 0x0283, "IA32_MC3_CTL2"},
		{ 0x02FF, "IA32_MTRR_DEF_TYPE"},
		{ 0x0309, "IA32_FIXED_CTR0"},
		{ 0x030A, "IA32_FIXED_CTR1"},
		{ 0x030B, "IA32_FIXED_CTR2"},
		{ 0x0345, "IA32_PERF_CAPABILITIES"},
		{ 0x038D, "IA32_FIXED_CTR_CTRL"},
		{ 0x038E, "IA32_PERF_GLOBAL_STATUS"},
		{ 0x038F, "IA32_PERF_GLOBAL_CTRL"},
		{ 0x0390, "IA32_PERF_GLOBAL_STATUS_RESET"},
		{ 0x0391, "IA32_PERF_GLOBAL_STATUS_SET"},
		{ 0x03F1, "MSR_PEBS_ENABLE"},
		{ 0x03F6, "MSR_PEBS_LD_LAT"},
		{ 0x03F7, "MSR_PEBS_FRONTEND"},
		{ 0x03FC, "MSR_CORE_C3_RESIDENCY"},
		{ 0x03FD, "MSR_CORE_C6_RESIDENCY"},
		{ 0x03FE, "MSR_CORE_C7_RESIDENCY"},
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		{ 0x0403, "IA32_MC0_MISC" },
		{ 0x0404, "IA32_MC1_CTL" },
		{ 0x0405, "IA32_MC1_STATUS" },
		{ 0x0406, "IA32_MC1_ADDR" },
		{ 0x0407, "IA32_MC1_MISC" },
		{ 0x0408, "IA32_MC2_CTL" },
		{ 0x0409, "IA32_MC2_STATUS" },
		{ 0x040a, "IA32_MC2_ADDR" },
		{ 0x040c, "IA32_MC3_CTL" },
		{ 0x040d, "IA32_MC3_STATUS" },
		{ 0x040e, "IA32_MC3_ADDR" },
		{ 0x0410, "IA32_MC4_CTL" },
		{ 0x0411, "IA32_MC4_STATUS" },
		{ 0x0480, "IA32_VMX_BASIC" },
		{ 0x0481, "IA32_VMX_PINBASED_CTLS" },
		{ 0x0482, "IA32_VMX_PROCBASED_CTLS" },
		{ 0x0483, "IA32_VMX_EXIT_CTLS" },
		{ 0x0484, "IA32_VMX_ENTRY_CTLS" },
		{ 0x0485, "IA32_VMX_MISC" },
		{ 0x0486, "IA32_VMX_CR0_FIXED0" },
		{ 0x0487, "IA32_VMX_CR0_FIXED1" },
		{ 0x0488, "IA32_VMX_CR4_FIXED0" },
		{ 0x0489, "IA32_VMX_CR4_FIXED1" },
		{ 0x048a, "IA32_VMX_VMCS_ENUM" },
		{ 0x048b, "IA32_VMX_PROCBASED_CTLS2" },
		{ 0x048c, "IA32_VMX_EPT_VPID_ENUM" },
		{ 0x048d, "IA32_VMX_TRUE_PINBASED_CTLS" },
		{ 0x048e, "IA32_VMX_TRUE_PROCBASED_CTLS" },
		{ 0x048f, "IA32_VMX_TRUE_EXIT_CTLS" },
		{ 0x0490, "IA32_VMX_TRUE_ENTRY_CTLS" },
		{ 0x04C1, "IA32_A_PMC0"},
		{ 0x04C2, "IA32_A_PMC1"},
		{ 0x04C3, "IA32_A_PMC2"},
		{ 0x04C4, "IA32_A_PMC3"},
		{ 0x0500, "IA32_SGX_SVN_STATUS"},
		{ 0x0560, "IA32_RTIT_OUTPUT_BASE"},
		{ 0x0561, "IA32_RTIT_OUTPUT_MASK_PTRS"},
		{ 0x0570, "IA32_RTIT_CTL"},
		{ 0x0571, "IA32_RTIT_STATUS"},
		{ 0x0572, "IA32_RTIT_CR3_MATCH"},
		{ 0x0580, "IA32_RTIT_ADDR0_A"},
		{ 0x0581, "IA32_RTIT_ADDR0_B"},
		{ 0x0582, "IA32_RTIT_ADDR1_A"},
		{ 0x0583, "IA32_RTIT_ADDR1_B"},
		{ 0x0600, "IA32_DS_AREA" },
		{ 0x064E, "MSR_PPERF"},
		{ 0x0653, "MSR_CORE_HDC_RESIDENCY"},
		{ 0x0690, "MSR_LASTBRANCH_16_FROM_IP" },
		{ 0x0691, "MSR_LASTBRANCH_17_FROM_IP" },
		{ 0x0692, "MSR_LASTBRANCH_18_FROM_IP" },
		{ 0x0693, "MSR_LASTBRANCH_19_FROM_IP" },
		{ 0x0694, "MSR_LASTBRANCH_20_FROM_IP" },
		{ 0x0695, "MSR_LASTBRANCH_21_FROM_IP" },
		{ 0x0696, "MSR_LASTBRANCH_22_FROM_IP" },
		{ 0x0697, "MSR_LASTBRANCH_23_FROM_IP" },
		{ 0x0698, "MSR_LASTBRANCH_24_FROM_IP" },
		{ 0x0699, "MSR_LASTBRANCH_25_FROM_IP" },
		{ 0x069A, "MSR_LASTBRANCH_26_FROM_IP" },
		{ 0x069B, "MSR_LASTBRANCH_27_FROM_IP" },
		{ 0x069C, "MSR_LASTBRANCH_28_FROM_IP" },
		{ 0x069D, "MSR_LASTBRANCH_29_FROM_IP" },
		{ 0x069E, "MSR_LASTBRANCH_30_FROM_IP" },
		{ 0x069F, "MSR_LASTBRANCH_31_FROM_IP" },
		{ 0x06d0, "MSR_LASTBRANCH_16_TO_IP" },
		{ 0x06d1, "MSR_LASTBRANCH_17_TO_IP" },
		{ 0x06d2, "MSR_LASTBRANCH_18_TO_IP" },
		{ 0x06d3, "MSR_LASTBRANCH_19_TO_IP" },
		{ 0x06d4, "MSR_LASTBRANCH_20_TO_IP" },
		{ 0x06d5, "MSR_LASTBRANCH_21_TO_IP" },
		{ 0x06d6, "MSR_LASTBRANCH_22_TO_IP" },
		{ 0x06d7, "MSR_LASTBRANCH_23_TO_IP" },
		{ 0x06d8, "MSR_LASTBRANCH_24_TO_IP" },
		{ 0x06d9, "MSR_LASTBRANCH_25_TO_IP" },
		{ 0x06da, "MSR_LASTBRANCH_26_TO_IP" },
		{ 0x06db, "MSR_LASTBRANCH_27_TO_IP" },
		{ 0x06dc, "MSR_LASTBRANCH_28_TO_IP" },
		{ 0x06dd, "MSR_LASTBRANCH_29_TO_IP" },
		{ 0x06de, "MSR_LASTBRANCH_30_TO_IP" },
		{ 0x06df, "MSR_LASTBRANCH_31_TO_IP" },
		{ 0x06E0, "IA32_TSC_DEADLINE"},
		{ 0x0771, "IA32_HWP_CAPABILITIES"},
		{ 0x0773, "IA32_HWP_INTERRUPT"},
		{ 0x0774, "IA32_HWP_REQUEST"},
		{ 0x0777, "IA32_HWP_STATUS"},
		{ 0x0D90, "IA32_BNDCFGS"},
		{ 0x0DA0, "IA32_XSS"},
		{ 0x0DB1, "IA32_PM_CTL1"},
		{ 0x0DB2, "IA32_THREAD_STALL"},
		{ 0x0DC0, "IA32_LBR_INFO_0"},
		{ 0x0DC1, "IA32_LBR_INFO_1"},
		{ 0x0DC2, "IA32_LBR_INFO_2"},
		{ 0x0DC3, "IA32_LBR_INFO_3"},
		{ 0x0DC4, "IA32_LBR_INFO_4"},
		{ 0x0DC5, "IA32_LBR_INFO_5"},
		{ 0x0DC6, "IA32_LBR_INFO_6"},
		{ 0x0DC7, "IA32_LBR_INFO_7"},
		{ 0x0DC8, "IA32_LBR_INFO_8"},
		{ 0x0DC9, "IA32_LBR_INFO_9"},
		{ 0x0DCA, "IA32_LBR_INFO_10"},
		{ 0x0DCB, "IA32_LBR_INFO_11"},
		{ 0x0DCC, "IA32_LBR_INFO_12"},
		{ 0x0DCD, "IA32_LBR_INFO_13"},
		{ 0x0DCE, "IA32_LBR_INFO_14"},
		{ 0x0DCF, "IA32_LBR_INFO_15"},
		{ 0x0DD0, "IA32_LBR_INFO_16"},
		{ 0x0DD1, "IA32_LBR_INFO_17"},
		{ 0x0DD2, "IA32_LBR_INFO_18"},
		{ 0x0DD3, "IA32_LBR_INFO_19"},
		{ 0x0DD4, "IA32_LBR_INFO_20"},
		{ 0x0DD5, "IA32_LBR_INFO_21"},
		{ 0x0DD6, "IA32_LBR_INFO_22"},
		{ 0x0DD7, "IA32_LBR_INFO_23"},
		{ 0x0DD8, "IA32_LBR_INFO_24"},
		{ 0x0DD9, "IA32_LBR_INFO_25"},
		{ 0x0DDA, "IA32_LBR_INFO_26"},
		{ 0x0DDB, "IA32_LBR_INFO_27"},
		{ 0x0DDC, "IA32_LBR_INFO_28"},
		{ 0x0DDD, "IA32_LBR_INFO_29"},
		{ 0x0DDE, "IA32_LBR_INFO_30"},
		{ 0x0DDF, "IA32_LBR_INFO_31"},
	};

/*
 * Intel® 64 and IA-32 Architecture Software Developer’s Manual
 * Volume 4: Model-Specific Registers
 * Order Number: 335592-070US
 * page 2-265 ... 2-286
 * page 2-297 ... 2-308
 *
 * The following two tables are for the Intel(R) Xeon(R) Processor Scalable
 * Family based on Skylake microarchitecture, 2nd generation Intel(R) Xeon(R)
 * Processor Scalable Family based on Cascade Lake product, and future Cooper
 * Lake product
 * family 6 model 85 (06_55h)
 */
	static const msr_entry_t model565x_global_msrs[] = {
		{ 0x004e, "MSR_PPIN_CTL" },
		{ 0x004f, "MSR_PPIN" },
		{ 0x00ce, "MSR_PLATFORM_INFO" },
		{ 0x0198, "IA32_PERF_STATUS" },
		{ 0x019c, "IA32_THERM_STATUS" },
		{ 0x01a2, "MSR_TEMPERATURE_TARGET" },
		{ 0x01ad, "MSR_TURBO_RATIO_LIMIT" },
		{ 0x01ae, "MSR_TURBO_RATIO_LIMIT_CORES" },
		{ 0x0284, "IA32_MC4_CTL2" },
		{ 0x0285, "IA32_MC5_CTL2" },
		{ 0x0286, "IA32_MC6_CTL2" },
		{ 0x0287, "IA32_MC7_CTL2" },
		{ 0x0288, "IA32_MC8_CTL2" },
		{ 0x0289, "IA32_MC9_CTL2" },
		{ 0x028a, "IA32_MC10_CTL2" },
		{ 0x028b, "IA32_MC11_CTL2" },
		{ 0x028c, "IA32_MC12_CTL2" },
		{ 0x028d, "IA32_MC13_CTL2" },
		{ 0x028e, "IA32_MC14_CTL2" },
		{ 0x028f, "IA32_MC15_CTL2" },
		{ 0x0290, "IA32_MC16_CTL2" },
		{ 0x0291, "IA32_MC17_CTL2" },
		{ 0x0292, "IA32_MC18_CTL2" },
		{ 0x0293, "IA32_MC19_CTL2" },
		{ 0x0300, "MSR_SGXOWNEREPOCH0" },
		{ 0x0301, "MSR_SGXOWNEREPOCH1" },
		{ 0x0410, "IA32_MC4_CTL" },
		{ 0x0411, "IA32_MC4_STATUS" },
		{ 0x0412, "IA32_MC4_ADDR" },
		{ 0x0413, "IA32_MC4_MISC" },
		{ 0x0414, "IA32_MC5_CTL" },
		{ 0x0415, "IA32_MC5_STATUS" },
		{ 0x0416, "IA32_MC5_ADDR" },
		{ 0x0417, "IA32_MC5_MISC" },
		{ 0x0418, "IA32_MC6_CTL" },
		{ 0x0419, "IA32_MC6_STATUS" },
		{ 0x041a, "IA32_MC6_ADDR" },
		{ 0x041b, "IA32_MC6_MISC" },
		{ 0x041c, "IA32_MC7_CTL" },
		{ 0x041d, "IA32_MC7_STATUS" },
		{ 0x041e, "IA32_MC7_ADDR" },
		{ 0x041f, "IA32_MC7_MISC" },
		{ 0x0420, "IA32_MC8_CTL" },
		{ 0x0421, "IA32_MC8_STATUS" },
		{ 0x0422, "IA32_MC8_ADDR" },
		{ 0x0423, "IA32_MC8_MISC" },
		{ 0x0424, "IA32_MC9_CTL" },
		{ 0x0425, "IA32_MC9_STATUS" },
		{ 0x0426, "IA32_MC9_ADDR" },
		{ 0x0427, "IA32_MC9_MISC" },
		{ 0x0428, "IA32_MC10_CTL" },
		{ 0x0429, "IA32_MC10_STATUS" },
		{ 0x042a, "IA32_MC10_ADDR" },
		{ 0x042b, "IA32_MC10_MISC" },
		{ 0x042c, "IA32_MC11_CTL" },
		{ 0x042d, "IA32_MC11_STATUS" },
		{ 0x042e, "IA32_MC11_ADDR" },
		{ 0x042f, "IA32_MC11_MISC" },
		{ 0x0430, "IA32_MC12_CTL" },
		{ 0x0431, "IA32_MC12_STATUS" },
		{ 0x0432, "IA32_MC12_ADDR" },
		{ 0x0433, "IA32_MC12_MISC" },
		{ 0x0434, "IA32_MC13_CTL" },
		{ 0x0435, "IA32_MC13_STATUS" },
		{ 0x0436, "IA32_MC13_ADDR" },
		{ 0x0437, "IA32_MC13_MISC" },
		{ 0x0438, "IA32_MC14_CTL" },
		{ 0x0439, "IA32_MC14_STATUS" },
		{ 0x043a, "IA32_MC14_ADDR" },
		{ 0x043b, "IA32_MC14_MISC" },
		{ 0x043c, "IA32_MC15_CTL" },
		{ 0x043d, "IA32_MC15_STATUS" },
		{ 0x043e, "IA32_MC15_ADDR" },
		{ 0x043f, "IA32_MC15_MISC" },
		{ 0x0440, "IA32_MC16_CTL" },
		{ 0x0441, "IA32_MC16_STATUS" },
		{ 0x0442, "IA32_MC16_ADDR" },
		{ 0x0443, "IA32_MC16_MISC" },
		{ 0x0444, "IA32_MC17_CTL" },
		{ 0x0445, "IA32_MC17_STATUS" },
		{ 0x0446, "IA32_MC17_ADDR" },
		{ 0x0447, "IA32_MC17_MISC" },
		{ 0x0448, "IA32_MC18_CTL" },
		{ 0x0449, "IA32_MC18_STATUS" },
		{ 0x044a, "IA32_MC18_ADDR" },
		{ 0x044b, "IA32_MC18_MISC" },
		{ 0x044c, "IA32_MC19_CTL" },
		{ 0x044b, "IA32_MC19_STATUS" },
		{ 0x044e, "IA32_MC19_ADDR" },
		{ 0x044f, "IA32_MC19_MISC" },
		{ 0x0606, "MSR_RAPL_POWER_UNIT" },
		{ 0x0618, "MSR_DRAM_POWER_LIMIT" },
		{ 0x0619, "MSR_DRAM_ENERGY_STATUS" },
		{ 0x061b, "MSR_DRAM_PERF_STATUS" },
		{ 0x061c, "MSR_DRAM_POWER_INFO" },
		{ 0x0620, "MSR_UNCORE_RATIO_LIMIT" },
		{ 0x0639, "MSR_PP0_ENERGY_STATUS" },
		{ 0x0638, "MSR_PP0_POWER_LIMIT" },
		{ 0x064d, "MSR_PLATFORM_ENERGY_COUNTER" },
		{ 0x064f, "MSR_CORE_PERF_LIMIT_REASONS" },
		{ 0x0652, "MSR_PKG_HDC_CONFIG" },
		{ 0x0655, "MSR_PKG_HDC_SHALLOW_RESIDENCY" },
		{ 0x0656, "MSR_PKG_HDC_DEEP_RESIDENCY" },
		{ 0x0658, "MSR_WEIGHTED_CORE_C0" },
		{ 0x0659, "MSR_ANY_CORE_C0" },
		{ 0x065a, "MSR_ANY_GFXE_C0" },
		{ 0x065b, "MSR_CORE_GFXE_OVERLAP_C0" },
		{ 0x065c, "MSR_PLATFORM_POWER_LIMIT" },
		{ 0x06b0, "MSR_GRAPHICS_PERF_LIMIT_REASONS" },
		{ 0x06b1, "MSR_RING_PERF_LIMIT_REASONS" },
		{ 0x0770, "IA32_PM_ENABLE" },
		{ 0x0db0, "IA32_PKG_HDC_CTL" },
		{ 0x0c90, "IA32_L3_QOS_MASK_0" },
		{ 0x0c91, "IA32_L3_QOS_MASK_1" },
		{ 0x0c92, "IA32_L3_QOS_MASK_2" },
		{ 0x0c93, "IA32_L3_QOS_MASK_3" },
		{ 0x0c94, "IA32_L3_QOS_MASK_4" },
		{ 0x0c95, "IA32_L3_QOS_MASK_5" },
		{ 0x0c96, "IA32_L3_QOS_MASK_6" },
		{ 0x0c97, "IA32_L3_QOS_MASK_7" },
		{ 0x0c98, "IA32_L3_QOS_MASK_8" },
		{ 0x0c99, "IA32_L3_QOS_MASK_9" },
		{ 0x0c9a, "IA32_L3_QOS_MASK_10" },
		{ 0x0c9b, "IA32_L3_QOS_MASK_11" },
		{ 0x0c9c, "IA32_L3_QOS_MASK_12" },
		{ 0x0c9d, "IA32_L3_QOS_MASK_13" },
		{ 0x0c9e, "IA32_L3_QOS_MASK_14" },
		{ 0x0c9f, "IA32_L3_QOS_MASK_15" },
	};

	static const msr_entry_t model565x_per_core_msrs[] = {
		{ 0x0000, "IA32_P5_MC_ADDR" },
		{ 0x0001, "IA32_P5_MC_TYPE" },
		{ 0x0006, "IA32_MONITOR_FILTER_SIZE" },
		{ 0x0010, "IA32_TIME_STAMP_COUNTER" },
		{ 0x0017, "IA32_PLATFORM_ID" },
		{ 0x001b, "IA32_APIC_BASE" },
		{ 0x0034, "MSR_SMI_COUNT" },
		{ 0x003a, "IA32_FEATURE_CONTROL" },
		{ 0x008b, "IA32_BIOS_SIGN_ID" },
		{ 0x00c1, "IA32_PMC0" },
		{ 0x00c2, "IA32_PMC1" },
		{ 0x00c3, "IA32_PMC2" },
		{ 0x00c4, "IA32_PMC3" },
		{ 0x00e2, "MSR_PKG_CST_CONFIG_CONTROL" },
		{ 0x00e4, "MSR_PMG_IO_CAPTURE_BASE" },
		{ 0x00e7, "IA32_MPERF" },
		{ 0x00e8, "IA32_APERF" },
		{ 0x00fe, "IA32_MTRRCAP" },
		{ 0x013c, "MSR_FEATURE_CONFIG" },
		{ 0x0174, "IA32_SYSENTER_CS" },
		{ 0x0175, "IA32_SYSENTER_ESP" },
		{ 0x0176, "IA32_SYSENTER_EIP" },
		{ 0x0179, "IA32_MCG_CAP" },
		{ 0x017a, "IA32_MCG_STATUS" },
		{ 0x017d, "MSR_SMM_MCA_CAP" },
		{ 0x0186, "IA32_PERFEVTSEL0" },
		{ 0x0187, "IA32_PERFEVTSEL1" },
		{ 0x0188, "IA32_PERFEVTSEL2" },
		{ 0x0189, "IA32_PERFEVTSEL3" },
		{ 0x019b, "IA32_THERM_INTERRUPT" },
		{ 0x0199, "IA32_PERF_CTL" },
		{ 0x019a, "IA32_CLOCK_MODULATION" },
		{ 0x01a0, "IA32_MISC_ENABLE" },
		{ 0x01a4, "IA32_MISC_FEATURE_CONTROL" },
		{ 0x01a6, "MSR_OFFCORE_RSP_0" },
		{ 0x01a7, "MSR_OFFCORE_RSP_1" },
		{ 0x01c8, "MSR_LBR_SELECT" },
		{ 0x01c9, "MSR_LASTBRANCH_TOS" },
		{ 0x01d9, "IA32_DEBUGCTL" },
		{ 0x01dd, "MSR_LER_FROM_LIP" },
		{ 0x01de, "MSR_LER_TO_LIP" },
		{ 0x01f2, "IA32_SMRR_PHYSBASE" },
		{ 0x01f3, "IA32_SMRR_PHYSMASK" },
		{ 0x01fc, "MSR_POWER_CTL" },
		{ 0x0200, "IA32_MTRR_PHYSBASE0" },
		{ 0x0201, "IA32_MTRR_PHYSBASE0" },
		{ 0x0202, "IA32_MTRR_PHYSBASE1" },
		{ 0x0203, "IA32_MTRR_PHYSBASE1" },
		{ 0x0204, "IA32_MTRR_PHYSBASE2" },
		{ 0x0205, "IA32_MTRR_PHYSBASE2" },
		{ 0x0206, "IA32_MTRR_PHYSBASE3" },
		{ 0x0207, "IA32_MTRR_PHYSBASE3" },
		{ 0x0208, "IA32_MTRR_PHYSBASE4" },
		{ 0x0209, "IA32_MTRR_PHYSBASE4" },
		{ 0x020a, "IA32_MTRR_PHYSBASE5" },
		{ 0x020b, "IA32_MTRR_PHYSBASE5" },
		{ 0x020c, "IA32_MTRR_PHYSBASE6" },
		{ 0x020d, "IA32_MTRR_PHYSBASE6" },
		{ 0x020e, "IA32_MTRR_PHYSBASE7" },
		{ 0x020f, "IA32_MTRR_PHYSBASE7" },
		{ 0x0210, "IA32_MTRR_PHYSBASE8" },
		{ 0x0211, "IA32_MTRR_PHYSBASE8" },
		{ 0x0212, "IA32_MTRR_PHYSBASE9" },
		{ 0x0213, "IA32_MTRR_PHYSBASE9" },
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
		{ 0x0277, "IA32_PAT" },
		{ 0x0280, "IA32_MC0_CTL2" },
		{ 0x0281, "IA32_MC1_CTL2" },
		{ 0x0282, "IA32_MC2_CTL2" },
		{ 0x0283, "IA32_MC3_CTL2" },
		{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
		{ 0x0309, "IA32_FIXED_CTR0" },
		{ 0x030a, "IA32_FIXED_CTR1" },
		{ 0x030b, "IA32_FIXED_CTR2" },
		{ 0x0345, "IA32_PERF_CAPABILITIES" },
		{ 0x038d, "IA32_FIXED_CTR_CTRL" },
		{ 0x038e, "IA32_PERF_GLOBAL_STATUS" },
		{ 0x038f, "IA32_PERF_GLOBAL_CTRL" },
		{ 0x0390, "IA32_PERF_GLOBAL_STATUS_RESET" },
		{ 0x0391, "IA32_PERF_GLOBAL_STATUS_SET" },
		{ 0x0392, "IA32_PERF_GLOBAL_INUSE" },
		{ 0x03f1, "MSR_PEBS_ENABLE" },
		{ 0x03f6, "MSR_PEBS_LD_LAT" },
		{ 0x03f7, "MSR_PEBS_FRONTEND" },
		{ 0x03fc, "MSR_CORE_C3_RESIDENCY" },
		{ 0x03fd, "MSR_CORE_C6_RESIDENCY" },
		{ 0x03fe, "MSR_CORE_C7_RESIDENCY" },
		{ 0x0400, "IA32_MC0_CTL" },
		{ 0x0401, "IA32_MC0_STATUS" },
		{ 0x0402, "IA32_MC0_ADDR" },
		{ 0x0403, "IA32_MC0_MISC" },
		{ 0x0404, "IA32_MC1_CTL" },
		{ 0x0405, "IA32_MC1_STATUS" },
		{ 0x0406, "IA32_MC1_ADDR" },
		{ 0x0407, "IA32_MC1_MISC" },
		{ 0x0408, "IA32_MC2_CTL" },
		{ 0x0409, "IA32_MC2_STATUS" },
		{ 0x040a, "IA32_MC2_ADDR" },
		{ 0x040b, "IA32_MC2_MISC" },
		{ 0x040c, "IA32_MC3_CTL" },
		{ 0x040d, "IA32_MC3_STATUS" },
		{ 0x040e, "IA32_MC3_ADDR" },
		{ 0x040f, "IA32_MC3_MISC" },
		{ 0x0480, "IA32_VMX_BASIC" },
		{ 0x0481, "IA32_VMX_PINBASED_CTLS" },
		{ 0x0482, "IA32_VMX_PROCBASED_CTLS" },
		{ 0x0483, "IA32_VMX_EXIT_CTLS" },
		{ 0x0484, "IA32_VMX_ENTRY_CTLS" },
		{ 0x0485, "IA32_VMX_MISC" },
		{ 0x0486, "IA32_VMX_CR0_FIXED0" },
		{ 0x0487, "IA32_VMX_CR0_FIXED1" },
		{ 0x0488, "IA32_VMX_CR4_FIXED0" },
		{ 0x0489, "IA32_VMX_CR4_FIXED1" },
		{ 0x048a, "IA32_VMX_VMCS_ENUM" },
		{ 0x048b, "IA32_VMX_PROCBASED_CTLS2" },
		{ 0x048c, "IA32_VMX_EPT_VPID_ENUM" },
		{ 0x048d, "IA32_VMX_TRUE_PINBASED_CTLS" },
		{ 0x048e, "IA32_VMX_TRUE_PROCBASED_CTLS" },
		{ 0x048f, "IA32_VMX_TRUE_EXIT_CTLS" },
		{ 0x0490, "IA32_VMX_TRUE_ENTRY_CTLS" },
		{ 0x04c1, "IA32_A_PMC0" },
		{ 0x04c2, "IA32_A_PMC1" },
		{ 0x04c3, "IA32_A_PMC2" },
		{ 0x04c4, "IA32_A_PMC3" },
		{ 0x04c5, "IA32_A_PMC4" },
		{ 0x04c6, "IA32_A_PMC5" },
		{ 0x04c7, "IA32_A_PMC6" },
		{ 0x04c8, "IA32_A_PMC7" },
		{ 0x0500, "IA32_SGX_SVN_STATUS" },
		{ 0x0560, "IA32_RTIT_OUTPUT_BASE" },
		{ 0x0561, "IA32_RTIT_OUTPUT_MASK_PTRS" },
		{ 0x0570, "IA32_RTIT_CTL" },
		{ 0x0571, "IA32_RTIT_STATUS" },
		{ 0x0572, "IA32_RTIT_CR3_MATCH" },
		{ 0x0580, "IA32_RTIT_ADDR0_A" },
		{ 0x0581, "IA32_RTIT_ADDR0_B" },
		{ 0x0582, "IA32_RTIT_ADDR1_A" },
		{ 0x0583, "IA32_RTIT_ADDR1_B" },
		{ 0x0600, "IA32_DS_AREA" },
		{ 0x064e, "MSR_PPERF" },
		{ 0x0653, "MSR_CORE_HDC_RESIDENCY" },
		{ 0x0690, "MSR_LASTBRANCH_16_FROM_IP" },
		{ 0x0691, "MSR_LASTBRANCH_17_FROM_IP" },
		{ 0x0692, "MSR_LASTBRANCH_18_FROM_IP" },
		{ 0x0693, "MSR_LASTBRANCH_19_FROM_IP" },
		{ 0x0694, "MSR_LASTBRANCH_20_FROM_IP" },
		{ 0x0695, "MSR_LASTBRANCH_21_FROM_IP" },
		{ 0x0696, "MSR_LASTBRANCH_22_FROM_IP" },
		{ 0x0697, "MSR_LASTBRANCH_23_FROM_IP" },
		{ 0x0698, "MSR_LASTBRANCH_24_FROM_IP" },
		{ 0x0699, "MSR_LASTBRANCH_25_FROM_IP" },
		{ 0x069a, "MSR_LASTBRANCH_26_FROM_IP" },
		{ 0x069b, "MSR_LASTBRANCH_27_FROM_IP" },
		{ 0x069c, "MSR_LASTBRANCH_28_FROM_IP" },
		{ 0x069d, "MSR_LASTBRANCH_29_FROM_IP" },
		{ 0x069e, "MSR_LASTBRANCH_30_FROM_IP" },
		{ 0x069f, "MSR_LASTBRANCH_31_FROM_IP" },
		{ 0x06d0, "MSR_LASTBRANCH_16_TO_IP" },
		{ 0x06d1, "MSR_LASTBRANCH_17_TO_IP" },
		{ 0x06d2, "MSR_LASTBRANCH_18_TO_IP" },
		{ 0x06d3, "MSR_LASTBRANCH_19_TO_IP" },
		{ 0x06d4, "MSR_LASTBRANCH_20_TO_IP" },
		{ 0x06d5, "MSR_LASTBRANCH_21_TO_IP" },
		{ 0x06d6, "MSR_LASTBRANCH_22_TO_IP" },
		{ 0x06d7, "MSR_LASTBRANCH_23_TO_IP" },
		{ 0x06d8, "MSR_LASTBRANCH_24_TO_IP" },
		{ 0x06d9, "MSR_LASTBRANCH_25_TO_IP" },
		{ 0x06da, "MSR_LASTBRANCH_26_TO_IP" },
		{ 0x06db, "MSR_LASTBRANCH_27_TO_IP" },
		{ 0x06dc, "MSR_LASTBRANCH_28_TO_IP" },
		{ 0x06dd, "MSR_LASTBRANCH_29_TO_IP" },
		{ 0x06de, "MSR_LASTBRANCH_30_TO_IP" },
		{ 0x06df, "MSR_LASTBRANCH_31_TO_IP" },
		{ 0x06e0, "IA32_TSC_DEADLINE" },
		{ 0x0771, "IA32_HWP_CAPABILITIES" },
		{ 0x0773, "IA32_HWP_INTERRUPT" },
		{ 0x0774, "IA32_HWP_REQUEST" },
		{ 0x0777, "IA32_HWP_STATUS" },
		{ 0x0c8d, "IA32_QM_EVTSEL" },
		{ 0x0c8f, "IA32_PQR_ASSOC" },
		{ 0x0d90, "IA32_BNDCFGS" },
		{ 0x0da0, "IA32_XSS" },
		{ 0x0db1, "IA32_PM_CTL1" },
		{ 0x0db2, "IA32_THREAD_STALL" },
		{ 0x0dc0, "MSR_LBR_INFO_0" },
		{ 0x0dc1, "MSR_LBR_INFO_1" },
		{ 0x0dc2, "MSR_LBR_INFO_2" },
		{ 0x0dc3, "MSR_LBR_INFO_3" },
		{ 0x0dc4, "MSR_LBR_INFO_4" },
		{ 0x0dc5, "MSR_LBR_INFO_5" },
		{ 0x0dc6, "MSR_LBR_INFO_6" },
		{ 0x0dc7, "MSR_LBR_INFO_7" },
		{ 0x0dc8, "MSR_LBR_INFO_8" },
		{ 0x0dc9, "MSR_LBR_INFO_9" },
		{ 0x0dca, "MSR_LBR_INFO_10" },
		{ 0x0dcb, "MSR_LBR_INFO_11" },
		{ 0x0dcc, "MSR_LBR_INFO_12" },
		{ 0x0dcd, "MSR_LBR_INFO_13" },
		{ 0x0dce, "MSR_LBR_INFO_14" },
		{ 0x0dcf, "MSR_LBR_INFO_15" },
		{ 0x0dd0, "MSR_LBR_INFO_16" },
		{ 0x0dd1, "MSR_LBR_INFO_17" },
		{ 0x0dd2, "MSR_LBR_INFO_18" },
		{ 0x0dd3, "MSR_LBR_INFO_19" },
		{ 0x0dd4, "MSR_LBR_INFO_20" },
		{ 0x0dd5, "MSR_LBR_INFO_21" },
		{ 0x0dd6, "MSR_LBR_INFO_22" },
		{ 0x0dd7, "MSR_LBR_INFO_23" },
		{ 0x0dd8, "MSR_LBR_INFO_24" },
		{ 0x0dd9, "MSR_LBR_INFO_25" },
		{ 0x0ddA, "MSR_LBR_INFO_26" },
		{ 0x0ddB, "MSR_LBR_INFO_27" },
		{ 0x0ddc, "MSR_LBR_INFO_28" },
		{ 0x0ddd, "MSR_LBR_INFO_29" },
		{ 0x0dde, "MSR_LBR_INFO_30" },
		{ 0x0ddf, "MSR_LBR_INFO_31" },
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
		{ 0x00f20, modelf2x_global_msrs, ARRAY_SIZE(modelf2x_global_msrs), modelf2x_per_core_msrs, ARRAY_SIZE(modelf2x_per_core_msrs) },
		{ 0x00f40, modelf4x_global_msrs, ARRAY_SIZE(modelf4x_global_msrs), modelf4x_per_core_msrs, ARRAY_SIZE(modelf4x_per_core_msrs) },
		{ 0x00f60, modelf6x_global_msrs, ARRAY_SIZE(modelf6x_global_msrs), modelf6x_per_core_msrs, ARRAY_SIZE(modelf6x_per_core_msrs) },
		{ 0x106c0, model6_atom_global_msrs, ARRAY_SIZE(model6_atom_global_msrs), model6_atom_per_core_msrs, ARRAY_SIZE(model6_atom_per_core_msrs) },
		{ 0x20650, model20650_global_msrs, ARRAY_SIZE(model20650_global_msrs), model20650_per_core_msrs, ARRAY_SIZE(model20650_per_core_msrs) },
		{ 0x906e0, model96ex_global_msrs, ARRAY_SIZE(model96ex_global_msrs), model96ex_per_core_msrs, ARRAY_SIZE(model96ex_per_core_msrs) },
		{ 0x50650, model565x_global_msrs, ARRAY_SIZE(model565x_global_msrs), model565x_per_core_msrs, ARRAY_SIZE(model565x_per_core_msrs) },

		{ CPUID_BAYTRAIL, silvermont_global_msrs, ARRAY_SIZE(silvermont_global_msrs), silvermont_per_core_msrs, ARRAY_SIZE(silvermont_per_core_msrs) }, /* Baytrail */

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

	const unsigned int cores_range_max_limit = get_number_of_cores() - 1;
	if (range_end > cores_range_max_limit) {
		if (range_end != UINT_MAX)
			printf("Warning: the range exceeds the maximum core number %d!\n",
				cores_range_max_limit);
		range_end = cores_range_max_limit;
	}

	for (core = range_start; core <= range_end; core++) {
#ifndef __DARWIN__
		char msrfilename[64];
		memset(msrfilename, 0, 64);
		sprintf(msrfilename, "/dev/cpu/%u/msr", core);

		fd_msr = open(msrfilename, O_RDWR);

		/* If the file is not there, we're probably through. No error,
		 * since we successfully opened /dev/cpu/0/msr before.
		 */
		if (fd_msr < 0)
			break;
#endif
		if (cpu->num_per_core_msrs)
			printf("\n====================== UNIQUE MSRs  (core %u) ======================\n", core);

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
