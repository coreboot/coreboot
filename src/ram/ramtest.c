#include <arch/cache_ram.h>
#include <ramtest.h>
#include <printk.h>

void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/* 
	 * Fill.
	 */
	printk_debug("DRAM fill: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr);
		RAM(unsigned long, addr) = addr;
	};
	/* Display final address */
	printk_debug("%08lx\nDRAM filled\n", addr);

	
}

int ram_verify(unsigned long start, unsigned long stop, int max_errors)
{
	unsigned long addr;
	int errors = 0;
	int correct = 0;
	/* 
	 * Verify.
	 */
	printk_debug("DRAM verify: %08lx-%08lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if ((addr & 0xffff) == 0)
			printk_debug("%08lx\r", addr); 
		value = RAM(unsigned long, addr);
		if (value != addr) {
			if (++errors <= max_errors) {
				/* Display address with error */
				printk_err("%08lx:%08lx\n", addr, value);
			}
		}
		else {
			if (++correct <= max_errors) {
				/* Display address without error */
				printk_err("%08lx:%08lx\n", addr, value);
			}
		}
	}
	/* Display final address */
	printk_debug("%08lx\nDRAM verified %d/%d errors %d/%d correct\n", 
		addr, errors, (stop - start)/4, correct, (stop - start)/4);
	return errors;
}


int ramcheck(unsigned long start, unsigned long stop, int max_errors)
{
	int result;
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk_debug("Testing DRAM : %08lx-%08lx\n",
		start, stop);

	ram_fill(start, stop);
	result = ram_verify(start, stop, max_errors);
	printk_debug("Done.\n");
	return result;
}

