/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>

#include "amdtool.h"
#include "smn.h"

#ifdef __x86_64__
# define BREG	"%%rbx"
#else
# define BREG	"%%ebx"
#endif

typedef struct {
	uint32_t number;
	char *name;
} msr_entry_t;

int fd_msr;

uint32_t cpuid(uint32_t eax)
{
	uint32_t ret;

#if defined(__PIC__) || defined(__DARWIN__) && !defined(__LP64__)
	asm volatile (
		"push " BREG "\n\t"
		"cpuid\n\t"
		"pop " BREG "\n\t"
		: "=a" (ret) : "a" (eax) : "%ecx", "%edx"
	);
#else
	asm ("cpuid" : "=a" (ret) : "a" (eax) : "%ebx", "%ecx", "%edx");
#endif

	return ret;
}

static inline cpuid_result_t cpuid_ext(uint32_t eax, unsigned int ecx)
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
			: "0" (eax), "2" (ecx)
			: "edi");
#endif
	return result;
}

#ifndef __DARWIN__
int msr_readerror = 0;

static msr_t rdmsr(unsigned int addr)
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

static bool is_sme_supported(void)
{
	cpuid_result_t cpuid_regs;

	if (cpuid(0x80000000) < 0x8000001f)
		return false;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return !!(cpuid_regs.eax & 1);
}

static bool is_sme_enabled(int cpunum)
{
	msr_t data;
	data = rdmsr_from_cpu(cpunum, 0xC0010010);
	return !!(data.lo & (1 << 23));
}

#endif

static int print_sme(void)
{
	int error = -1;
#ifndef __DARWIN__
	int ncpus = get_number_of_cpus();
	int i = 0;
	bool sme_supported;

	printf("\n============= Dumping AMD SME status =============\n");

	if (ncpus < 1) {
		perror("Failed to get number of CPUs");
		error = -1;
	} else {
		sme_supported = is_sme_supported();
		for (i = 0; i < ncpus ; i++) {

			printf("------------- CPU %d ----------------\n", i);
			printf("SME supported             : %s\n",
					sme_supported ? "YES" : "NO");
			if (sme_supported)
				printf("SME enabled               : %s\n",
					is_sme_enabled(i) ? "YES" : "NO");
		}
		error = 0;
	}
	printf("====================================================\n\n");
#endif
	return error;
}

static bool is_sev_supported(void)
{
	cpuid_result_t cpuid_regs;

	if (cpuid(0x80000000) < 0x8000001f)
		return false;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return !!(cpuid_regs.eax & 2);
}

static bool is_sev_es_supported(void)
{
	cpuid_result_t cpuid_regs;

	if (cpuid(0x80000000) < 0x8000001f)
		return false;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return !!(cpuid_regs.eax & 8);
}

static bool is_sev_snp_supported(void)
{
	cpuid_result_t cpuid_regs;

	if (cpuid(0x80000000) < 0x8000001f)
		return false;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return !!(cpuid_regs.eax & 0x10);
}

static bool is_sev_enabled(int cpunum)
{
	msr_t data;

	data = rdmsr_from_cpu(cpunum, 0xC0010131);
	return !!(data.lo & 1);
}

static bool is_sev_es_enabled(int cpunum)
{
	msr_t data;

	data = rdmsr_from_cpu(cpunum, 0xC0010131);
	return !!(data.lo & 2);
}

static bool is_sev_snp_enabled(int cpunum)
{
	msr_t data;

	data = rdmsr_from_cpu(cpunum, 0xC0010131);
	return !!(data.lo & 4);
}

static unsigned int get_sev_max_guest_num(void)
{
	cpuid_result_t cpuid_regs;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return cpuid_regs.ecx;
}

static unsigned int get_sev_min_asid(void)
{
	cpuid_result_t cpuid_regs;

	cpuid_regs = cpuid_ext(0x8000001f, 0x0);
	return cpuid_regs.edx;
}

static int print_sev(void)
{
	int error = -1;
#ifndef __DARWIN__
	int ncpus = get_number_of_cpus();
	int i = 0;
	bool sev_supported, sev_es_supported, sev_snp_supported;
	unsigned int max_guest, min_asid;

	printf("\n============= Dumping AMD SEV status =============\n");

	if (ncpus < 1) {
		perror("Failed to get number of CPUs");
		error = -1;
	} else {
		/*
		 * The following use CPUID, so should be the same for each core
		 * in the scope of processor.
		 */
		sev_supported = is_sev_supported();
		sev_es_supported = is_sev_es_supported();
		sev_snp_supported = is_sev_snp_supported();
		if (sev_supported) {
			max_guest = get_sev_max_guest_num();
			min_asid = get_sev_min_asid();
		}

		for (i = 0; i < ncpus ; i++) {
			printf("------------- CPU %d ----------------\n", i);
			printf("SEV supported             : %s\n",
					sev_supported ? "YES" : "NO");
			if (sev_supported) {
				printf("Max SEV encrypted guests  : %u\n",
					max_guest);
				printf("Min SEV ASID              : %u\n",
					min_asid);
				printf("SEV enabled               : %s\n",
					is_sev_enabled(i) ? "YES" : "NO");
			}
			printf("SEV-ES supported          : %s\n",
					sev_es_supported ? "YES" : "NO");
			if (sev_es_supported)
				printf("SEV-ES enabled            : %s\n",
					is_sev_es_enabled(i) ? "YES" : "NO");
			printf("SEV-SNP supported         : %s\n",
					sev_snp_supported ? "YES" : "NO");
			if (sev_snp_supported)
				printf("SEV-SNP enabled           : %s\n",
					is_sev_snp_enabled(i) ? "YES" : "NO");
		}
		error = 0;
	}
	printf("====================================================");
#endif
	return error;
}

static void get_cpu_brand_string(char *cpu_string)
{
	u32 tmp[13];
	cpuid_result_t res;
	const char *str = "Unknown Processor Name";
	int i, j;

	if (cpuid(0x80000000) >= 0x80000004) {
		j = 0;
		for (i = 0; i < 3; i++) {
			res = cpuid_ext(0x80000002 + i, 0x0);
			tmp[j++] = res.eax;
			tmp[j++] = res.ebx;
			tmp[j++] = res.ecx;
			tmp[j++] = res.edx;
		}
		tmp[12] = 0;
		str = (const char *)tmp;
	}

	strcpy(cpu_string, str);
}

#define CPU_BRAND_STRING_LEN 48

static int print_cpu_features(void)
{
	int error = -1;
#ifndef __DARWIN__
	cpuid_result_t cpuid_regs;

	printf("\n============= AMD CPU features =============\n");

	if (cpuid(0x80000000) >= 0x80000001) {
		cpuid_regs = cpuid_ext(0x80000001, 0x0);
		printf("SVM supported             : %s\n",
			cpuid_regs.ecx & (1 << 2) ? "YES" : "NO");
		printf("SKINIT supported          : %s\n",
			cpuid_regs.ecx & (1 << 12) ? "YES" : "NO");
	}

	error = 0;

	printf("====================================================\n");
#endif
	return error;
}

int print_cpu_info(void)
{
	int ret;
	char brand_string[CPU_BRAND_STRING_LEN + 1];

	get_cpu_brand_string(brand_string);
	printf("CPU brand string: %s\n", brand_string);

	ret = print_cpu_features();
	ret += print_sme();
	ret += print_sev();

	return ret;
}

static const msr_entry_t common_msrs[] = {
	{ 0x001b, "IA32_APIC_BASE" },
	{ 0x008b, "MICROCODE_PATCH_LEVEL" },
	{ 0x00fe, "IA32_MTRRCAP" },
	{ 0x0179, "IA32_MCG_CAP" },
	{ 0x017a, "IA32_MCG_STATUS" },
	{ 0x017b, "IA32_MCG_CONTROL" },
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
	{ 0x0277, "IA32_PAT" },
	{ 0x02ff, "IA32_MTRR_DEF_TYPE" },
	{ 0xc0000080, "EFER" },
	{ 0xc0010010, "SYS_CFG" },
	{ 0xc0010015, "HWCR" },
	{ 0xc0010016, "IORR_BASE0" },
	{ 0xc0010017, "IORR_MASK0" },
	{ 0xc0010018, "IORR_BASE1" },
	{ 0xc0010019, "IORR_MASK1" },
	{ 0xc001001a, "TOP_MEM" },
	{ 0xc001001d, "TOM2" },
	{ 0xc0010030, "PROCESSOR_NAME_STRING0" },
	{ 0xc0010031, "PROCESSOR_NAME_STRING1" },
	{ 0xc0010032, "PROCESSOR_NAME_STRING2" },
	{ 0xc0010033, "PROCESSOR_NAME_STRING3" },
	{ 0xc0010034, "PROCESSOR_NAME_STRING4" },
	{ 0xc0010035, "PROCESSOR_NAME_STRING5" },
	{ 0xc0010050, "SMI_ON_IO_TRAP0" },
	{ 0xc0010051, "SMI_ON_IO_TRAP1" },
	{ 0xc0010052, "SMI_ON_IO_TRAP2" },
	{ 0xc0010053, "SMI_ON_IO_TRAP3" },
	{ 0xc0010054, "SMI_ON_IO_TRAP_CTL_STS" },
	{ 0xc0010056, "SMI_TRIGGER_IO_CYCLE" },
	{ 0xc0010058, "MMCONF_BASE_ADDR" },
	{ 0xc0010061, "PSTATE_CURRENT_LIMIT" },
	{ 0xc0010062, "PSTATE_CNTRL" },
	{ 0xc0010063, "PSTATE_STATUS" },
	{ 0xc0010064, "PSTATE0_DEF" },
	{ 0xc0010065, "PSTATE1_DEF" },
	{ 0xc0010066, "PSTATE2_DEF" },
	{ 0xc0010067, "PSTATE3_DEF" },
	{ 0xc0010068, "PSTATE4_DEF" },
	{ 0xc0010069, "PSTATE5_DEF" },
	{ 0xc001006a, "PSTATE6_DEF" },
	{ 0xc001006b, "PSTATE7_DEF" },
	{ 0xc0010073, "CSTATE_BASE_ADDR" },
	{ 0xc0010074, "CPU_WDT_CFG" },
	{ 0xc0010111, "SMM_BASE" },
	{ 0xc0010112, "SMM_TSEG_BASE" },
	{ 0xc0010113, "SMM_TSEG_MASK" },
	{ 0xc0010114, "VM_CR" },
	{ 0xc0010118, "SVM_LOCK_KEY" },
	{ 0xc0010119, "SMM_LOCK_KEY" },
	{ 0xc0010131, "SEV_STATUS" },
	{ 0xc0010140, "OSVW_ID_LENGTH" },
	{ 0xc0010141, "OSVW_STATUS" },
	{ 0xc0010292, "PWR_MNGMNT_MISC" },
	{ 0xc0010293, "HW_PSTATE_STATUS" },
	{ 0xc0010294, "CSTATE_POLICY" },
	{ 0xc0010296, "CSTATE_CONFIG" },
	{ 0xc0010297, "PWR_MNGMNT_DEFAULT" },
	{ 0xc0010299, "RAPL_PWR_UNIT" },
	{ 0xc001029a, "CORE_ENERGY_STS" },
	{ 0xc001029b, "PKG_ENERGY_STS" },
	{ 0xc00102b0, "CPPC_CAP1" },
	{ 0xc00102b1, "CPPC_ENABLE" },
	{ 0xc00102b2, "CPPC_CAP2" },
	{ 0xc00102b3, "CPPC_REQUEST" },
	{ 0xc00102b4, "CPPC_STATUS" },
	{ 0xc0011002, "CPUID_7_FEATURES" },
	{ 0xc0011003, "CPUID_PWR_THERM" },
	{ 0xc0011004, "CPUID_FEATURES" },
	{ 0xc0011005, "CPUID_EXT_FEATURES" },
	{ 0xc001100c, "NODE_ID/SCRATCH" },
	{ 0xC0011020, "LS_CFG" },
	{ 0xC0011021, "IC_CFG" },
	{ 0xc0011022, "DC_CFG" },
	{ 0xc0011023, "TW_CFG" },
	{ 0xc0011028, "FP_CFG" },
	{ 0xc0011029, "ME_CFG" },
	{ 0xc001102a, "BU_CFG2" },
	{ 0xc001102b, "L2_PFCFG" },
	{ 0xC001102d, "LS_CFG2" },
	{ 0xc001102e, "BP_CFG" },
	{ 0xc00110a2, "PSP_ADDR" },
};

int print_amd_msrs(void)
{
	unsigned int i, id;
	msr_t msr;

	typedef struct {
		unsigned int model;
		const msr_entry_t *global_msrs;
		unsigned int num_global_msrs;
	} cpu_t;

	cpu_t cpulist[] = {
		{ CPUID_TURIN_C1, common_msrs, ARRAY_SIZE(common_msrs) },
		{ CPUID_PHOENIX_A2, common_msrs, ARRAY_SIZE(common_msrs) },
	};

	cpu_t *cpu = NULL;

	/* Get CPU family, model and stepping */
	id = cpuid(1);
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

	printf("\n===================== MSRs =====================\n");

	for (i = 0; i < cpu->num_global_msrs; i++) {
		msr = rdmsr(cpu->global_msrs[i].number);
		printf(" MSR 0x%08X = 0x%08X:0x%08X (%s)\n",
		       cpu->global_msrs[i].number, msr.hi, msr.lo,
		       cpu->global_msrs[i].name);
	}

#ifndef __DARWIN__
	close(fd_msr);

	if (msr_readerror)
		printf("\n(*) Some MSRs could not be read. The marked values are unreliable.\n");
#endif
	return 0;
}
