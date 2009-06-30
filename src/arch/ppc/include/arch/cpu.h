#ifndef ARCH_CPU_H
#define ARCH_CPU_H
/*
 * this should probably integrate code from src/arch/ppc/lib/cpuid.c
 */ 

struct cpu_device_id {
	unsigned pvr;
};

struct cpu_driver {
	struct device_operations *ops;
	struct cpu_device_id *id_table;
};

#ifndef CONFIG_STACK_SIZE
#error CONFIG_STACK_SIZE not defined
#endif

/* The basic logic comes from the Linux kernel.
 * The invariant is that (1 << 31 - STACK_BITS) == CONFIG_STACK_SIZE
 * I wish there was simpler way to support multiple stack sizes.
 * Oh well.
 */
#if CONFIG_STACK_SIZE == 4096
#define STACK_BITS "19"
#elif CONFIG_STACK_SIZE == 8192
#define STACK_BITS "18"
#elif CONFIG_STACK_SIZE == 16384
#define STACK_BITS "17"
#elif CONFIG_STACK_SIZE == 32768
#define STACK_BITS "16"
#elif CONFIG_STACK_SIZE == 65536
#define STACK_BITS "15"
#else
#error Unimplemented stack size
#endif


struct cpu_info {
	struct device *cpu;
	unsigned long index;
};


static inline struct cpu_info *cpu_info(void)
{
	struct cpu_info *ci;
	__asm__("rlwinm %0,1,0,0," STACK_BITS : "=r"(ci));
	return ci;
}

static inline unsigned long cpu_index(void)
{
	struct cpu_info *ci;
	ci = cpu_info();
	return ci->index;
}

#endif /* ARCH_CPU_H */
