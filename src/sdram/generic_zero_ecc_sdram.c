#include <sdram.h>
#include <arch/cache_ram.h>
#include <printk.h>
#include <bitops.h>
#include <cpu/p6/mtrr.h>
#include <string.h>

#if 0
void sdram_initialize_ecc(void)
{
	unsigned long size;
	void *start_addr;
	printk_debug("ecc_ram_1\n");
	size = sdram_get_ecc_size_bytes();

	/* If we don't have an ECC SDRAM size skip the zeroing */
	if (size > 0) {
		/* Compute the next greater power of two memory size,
		 * to use in the mtrrs */
		unsigned long bits;
		unsigned long mtrr_size;
		bits = log2(size);
		/* See if I need to round up */
		if (size & ((1 << bits) -1)) {
			bits++;
		}
		mtrr_size = (1 << bits);

		/* Set caching on all of memory into write-combining mode. 
		 * So we can zero it quickly.
		 */
		set_var_mtrr(0, 0, mtrr_size, MTRR_TYPE_WRCOMB);

		/* Now zero the memory */
		printk_debug("ecc_ram_2\n");
#if defined(HAVE_PC80_MEMORY_HOLE) || 1
		printk_debug("zeroing 0 - 640K\n");
		start_addr = RAM_ADDR(0);
		memset(start_addr, 0, 640*1024);
		printk_debug("zeroing 1M - %dM\n", size/(1024*1024));
		start_addr = RAM_ADDR(1024*1024);
		memset(start_addr, 0, size - (1024*1024));
#else
		printk_debug("zeroing 0 - 0x%08x\n", size);
		start_addr = RAM_ADDR(0);
		memset(start_addr, 0, size);
#endif
		printk_debug("ecc_ram_3\n");
		/* Fully disable caching on ram again. */
		set_var_mtrr(0, 0, 0, MTRR_TYPE_UNCACHABLE);
	}
	printk_debug("ecc_ram_4\n");
	
}
#else
static void ram_clear(unsigned long start, unsigned long stop)
{
#if 0
	unsigned long addr;
	/* 
	 * Fill.
	 */
	printk_debug("DRAM clear: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr);
		RAM(unsigned long, addr) = 0;
	};
	/* Display final address */
	printk_debug("%08lx\nDRAM cleared\n", addr);
#else
	printk_debug("DRAM clear: %08lx-%08lx\n", start, stop);
	__asm__ volatile(
		"cld\n\t"
		"rep stosl\n\t"
		:
		: "a" (0), "D" (RAM_ADDR(start)), "c" ((stop - start)/4)
		);
#endif
}

void sdram_initialize_ecc(void)
{
	unsigned long size;
	printk_debug("stupid_ecc_ram_1\n");
	size = sdram_get_ecc_size_bytes();

	/* If we don't have an ECC SDRAM size skip the zeroing */
	if (size > 0) {
		printk_debug("stupid_ecc_ram_2\n");
		set_var_mtrr(0, 0, size, MTRR_TYPE_WRCOMB);
		printk_debug("stupid_ecc_ram_3\n");
		ram_clear(0, 640*1024);
		ram_clear(1024*1024, size);
	};
	printk_debug("stupid_ecc_ram_4\n");
	
}

#endif
