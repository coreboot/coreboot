#include <console/console.h>
#include <device/device.h>
#include <arch/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>

#if CONFIG_GFXUMA == 1
extern uint64_t uma_memory_size;
#endif

static unsigned long resk(uint64_t value)
{
	unsigned long resultk;
	if (value < (1ULL << 42)) {
		resultk = value >> 10;
	}
	else {
		resultk = 0xffffffff;
	}
	return resultk;
}

static unsigned fixed_mtrr_index(unsigned long addrk)
{
	unsigned index;
	index = (addrk - 0) >> 6;
	if (index >= 8) {
		index = ((addrk - 8*64) >> 4) + 8;
	}
	if (index >= 24) {
		index = ((addrk - (8*64 + 16*16)) >> 2) + 24;
	}
	if (index > NUM_FIXED_RANGES) {
		index = NUM_FIXED_RANGES;
	}
	return index;
}

static unsigned int mtrr_msr[] = {
	MTRRfix64K_00000_MSR, MTRRfix16K_80000_MSR, MTRRfix16K_A0000_MSR,
	MTRRfix4K_C0000_MSR, MTRRfix4K_C8000_MSR, MTRRfix4K_D0000_MSR, MTRRfix4K_D8000_MSR,
	MTRRfix4K_E0000_MSR, MTRRfix4K_E8000_MSR, MTRRfix4K_F0000_MSR, MTRRfix4K_F8000_MSR,
};

static void set_fixed_mtrrs(unsigned int first, unsigned int last, unsigned char type)
{
	unsigned int i;
	unsigned int fixed_msr = NUM_FIXED_RANGES >> 3;
	msr_t msr;
	msr.lo = msr.hi = 0; /* Shut up gcc */
	for (i = first; i < last; i++) {
		/* When I switch to a new msr read it in */
		if (fixed_msr != i >> 3) {
			/* But first write out the old msr */
			if (fixed_msr < (NUM_FIXED_RANGES >> 3)) {
				disable_cache();
				wrmsr(mtrr_msr[fixed_msr], msr);
				enable_cache();
			}
			fixed_msr = i>>3;
			msr = rdmsr(mtrr_msr[fixed_msr]);
		}
		if ((i & 7) < 4) {
			msr.lo &= ~(0xff << ((i&3)*8));
			msr.lo |= type << ((i&3)*8);
		} else {
			msr.hi &= ~(0xff << ((i&3)*8));
			msr.hi |= type << ((i&3)*8);
		}
	}
	/* Write out the final msr */
	if (fixed_msr < (NUM_FIXED_RANGES >> 3)) {
		disable_cache();
		wrmsr(mtrr_msr[fixed_msr], msr);
		enable_cache();
	}
}

struct mem_state {
	unsigned long mmio_basek, tomk;
};
static void set_fixed_mtrr_resource(void *gp, struct device *dev, struct resource *res)
{
	struct mem_state *state = gp;
	unsigned long topk;
	unsigned int start_mtrr;
	unsigned int last_mtrr;

	topk = resk(res->base + res->size);
	if (state->tomk < topk) {
		state->tomk = topk;
	}
	if ((topk < 4*1024*1024) && (state->mmio_basek < topk)) {
		state->mmio_basek = topk;
	}
	start_mtrr = fixed_mtrr_index(resk(res->base));
	last_mtrr  = fixed_mtrr_index(resk((res->base + res->size)));
	if (start_mtrr >= NUM_FIXED_RANGES) {
		return;
	}
	printk(BIOS_DEBUG, "Setting fixed MTRRs(%d-%d) Type: WB, RdMEM, WrMEM\n",
		start_mtrr, last_mtrr);
	set_fixed_mtrrs(start_mtrr, last_mtrr, MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM);

}

void amd_setup_mtrrs(void)
{
	unsigned long address_bits;
	struct mem_state state;
	unsigned long i;
	msr_t msr, sys_cfg;


	/* Enable the access to AMD RdDram and WrDram extension bits */
	disable_cache();
	sys_cfg = rdmsr(SYSCFG_MSR);
	sys_cfg.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, sys_cfg);
	enable_cache();

	printk(BIOS_DEBUG, "\n");
	/* Initialized the fixed_mtrrs to uncached */
	printk(BIOS_DEBUG, "Setting fixed MTRRs(%d-%d) type: UC\n",
		0, NUM_FIXED_RANGES);
	set_fixed_mtrrs(0, NUM_FIXED_RANGES, MTRR_TYPE_UNCACHEABLE);

	/* Except for the PCI MMIO hole just before 4GB there are no
	 * significant holes in the address space, so just account
	 * for those two and move on.
	 */
	state.mmio_basek = state.tomk = 0;
	search_global_resources(
		IORESOURCE_MEM | IORESOURCE_CACHEABLE, IORESOURCE_MEM | IORESOURCE_CACHEABLE,
		set_fixed_mtrr_resource, &state);
	printk(BIOS_DEBUG, "DONE fixed MTRRs\n");

	if (state.mmio_basek > state.tomk) {
		state.mmio_basek = state.tomk;
	}
	/* Round state.mmio_basek down to the nearst size that will fit in TOP_MEM */
	state.mmio_basek = state.mmio_basek & ~TOP_MEM_MASK_KB;
	/* Round state.tomk up to the next greater size that will fit in TOP_MEM */
	state.tomk = (state.tomk + TOP_MEM_MASK_KB) & ~TOP_MEM_MASK_KB;

	disable_cache();

	/* Setup TOP_MEM */
	msr.hi = state.mmio_basek >> 22;
	msr.lo = state.mmio_basek << 10;

	/* If UMA graphics is enabled, the frame buffer memory
	 * has been deducted from the size of memory below 4GB.
	 * When setting TOM, include UMA DRAM
	 */
	#if CONFIG_GFXUMA == 1
	msr.lo += uma_memory_size;
	#endif
	wrmsr(TOP_MEM, msr);

	sys_cfg.lo &= ~(SYSCFG_MSR_TOM2En | SYSCFG_MSR_TOM2WB);
	if(state.tomk > (4*1024*1024)) {
		/* DRAM above 4GB: set TOM2, SYSCFG_MSR_TOM2En
		 * and SYSCFG_MSR_TOM2WB
		 */
		msr.hi = state.tomk >> 22;
		msr.lo = state.tomk << 10;
		wrmsr(TOP_MEM2, msr);
		sys_cfg.lo |= SYSCFG_MSR_TOM2En | SYSCFG_MSR_TOM2WB;
	}

	/* zero the IORR's before we enable to prevent
	 * undefined side effects.
	 */
	msr.lo = msr.hi = 0;
	for(i = IORR_FIRST; i <= IORR_LAST; i++) {
		wrmsr(i, msr);
	}

	/* Enable Variable Mtrrs
	 * Enable the RdMem and WrMem bits in the fixed mtrrs.
	 * Disable access to the RdMem and WrMem in the fixed mtrr.
	 */
	sys_cfg.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn;
	sys_cfg.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, sys_cfg);

	enable_fixed_mtrr();

	enable_cache();

	address_bits = CONFIG_CPU_ADDR_BITS; //K8 could be 40, and GH could be 48

	/* AMD specific cpuid function to query number of address bits */
	if (cpuid_eax(0x80000000) >= 0x80000008) {
		address_bits = cpuid_eax(0x80000008) & 0xff;
	}

	/* Now that I have mapped what is memory and what is not
	 * Setup the mtrrs so we can cache the memory.
	 */
	x86_setup_var_mtrrs(address_bits, 0);
}
