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

void amd_setup_mtrrs(void)
{
	unsigned long mmio_basek, tomk;
	unsigned long i;
	device_t dev;
	msr_t msr;

	x86_setup_mtrrs();

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
		}
	}
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

	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_TOM2En;
	wrmsr(SYSCFG_MSR, msr);

	enable_cache();
}
