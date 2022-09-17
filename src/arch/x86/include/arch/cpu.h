/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_CPU_H
#define ARCH_CPU_H

#include <types.h>
#include <arch/cpuid.h>

/*
 * EFLAGS bits
 */
#define X86_EFLAGS_CF	0x00000001 /* Carry Flag */
#define X86_EFLAGS_PF	0x00000004 /* Parity Flag */
#define X86_EFLAGS_AF	0x00000010 /* Auxiliary carry Flag */
#define X86_EFLAGS_ZF	0x00000040 /* Zero Flag */
#define X86_EFLAGS_SF	0x00000080 /* Sign Flag */
#define X86_EFLAGS_TF	0x00000100 /* Trap Flag */
#define X86_EFLAGS_IF	0x00000200 /* Interrupt Flag */
#define X86_EFLAGS_DF	0x00000400 /* Direction Flag */
#define X86_EFLAGS_OF	0x00000800 /* Overflow Flag */
#define X86_EFLAGS_IOPL	0x00003000 /* IOPL mask */
#define X86_EFLAGS_NT	0x00004000 /* Nested Task */
#define X86_EFLAGS_RF	0x00010000 /* Resume Flag */
#define X86_EFLAGS_VM	0x00020000 /* Virtual Mode */
#define X86_EFLAGS_AC	0x00040000 /* Alignment Check */
#define X86_EFLAGS_VIF	0x00080000 /* Virtual Interrupt Flag */
#define X86_EFLAGS_VIP	0x00100000 /* Virtual Interrupt Pending */
#define X86_EFLAGS_ID	0x00200000 /* CPUID detection flag */

static inline unsigned int cpuid_get_max_func(void)
{
	return cpuid_eax(0);
}

#define X86_VENDOR_INVALID    0
#define X86_VENDOR_INTEL      1
#define X86_VENDOR_CYRIX      2
#define X86_VENDOR_AMD        3
#define X86_VENDOR_UMC        4
#define X86_VENDOR_NEXGEN     5
#define X86_VENDOR_CENTAUR    6
#define X86_VENDOR_RISE       7
#define X86_VENDOR_TRANSMETA  8
#define X86_VENDOR_NSC        9
#define X86_VENDOR_SIS       10
#define X86_VENDOR_HYGON     11
#define X86_VENDOR_ANY     0xfe
#define X86_VENDOR_UNKNOWN 0xff

#define CPUID_FEATURE_PAE (1 << 6)
#define CPUID_FEATURE_PSE36 (1 << 17)
#define CPUID_FEAURE_HTT (1 << 28)

// Intel leaf 0x4, AMD leaf 0x8000001d EAX

#define CPUID_CACHE(x, res) \
	(((res) >> CPUID_CACHE_##x##_SHIFT) & CPUID_CACHE_##x##_MASK)

#define CPUID_CACHE_SHARING_CACHE_SHIFT 14
#define CPUID_CACHE_SHARING_CACHE_MASK 0xfff
#define CPUID_CACHE_SHARING_CACHE(res) CPUID_CACHE(SHARING_CACHE, (res).eax)

#define CPUID_CACHE_FULL_ASSOC_SHIFT 9
#define CPUID_CACHE_FULL_ASSOC_MASK 0x1
#define CPUID_CACHE_FULL_ASSOC(res) CPUID_CACHE(FULL_ASSOC, (res).eax)

#define CPUID_CACHE_SELF_INIT_SHIFT 8
#define CPUID_CACHE_SELF_INIT_MASK 0x1
#define CPUID_CACHE_SELF_INIT(res) CPUID_CACHE(SELF_INIT, (res).eax)

#define CPUID_CACHE_LEVEL_SHIFT 5
#define CPUID_CACHE_LEVEL_MASK 0x7
#define CPUID_CACHE_LEVEL(res) CPUID_CACHE(LEVEL, (res).eax)

#define CPUID_CACHE_TYPE_SHIFT 0
#define CPUID_CACHE_TYPE_MASK 0x1f
#define CPUID_CACHE_TYPE(res) CPUID_CACHE(TYPE, (res).eax)

// Intel leaf 0x4, AMD leaf 0x8000001d EBX

#define CPUID_CACHE_WAYS_OF_ASSOC_SHIFT 22
#define CPUID_CACHE_WAYS_OF_ASSOC_MASK 0x3ff
#define CPUID_CACHE_WAYS_OF_ASSOC(res) CPUID_CACHE(WAYS_OF_ASSOC, (res).ebx)

#define CPUID_CACHE_PHYS_LINE_SHIFT 12
#define CPUID_CACHE_PHYS_LINE_MASK 0x3ff
#define CPUID_CACHE_PHYS_LINE(res) CPUID_CACHE(PHYS_LINE, (res).ebx)

#define CPUID_CACHE_COHER_LINE_SHIFT 0
#define CPUID_CACHE_COHER_LINE_MASK 0xfff
#define CPUID_CACHE_COHER_LINE(res) CPUID_CACHE(COHER_LINE, (res).ebx)

// Intel leaf 0x4, AMD leaf 0x8000001d ECX

#define CPUID_CACHE_NO_OF_SETS_SHIFT 0
#define CPUID_CACHE_NO_OF_SETS_MASK 0xffffffff
#define CPUID_CACHE_NO_OF_SETS(res) CPUID_CACHE(NO_OF_SETS, (res).ecx)

unsigned int cpu_cpuid_extended_level(void);
int cpu_have_cpuid(void);

static inline bool cpu_is_amd(void)
{
	return CONFIG(CPU_AMD_AGESA) || CONFIG(CPU_AMD_PI) || CONFIG(SOC_AMD_COMMON);
}

static inline bool cpu_is_intel(void)
{
	return CONFIG(CPU_INTEL_COMMON) || CONFIG(SOC_INTEL_COMMON);
}

struct device;

struct cpu_device_id {
	unsigned int vendor;
	unsigned int device;
};

struct cpu_driver {
	struct device_operations *ops;
	const struct cpu_device_id *id_table;
};

struct cpu_driver *find_cpu_driver(struct device *cpu);

struct thread;

struct cpu_info {
	struct device *cpu;
	size_t index;
};

/*
 * This structure describes the data allocated in the %gs segment for each CPU.
 * In order to read from this structure you will need to use assembly to
 * reference the segment.
 *
 * e.g., Reading the cpu_info pointer:
 *     %%gs:0
 */
struct per_cpu_segment_data {
	/*
	 * Instead of keeping a `struct cpu_info`, we actually keep a pointer
	 * pointing to the cpu_info struct located in %ds. This prevents
	 * needing specific access functions to read the fields in the cpu_info.
	 */
	struct cpu_info *cpu_info;
};

static inline struct cpu_info *cpu_info(void)
{
/* We use a #if because we don't want to mess with the &s below. */
#if CONFIG(CPU_INFO_V2)
	struct cpu_info *ci = NULL;

	__asm__("mov %%gs:%c[offset], %[ci]"
		: [ci] "=r" (ci)
		: [offset] "i" (offsetof(struct per_cpu_segment_data, cpu_info))
	);

	return ci;
#else
	char s;
	uintptr_t info = ALIGN_UP((uintptr_t)&s, CONFIG_STACK_SIZE) - sizeof(struct cpu_info);
	return (struct cpu_info *)info;
#endif /* CPU_INFO_V2 */
}

struct cpuinfo_x86 {
	uint8_t	x86;		/* CPU family */
	uint8_t	x86_vendor;	/* CPU vendor */
	uint8_t	x86_model;
	uint8_t	x86_mask;
};

static inline void get_fms(struct cpuinfo_x86 *c, uint32_t tfms)
{
	c->x86 = (tfms >> 8) & 0xf;
	c->x86_model = (tfms >> 4) & 0xf;
	c->x86_mask = tfms & 0xf;
	if (c->x86 == 0xf)
		c->x86 += (tfms >> 20) & 0xff;
	if (c->x86 >= 0x6)
		c->x86_model += ((tfms >> 16) & 0xF) << 4;

}

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static __always_inline void cpu_relax(void)
{
	__asm__ __volatile__("rep;nop" : : : "memory");
}

#define asmlinkage __attribute__((regparm(0)))

/*
 * The car_stage_entry() is the symbol jumped to for each stage
 * after bootblock using cache-as-ram.
 */
asmlinkage void car_stage_entry(void);

/*
 * Get processor id using cpuid eax=1
 * return value in EAX register
 */
uint32_t cpu_get_cpuid(void);

/*
 * Get processor feature flag using cpuid eax=1
 * return value in ECX register
 */
uint32_t cpu_get_feature_flags_ecx(void);

/*
 * Get processor feature flag using cpuid eax=1
 * return value in EDX register
 */
uint32_t cpu_get_feature_flags_edx(void);

/*
 * Previously cpu_index() implementation assumes that cpu_index()
 * function will always getting called from coreboot context
 * (ESP stack pointer will always refer to coreboot).
 *
 * But with MP_SERVICES_PPI implementation in coreboot this
 * assumption might not be true, where FSP context (stack pointer refers
 * to FSP) will request to get cpu_index().
 *
 * Hence new logic to use cpuid to fetch lapic id and matches with
 * cpus_default_apic_id[] variable to return correct cpu_index().
 */
int cpu_index(void);

#define DETERMINISTIC_CACHE_PARAMETERS_CPUID_IA	0x04
#define DETERMINISTIC_CACHE_PARAMETERS_CPUID_AMD	0x8000001d

enum cache_level {
	CACHE_L1D = 0,
	CACHE_L1I = 1,
	CACHE_L2 = 2,
	CACHE_L3 = 3,
	CACHE_LINV = 0xFF,
};

enum cpu_type {
	CPUID_COMMAND_UNSUPPORTED = 0,
	CPUID_TYPE_AMD = 1,
	CPUID_TYPE_INTEL = 2,
	CPUID_TYPE_INVALID = 0xFF,
};

struct cpu_cache_info {
	uint8_t type;
	uint8_t level;
	size_t num_ways;
	size_t num_sets;
	size_t line_size;
	size_t size;
	size_t physical_partitions;
	size_t num_cores_shared;
	bool fully_associative;
};

enum cpu_type cpu_check_deterministic_cache_cpuid_supported(void);

/* cpu_get_cache_assoc_info to get cache ways of associativity information. */
size_t cpu_get_cache_ways_assoc_info(const struct cpu_cache_info *info);

/*
 * cpu_get_cache_type to get cache type.
 * Cache type can be between 0: no cache, 1: data cache, 2: instruction cache
 * 3: unified cache and rests are reserved.
 */
uint8_t cpu_get_cache_type(const struct cpu_cache_info *info);

/*
 * cpu_get_cache_level to get cache level.
 * Cache level can be between 0: reserved, 1: L1, 2: L2, 3: L3 and rests are reserved.
 */
uint8_t cpu_get_cache_level(const struct cpu_cache_info *info);

/* cpu_get_cache_phy_partition_info to get cache physical partitions information. */
size_t cpu_get_cache_phy_partition_info(const struct cpu_cache_info *info);

/* cpu_get_cache_line_size to get cache line size in bytes. */
size_t cpu_get_cache_line_size(const struct cpu_cache_info *info);

/* cpu_get_cache_line_size to get cache number of sets information. */
size_t cpu_get_cache_sets(const struct cpu_cache_info *info);

/* cpu_is_cache_full_assoc checks if cache is fully associative. */
bool cpu_is_cache_full_assoc(const struct cpu_cache_info *info);

/* cpu_get_max_cache_share checks the number of cores are sharing this cache. */
size_t cpu_get_max_cache_share(const struct cpu_cache_info *info);

/* get_cache_size to calculate the cache size. */
size_t get_cache_size(const struct cpu_cache_info *info);

/*
 * fill_cpu_cache_info to get all required cache info data and fill into cpu_cache_info
 * structure by calling CPUID.EAX=leaf and ECX=Cache Level.
 */
bool fill_cpu_cache_info(uint8_t level, struct cpu_cache_info *info);

#endif /* ARCH_CPU_H */
