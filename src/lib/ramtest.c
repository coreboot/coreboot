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
 * Rotate ones test pattern that access every bit on a 128bit wide
 * memory bus. To test most address lines, addresses are scattered
 * using 256B, 4kB and 64kB increments.
 *
 * @idx		Index to test pattern (0=<idx<0x400)
 * @addr	Memory to access on @idx
 * @value	Value to write or read at @addr
 */
static inline void test_pattern(unsigned short int idx,
	unsigned long *addr, unsigned long *value)
{
	uint8_t j, k;

	k = (idx >> 8) + 1;
	j = (idx >> 4) & 0x0f;
	*addr = idx & 0x0f;
	*addr |= j << (4*k);
	*value = 0x01010101 << (j & 7);
	if (j & 8)
		*value = ~(*value);
}

/**
 * Simple write-read-verify memory test. See console debug output for
 * any dislocated bytes.
 *
 * @start   System memory offset, aligned to 128bytes
 */
static int ram_bitset_nodie(unsigned long start)
{
	unsigned long addr, value, value2;
	unsigned short int idx;
	unsigned char failed, failures;
	uint8_t verbose = 0;

#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "DRAM bitset write: 0x%08lx\n", start);
#else
	print_debug("DRAM bitset write: 0x");
	print_debug_hex32(start);
	print_debug("\n");
#endif
	for (idx=0; idx<0x400; idx+=4) {
		test_pattern(idx, &addr, &value);
		write_phys(start + addr, value);
	}

	/* Make sure we don't read before we wrote */
	phys_memory_barrier();

#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "DRAM bitset verify: 0x%08lx\n", start);
#else
	print_debug("DRAM bitset verify: 0x");
	print_debug_hex32(start);
	print_debug("\n");
#endif
	failures = 0;
	for (idx=0; idx<0x400; idx+=4) {
		test_pattern(idx, &addr, &value);
		value2 = read_phys(start + addr);

		failed = (value2 != value);
		failures |= failed;
		if  (failed && !verbose) {
#if !defined(__ROMCC__)
			printk(BIOS_ERR, "0x%08lx wr: 0x%08lx rd: 0x%08lx FAIL\n",
				 start + addr, value, value2);
#else
			print_err_hex32(start + addr);
			print_err(" wr: 0x");
			print_err_hex32(value);
			print_err(" rd: 0x");
			print_err_hex32(value2);
			print_err(" FAIL\n");
#endif
		}
		if (verbose) {
#if !defined(__ROMCC__)
			if ((addr & 0x0f) == 0)
				printk(BIOS_DEBUG, "%08lx wr: %08lx rd:",
					start + addr, value);
			if (failed)
				printk(BIOS_DEBUG, " %08lx!", value2);
			else
				printk(BIOS_DEBUG, " %08lx ", value2);
			if ((addr & 0x0f) == 0xc)
				printk(BIOS_DEBUG, "\n");
#else
			if ((addr & 0x0f) == 0) {
				print_dbg_hex32(start + addr);
				print_dbg(" wr: ");
				print_dbg_hex32(value);
				print_dbg(" rd: ");
			}
			print_dbg_hex32(value2);
			if (failed)
				print_dbg("! ");
			else
				print_dbg("  ");
			if ((addr & 0x0f) == 0xc)
				print_dbg("\n");
#endif
		}
	}
	if (failures) {
		post_code(0xea);
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, "\nDRAM did _NOT_ verify!\n");
#else
		print_debug("\nDRAM did _NOT_ verify!\n");
#endif
		return 1;
	}
	else {
#if !defined(__ROMCC__)
		printk(BIOS_DEBUG, "\nDRAM range verified.\n");
#else
		print_debug("\nDRAM range verified.\n");
		return 0;
#endif
	}
	return 0;
}


void ram_check(unsigned long start, unsigned long stop)
{
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "Testing DRAM at: %08lx\n", start);
#else
	print_debug("Testing DRAM at: ");
	print_debug_hex32(start);
	print_debug("\n");
#endif
	if (ram_bitset_nodie(start))
		die("DRAM ERROR");
#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "Done.\n");
#else
	print_debug("Done.\n");
#endif
}


int ram_check_nodie(unsigned long start, unsigned long stop)
{
	int ret;
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "Testing DRAM at : %08lx\n", start);
#else
	print_debug("Testing DRAM at : ");
	print_debug_hex32(start);
	print_debug("\n");
#endif

	ret = ram_bitset_nodie(start);
#if !defined(__ROMCC__)
	printk(BIOS_DEBUG, "Done.\n");
#else
	print_debug("Done.\n");
#endif
	return ret;
}

int ram_check_noprint_nodie(unsigned long start, unsigned long stop)
{
	unsigned long addr, value, value2;
	unsigned short int idx;
	unsigned char failed, failures;

	for (idx=0; idx<0x400; idx+=4) {
		test_pattern(idx, &addr, &value);
		write_phys(start + addr, value);
	}

	/* Make sure we don't read before we wrote */
	phys_memory_barrier();

	failures = 0;
	for (idx=0; idx<0x400; idx+=4) {
		test_pattern(idx, &addr, &value);
		value2 = read_phys(start + addr);

		failed = (value2 != value);
		failures |= failed;
	}
	return failures;
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
