/* Needed so the AMD K8 runs correctly.  */
#include <console/console.h>
#include <mem.h>
#include <cpu/p6/msr.h>

#define TOP_MEM    0xc001001A
#define TOP_MEM2   0xc001001D
#define IORR_FIRST 0xC0010016
#define IORR_LAST  0xC0010019
#define SYSCFG     0xC0010010

#define MTRRVARDRAMEN (1 << 20)

void k8_cpufixup(struct mem_range *mem)
{
	msr_t msr;
	unsigned long i;
	unsigned long ram_megabytes;

	/* For now no Athlon board has significant holes in it's
	 * address space so just find the last memory region
	 * and compute the end of memory from that.
	 */
	for(i = 0; mem[i].sizek; i++)
		;
	if (i == 0) 
		return;
	ram_megabytes = (mem[i-1].basek + mem[i-1].sizek) *1024;
		

#warning "FIXME handle > 4GB of ram"
	// 8 MB alignment please
	ram_megabytes += 0x7fffff;
	ram_megabytes &= (~0x7fffff);

	// set top_mem registers to ram size
	printk_spew("Setting top_mem to 0x%x\n", ram_megabytes);
	msr = rdmsr(TOP_MEM);
	printk_spew("TOPMEM was 0x%02x:0x%02x\n", msr.hi, msr.lo);
	msr.hi = 0;
	msr.lo = ram_megabytes;
	wrmsr(TOP_MEM, msr);

	// I am setting this even though I won't enable it
	wrmsr(TOP_MEM2, msr);

	/* zero the IORR's before we enable to prevent
	 * undefined side effects
	 */
	msr.lo = msr.hi = 0;
	for (i = IORR_FIRST; i <= IORR_LAST; i++)
		wrmsr(i, msr);

	msr = rdmsr(SYSCFG);
	printk_spew("SYSCFG was 0x%x:0x%x\n", msr.hi, msr.lo);
	msr.lo |= MTRRVARDRAMEN;
	wrmsr(SYSCFG, msr);
	msr = rdmsr(SYSCFG);
	printk_spew("SYSCFG IS NOW 0x%x:0x%x\n", msr.hi, msr.lo);
}

