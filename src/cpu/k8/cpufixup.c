/* Needed so the AMD K8 runs correctly.  */
#include <console/console.h>
#include <mem.h>
#include <cpu/p6/msr.h>
#include <cpu/k8/mtrr.h>
#include <device/device.h>
#include "../../northbridge/amd/amdk8/cpu_rev.c"
#include <device/chip.h>
#include "chip.h"

#define MCI_STATUS 0x401

static inline void disable_cache(void)
{
	unsigned int tmp;
	/* Disable cache */
	/* Write back the cache and flush TLB */
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"orl  $0x40000000, %0\n\t"
		"wbinvd\n\t"
		"movl  %0, %%cr0\n\t"
		"wbinvd\n\t"
		:"=r" (tmp)
		::"memory");
}

static inline void enable_cache(void)
{
	unsigned int tmp;
	// turn cache back on. 
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"andl  $0x9fffffff, %0\n\t"
		"movl  %0, %%cr0\n\t"
		:"=r" (tmp)
		::"memory");
}

static inline msr_t rdmsr_amd(unsigned index)
{
        msr_t result;
        __asm__ __volatile__ (
                "rdmsr"
                : "=a" (result.lo), "=d" (result.hi)
                : "c" (index), "D" (0x9c5a203a)
                );
        return result;
}

static inline void wrmsr_amd(unsigned index, msr_t msr)
{
        __asm__ __volatile__ (
                "wrmsr"
                : /* No outputs */
                : "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (0x9c5a203a)
                );
}

void k8_cpufixup(struct mem_range *mem)
{
	unsigned long mmio_basek, tomk;
	unsigned long i;
	msr_t msr;

	disable_cache();
	
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

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for(i=0; i<5; i++) {
		wrmsr(MCI_STATUS + (i*4),msr);
	}
	
	if (is_cpu_pre_c0()) {
		/* Erratum 63... */
		msr = rdmsr(HWCR_MSR);
		msr.lo |= (1 << 6);
		wrmsr(HWCR_MSR, msr);
		/* Erratum 69... */
#if 1
		msr = rdmsr_amd(BU_CFG_MSR);
		msr.hi |= (1 << (45 - 32));
		wrmsr_amd(BU_CFG_MSR, msr);
#endif
		/* Erratum 81... */
#if 1
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |=  (1 << 10);
		wrmsr_amd(DC_CFG_MSR, msr);
#endif
			
	}
	/* Erratum 89 ... */
	msr = rdmsr(NB_CFG_MSR);
	msr.lo |= 1 << 3;

	if (!is_cpu_pre_c0()) {
		/* Erratum 86 Disable data masking on C0 and 
		 * later processor revs.
		 * FIXME this is only needed if ECC is enabled.
		 */
		msr.hi |= 1 << (36 - 32);
	}	
	wrmsr(NB_CFG_MSR, msr);
#if 1  /* The following erratum fixes reset the cpu ???? */
	
	/* Erratum 97 ... */
	if (!is_cpu_pre_c0()) {
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |= 1 << 3;
		wrmsr_amd(DC_CFG_MSR, msr);
	}	
	
	/* Erratum 94 ... */
	msr = rdmsr_amd(IC_CFG_MSR);
	msr.lo |= 1 << 11;
	wrmsr_amd(IC_CFG_MSR, msr);

#endif

	/* Erratum 91 prefetch miss is handled in the kernel */
	
	enable_cache();
}

static
void k8_enable(struct chip *chip, enum chip_pass pass)
{

        struct cpu_k8_config *conf = (struct cpu_k8_config *)chip->chip_info;

        switch (pass) {
        case CONF_PASS_PRE_CONSOLE:
                break;
	case CONF_PASS_PRE_PCI:
		init_timer();
		break;
        default:
                /* nothing yet */
                break;
        }
}

struct chip_control cpu_k8_control = {
        .enable = k8_enable,
        .name   = "AMD K8",
};
