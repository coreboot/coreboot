#include <cpu/k8/mtrr.h>

/* the fixed and variable MTTRs are power-up with random values,
 * clear them to MTRR_TYPE_UNCACHEABLE for safty.
 */

static void early_mtrr_init(void)
{
	static const unsigned long mtrr_msrs[] = {
		/* fixed mtrr */
		0x250, 0x258, 0x259,
		0x268, 0x269, 0x26A
		0x26B, 0x26C, 0x26D
		0x26E, 0x26F,
		/* var mtrr */
		0x200, 0x201, 0x202, 0x203,
		0x204, 0x205, 0x206, 0x207,
		0x208, 0x209, 0x20A, 0x20B,
		0x20C, 0x20D, 0x20E, 0x20F,
		/* var iorr msr */
		0xC0010016, 0xC0010017, 0xC0010018, 0xC0010019,
		/* mem top */
		0xC001001A, 0xC001001D,
		/* NULL end of table */
		0
	};
	msr_t msr;
	const unsigned long *msr_addr;

	/* Inialize all of the relevant msrs to 0 */
	msr.lo = 0;
	msr.hi = 0;
	for(msr_addr = mtrr_msrs; *msr_addr; msr_addr++) {
		wrmsr(*msr_addr, msr);
	}

	/* Enable memory access for 0 - 1MB using top_mem */
	msr.hi = 0;
	msr.lo = ((CONFIG_LB_MEM_TOPK << 10) + TOP_MEM_MASK) & ~TOP_MEM_MASK;
	wrmsr(TOP_MEM, msr);

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
	/* enable write through caching so we can do execute in place
	 * on the flash rom.
	 */
	msr.hi = 0x00000000;
	msr.lo = XIP_ROM_BASE | MTRR_TYPE_WRTHROUGH;
	wrmsr(0x202, msr);
#error "FIXME verify the type of MTRR I have setup"
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
	
	/* Enale the MTRRs in SYSCFG */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrrVarDramEn;
	wrmsr(SYSCFG_MSR, msr);

	/* Enable the cache */
	unsigned long cr0;
	cr0 = read_cr0();
	cr0 &= 0x9fffffff;
	write_cr0(cr0);
}
