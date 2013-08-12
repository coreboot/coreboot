#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>

#if CONFIG_SMP
int boot_cpu(void)
{
	int bsp;
	msr_t msr;
	msr = rdmsr(0x1b);
	bsp = !!(msr.lo & (1 << 8));
	return bsp;
}
#endif

