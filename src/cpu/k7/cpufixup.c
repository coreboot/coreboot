// Needed so the AMD K7 runs correctly. 
#include <printk.h>
#include <cpu/p6/msr.h>
#include <cpu/cpufixup.h>

#define TOP_MEM 0xc001001a
#define TOP_MEM2 0xc001001d
#define IORR_FIRST 0xC0010016
#define IORR_LAST 0xC0010019
#define SYSCFG    0xc0010010

#define MTRRVARDRAMEN (1 << 20)
void
k7_cpufixup(unsigned long ram_kilobytes)
{
	unsigned long lo = 0, hi = 0, i;
	unsigned long ram_megabytes = ram_kilobytes * 1024;
	// 8 MB alignment please
	ram_megabytes += 0x7fffff;
	ram_megabytes &= (~0x7fffff);
	// set top_mem registers to ram size 
	printk_spew("Setting top_mem to 0x%x\n", ram_megabytes);
	rdmsr(TOP_MEM, lo, hi);
	printk_spew("TOPMEM was 0x%02x:0x%02x\n", hi, lo);
	hi = 0;
	lo = ram_megabytes;
	wrmsr(TOP_MEM, lo, hi);
	// I am setting this even though I won't enable it
	wrmsr(TOP_MEM2, lo, hi);
	
	/* zero the IORR's before we enable to prevent
	 * undefined side effects
	 */
	lo = hi = 0;
	for(i = IORR_FIRST; i <= IORR_LAST; i++)
		wrmsr(i, lo, hi);
	
	rdmsr(SYSCFG, lo, hi);
	printk_spew("SYSCFG was 0x%x:0x%x\n", hi, lo);
	lo |= MTRRVARDRAMEN;
	wrmsr(SYSCFG, lo, hi);
	rdmsr(SYSCFG, lo, hi);
	printk_spew("SYSCFG IS NOW 0x%x:0x%x\n", hi, lo);
}
