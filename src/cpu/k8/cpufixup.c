/* Needed so the AMD K8 runs correctly.  */
#include <console/console.h>
#include <mem.h>
#include <cpu/p6/msr.h>
#include <cpu/k8/mtrr.h>
#include <device/device.h>
#include <device/chip.h>

#include "chip.h"

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

#if 1
        /* Report the amount of memory. */
        print_debug("cpufixup RAM: 0x");
        print_debug_hex32(tomk);
        print_debug(" KB\r\n");
#endif

        /* Now set top of memory */
        msr.lo = (tomk & 0x003fffff) << 10;
        msr.hi = (tomk & 0xffc00000) >> 22;
        wrmsr(TOP_MEM2, msr);

        /* Leave a 64M hole between TOP_MEM and TOP_MEM2
         * so I can see my rom chip and other I/O devices.
         */
        if (tomk >= 0x003f0000) {
                tomk = 0x3f0000;
        } //    tom_k = 0x3c0000;
        msr.lo = (tomk & 0x003fffff) << 10;
        msr.hi = (tomk & 0xffc00000) >> 22;
        wrmsr(TOP_MEM, msr);


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

static
void k8_enable(struct chip *chip, enum chip_pass pass)
{

        struct cpu_k8_config *conf = (struct cpu_k8_config *)chip->chip_info;

        switch (pass) {
        case CONF_PASS_PRE_CONSOLE:
                break;
        default:
                /* nothing yet */
                break;
        }
}

struct chip_control cpu_k8_control = {
        enable: k8_enable,
        name:   "AMD K8"
};




