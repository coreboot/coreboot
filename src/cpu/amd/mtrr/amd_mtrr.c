#include <console/console.h>
#include <device/device.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>

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

#if 1
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

#endif

void amd_setup_mtrrs(void)
{
	unsigned long mmio_basek, tomk;
	unsigned long i;
	device_t dev;
	msr_t msr;

	/* Enable the access to AMD RdDram and WrDram extension bits */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);

	printk_debug("\n");
	/* Initialized the fixed_mtrrs to uncached */
	printk_debug("Setting fixed MTRRs(%d-%d) type: UC\n", 
		0, NUM_FIXED_RANGES);
	set_fixed_mtrrs(0, NUM_FIXED_RANGES, MTRR_TYPE_UNCACHEABLE);

	/* Except for the PCI MMIO hole just before 4GB there are no
	 * significant holes in the address space, so just account
	 * for those two and move on.
	 */
	mmio_basek = tomk = 0;
	for(dev = all_devices; dev; dev = dev->next) {
		struct resource *res, *last;
		last = &dev->resource[dev->resources];
		for(res = &dev->resource[0]; res < last; res++) {
			unsigned long topk;
			unsigned long start_mtrr, last_mtrr;
			if (!(res->flags & IORESOURCE_MEM) ||
				(!(res->flags & IORESOURCE_CACHEABLE))) {
				continue;
			}
			topk = resk(res->base + res->size);
			if (tomk < topk) {
				tomk = topk;
			}
			if ((topk < 4*1024*1024) && (mmio_basek < topk)) {
				mmio_basek = topk;
			}

			start_mtrr = fixed_mtrr_index(resk(res->base));
			last_mtrr  = fixed_mtrr_index(resk(res->base + res->size));
			if (start_mtrr >= NUM_FIXED_RANGES) {
				continue;
			}
			printk_debug("Setting fixed MTRRs(%d-%d) Type: WB\n",
				start_mtrr, last_mtrr);
			set_fixed_mtrrs(start_mtrr, last_mtrr, MTRR_TYPE_WRBACK | MTRR_READ_MEM | MTRR_WRITE_MEM);
		}
	}
	printk_debug("DONE fixed MTRRs\n");
	if (mmio_basek > tomk) {
		mmio_basek = tomk;
	}
	/* Round mmio_basek down to the nearst size that will fit in TOP_MEM */
	mmio_basek = mmio_basek & ~TOP_MEM_MASK_KB;
	/* Round tomk up to the next greater size that will fit in TOP_MEM */
	tomk = (tomk + TOP_MEM_MASK_KB) & ~TOP_MEM_MASK_KB;

	disable_cache();

	/* Setup TOP_MEM */
	msr.hi = mmio_basek >> 22;
	msr.lo = mmio_basek << 10;
	wrmsr(TOP_MEM, msr);

	/* Setup TOP_MEM2 */
	msr.hi = tomk >> 22;
	msr.lo = tomk << 10;
	wrmsr(TOP_MEM2, msr);

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
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_MtrrFixDramEn | SYSCFG_MSR_TOM2En;
	msr.lo &= ~SYSCFG_MSR_MtrrFixDramModEn;
	wrmsr(SYSCFG_MSR, msr);

	enable_cache();

	/* Now that I have mapped what is memory and what is not
	 * Setup the mtrrs so we can cache the memory.
	 */
	x86_setup_mtrrs();
}
