#include <cpu/p6/mtrr.h>
#include <cpu/p6/msr.h>

static inline unsigned long read_cr0(void)
{
    unsigned long cr0;
    asm volatile ("movl %%cr0, %0" : "=r" (cr0));
    return cr0;
}

static inline void write_cr0(unsigned long cr0)
{
    asm volatile ("movl %0, %%cr0" : : "r" (cr0));
}

/* the fixed and variable MTTRs are power-up with random values,
 * clear them to MTRR_TYPE_UNCACHEABLE for safty.
 */

static void early_mtrr_init(void)
{
	static const unsigned long mtrr_msrs[] = {
		/* fixed mtrr */
		0x250, 0x258, 0x259,
		0x268, 0x269, 0x26A,
		0x26B, 0x26C, 0x26D,
		0x26E, 0x26F,
		/* var mtrr */
		0x200, 0x201, 0x202, 0x203,
		0x204, 0x205, 0x206, 0x207,
		0x208, 0x209, 0x20A, 0x20B,
		0x20C, 0x20D, 0x20E, 0x20F,
		/* NULL end of table */
		0
	};
	msr_t msr;
	const unsigned long *msr_addr;
	unsigned long cr0;

	print_debug("Disabling cache\r\n");
	/* Just to be sure, take all the steps to disable the cache.
	 * This may not be needed, but C3's may...
	 * Invalidate the cache */
	asm volatile ("invd");

	/* Disable the cache */
	cr0 = read_cr0();
	cr0 |= 0x40000000;
	write_cr0(cr0);

	/* Disable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000000;
	wrmsr(MTRRdefType_MSR, msr);

	/* Invalidate the cache again */
	asm volatile ("invd");

	print_debug("Clearing mtrr\r\n");

	/* Inialize all of the relevant msrs to 0 */
	msr.lo = 0;
	msr.hi = 0;
	for(msr_addr = mtrr_msrs; *msr_addr; msr_addr++) {
		wrmsr(*msr_addr, msr);
	}

	/* Enable caching for 0 - 1MB using variable mtrr */
	msr = rdmsr(0x200);
	msr.hi &= 0xfffffff0;
	msr.hi |= 0x00000000;
	msr.lo &= 0x00000f00;
	msr.lo |= 0x00000000 | MTRR_TYPE_WRBACK;
	wrmsr(0x200, msr);

	msr = rdmsr(0x201);
	msr.hi &= 0xfffffff0;
	msr.hi |= 0x0000000f;
	msr.lo &= 0x000007ff;
	msr.lo |= (~((CONFIG_LB_MEM_TOPK << 10) - 1)) | 0x800;
	wrmsr(0x201, msr);

#if defined(XIP_ROM_SIZE) && defined(XIP_ROM_BASE)
	print_debug("Setting XIP\r\n");
	/* enable write through caching so we can do execute in place
	 * on the flash rom.
	 */
	msr.hi = 0x00000000;
	msr.lo = XIP_ROM_BASE | MTRR_TYPE_WRTHROUGH;
	wrmsr(0x202, msr);
	msr.hi = 0x0000000f;
	msr.lo = ~(XIP_ROM_SIZE - 1) | 0x800;
	wrmsr(0x203, msr);
#endif

	/* Set the default memory type and enable fixed and variable MTRRs 
	 */
	/* Enable Variable MTRRs */
	msr.hi = 0x00000000;
	msr.lo = 0x00000800;
	wrmsr(MTRRdefType_MSR, msr);

	/* Enable the cache */
	cr0 = read_cr0();
	cr0 &= 0x9fffffff;
	write_cr0(cr0);
	print_debug("Enabled the cache\r\n");
}
