#include <stdint.h>
#include <lib.h> /* Prototypes */
#include <console/console.h>

static void write_phys(unsigned long addr, u32 value)
{
	// Assembler in lib/ is very ugly. But we properly guarded
	// it so let's obey this one for now
#if CONFIG_SSE2
	asm volatile(
		"movnti %1, (%0)"
		: /* outputs */
		: "r" (addr), "r" (value) /* inputs */
#ifndef __GNUC__ /* GCC does not like empty clobbers? */
		: /* clobbers */
#endif
		);
#else
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	*ptr = value;
#endif
}

static u32 read_phys(unsigned long addr)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	return *ptr;
}

static void phys_memory_barrier(void)
{
#if CONFIG_SSE2
	// Needed for movnti
	asm volatile (
		"sfence"
		::
#ifdef __GNUC__ /* ROMCC does not like memory clobbers */
		: "memory"
#endif
	);
#else
#ifdef __GNUC__ /* ROMCC does not like empty asm statements */
	asm volatile ("" ::: "memory");
#endif
#endif
}

/**
 * Simple rotate ones test pattern for 128bit wide memory bus.
 * See console debug output for any dislocated bytes.
 *
 *  for (i=0; i<8; i++)
 *    for (j=0; j<16; j++)
 *      *(start + i<<4 + j) = 1<<i;
 *
 * @start   System memory offset, aligned to 128bytes
 */
static void ram_bitset(unsigned long start)
{
	unsigned long value, value2;
	unsigned long addr;
	uint8_t i;
	uint8_t verbose = 0;
	/*
	 * Fill.
	 */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "DRAM bitset write: 0x%08lx\n", start);
#else
	print_debug("DRAM bitset write: 0x");
	print_debug_hex32(start);
	print_debug("\n");
#endif
	value  = 0x01010101;
	for(addr = start; addr < (start + 0x80) ; addr += 4) {

		if ((addr & 0xf0) && (! (addr & 0x0f)))
			value <<= 1;
		write_phys(addr, value);
	};
	phys_memory_barrier();
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "DRAM bitset verify: 0x%08lx\n", start);
#else
	print_debug("DRAM bitset verify: 0x");
	print_debug_hex32(start);
	print_debug("\n");
#endif
	value  = 0x01010101;
	for(addr = start; addr < (start + 0x80) ; addr += 4) {

		if ((addr & 0xf0) && (! (addr & 0x0f)))
			value <<= 1;

		value2 = read_phys(addr);
		i = (value2 != value);

		if  ( i || verbose ) {
#if CONFIG_CACHE_AS_RAM
			printk(BIOS_ERR, "0x%08lx wr: 0x%08lx rd: 0x%08lx\n",
					 addr, value, value2);
#else
			print_err_hex32(addr);
			print_err(" wr: 0x");
			print_err_hex32(value);
			print_err(" rd: 0x");
			print_err_hex32(value2);
#endif
		}
		if (i)
			print_err(" FAIL\n");
		else if (verbose)
			print_err("\n");
	};

}


static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/*
	 * Fill.
	 */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "DRAM fill: 0x%08lx-0x%08lx\n", start, stop);
#else
	print_debug("DRAM fill: ");
	print_debug_hex32(start);
	print_debug("-");
	print_debug_hex32(stop);
	print_debug("\n");
#endif
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if (!(addr & 0xfffff)) {
#if CONFIG_CACHE_AS_RAM
			printk(BIOS_DEBUG, "%08lx \r", addr);
#else
			print_debug_hex32(addr);
			print_debug(" \r");
#endif
		}
		write_phys(addr, (u32)addr);
	};
	/* Display final address */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "%08lx\nDRAM filled\n", addr);
#else
	print_debug_hex32(addr);
	print_debug("\nDRAM filled\n");
#endif
}

static void ram_verify(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	int i = 0;
	/*
	 * Verify.
	 */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "DRAM verify: 0x%08lx-0x%08lx\n", start, stop);
#else
	print_debug("DRAM verify: ");
	print_debug_hex32(start);
	print_debug_char('-');
	print_debug_hex32(stop);
	print_debug("\n");
#endif
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if (!(addr & 0xfffff)) {
#if CONFIG_CACHE_AS_RAM
			printk(BIOS_DEBUG, "%08lx \r", addr);
#else
			print_debug_hex32(addr);
			print_debug(" \r");
#endif
		}
		value = read_phys(addr);
		if (value != addr) {
			/* Display address with error */
#if CONFIG_CACHE_AS_RAM
			printk(BIOS_ERR, "Fail: @0x%08lx Read value=0x%08lx\n", addr, value);
#else
			print_err("Fail: @0x");
			print_err_hex32(addr);
			print_err(" Read value=0x");
			print_err_hex32(value);
			print_err("\n");
#endif
			i++;
			if(i>256) {
#if CONFIG_CACHE_AS_RAM
				printk(BIOS_DEBUG, "Aborting.\n");
#else
				print_debug("Aborting.\n");
#endif
				break;
			}
		}
	}
	/* Display final address */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "%08lx", addr);
#else
	print_debug_hex32(addr);
#endif

	if (i) {
#if CONFIG_CACHE_AS_RAM
		printk(BIOS_DEBUG, "\nDRAM did _NOT_ verify!\n");
#else
		print_debug("\nDRAM did _NOT_ verify!\n");
#endif
		die("DRAM ERROR");
	}
	else {
#if CONFIG_CACHE_AS_RAM
		printk(BIOS_DEBUG, "\nDRAM range verified.\n");
#else
		print_debug("\nDRAM range verified.\n");
#endif
	}
}


void ram_check(unsigned long start, unsigned long stop)
{
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "Testing DRAM : %08lx - %08lx\n", start, stop);
#else
	print_debug("Testing DRAM : ");
	print_debug_hex32(start);
	print_debug("-");
	print_debug_hex32(stop);
	print_debug("\n");
#endif
	ram_bitset(start);
	ram_fill(start, stop);
	/* Make sure we don't read before we wrote */
	phys_memory_barrier();
	ram_verify(start, stop);
#if CONFIG_CACHE_AS_RAM
	printk(BIOS_DEBUG, "Done.\n");
#else
	print_debug("Done.\n");
#endif
}

void quick_ram_check(void)
{
	int fail = 0;
	u32 backup;
	backup = read_phys(CONFIG_RAMBASE);
	write_phys(CONFIG_RAMBASE, 0x55555555);
	phys_memory_barrier();
	if (read_phys(CONFIG_RAMBASE) != 0x55555555)
		fail=1;
	write_phys(CONFIG_RAMBASE, 0xaaaaaaaa);
	phys_memory_barrier();
	if (read_phys(CONFIG_RAMBASE) != 0xaaaaaaaa)
		fail=1;
	write_phys(CONFIG_RAMBASE, 0x00000000);
	phys_memory_barrier();
	if (read_phys(CONFIG_RAMBASE) != 0x00000000)
		fail=1;
	write_phys(CONFIG_RAMBASE, 0xffffffff);
	phys_memory_barrier();
	if (read_phys(CONFIG_RAMBASE) != 0xffffffff)
		fail=1;

	write_phys(CONFIG_RAMBASE, backup);
	if (fail) {
		post_code(0xea);
		die("RAM INIT FAILURE!\n");
	}
	phys_memory_barrier();
}

