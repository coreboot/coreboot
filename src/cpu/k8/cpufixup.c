/* Needed so the AMD K8 runs correctly.  */
#include <console/console.h>
#include <mem.h>
#include <cpu/p6/msr.h>
#include <cpu/k8/mtrr.h>
#include <device/device.h>

void k8_cpufixup(struct mem_range *mem)
{
	unsigned long mmio_basek, tomk;
	unsigned long i;
	msr_t msr;
	/* Except for the PCI MMIO hold just before 4GB there are no
	 * significant holes in the address space, so just account
	 * for those two and move on.
	 */
	mmio_basek = tomk = 0;
	for(i = 0; mem[i].sizek; i++) {
		unsigned long topk;
		topk = mem[i].basek + mem[i].sizek;
		if (tomk < topk) {
			tomk = topk;
		}
		if ((topk < 4*1024*1024) && (mmio_basek < topk)) {
			mmio_basek = topk;
		}
	}
	if (mmio_basek > tomk) {
		mmio_basek = tomk;
	}

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
	
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_TOM2En;
	wrmsr(SYSCFG_MSR, msr);
}
