#include <stdint.h>
#include <lib.h> /* Prototypes */
#include <console/console.h>

static void write_phys(unsigned long addr, u32 value)
{
	// Assembler in lib/ is very ugly. But we properly guarded
	// it so let's obey this one for now
#if IS_ENABLED(CONFIG_SSE2)
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
#if IS_ENABLED(CONFIG_SSE2)
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
 * @param idx	Index to test pattern (0=<idx<0x400)
 * @param addr	Memory to access on idx
 * @param value	Value to write or read at addr
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
 * Tests 1MiB of memory starting from start.
 *
 * @param start   System memory offset, aligned to 128bytes
 */
static int ram_bitset_nodie(unsigned long start)
{
	unsigned long addr, value, value2;
	unsigned short int idx;
	unsigned char failed, failures;
	uint8_t verbose = 0;

	printk(BIOS_DEBUG, "DRAM bitset write: 0x%08lx\n", start);
	for (idx = 0; idx < 0x400; idx += 4) {
		test_pattern(idx, &addr, &value);
		write_phys(start + addr, value);
	}

	/* Make sure we don't read before we wrote */
	phys_memory_barrier();

	printk(BIOS_DEBUG, "DRAM bitset verify: 0x%08lx\n", start);
	failures = 0;
	for (idx = 0; idx < 0x400; idx += 4) {
		test_pattern(idx, &addr, &value);
		value2 = read_phys(start + addr);

		failed = (value2 != value);
		failures |= failed;
		if (failed && !verbose) {
			printk(BIOS_ERR, "0x%08lx wr: 0x%08lx rd: 0x%08lx FAIL\n",
				 start + addr, value, value2);
		}
		if (verbose) {
			if ((addr & 0x0f) == 0)
				printk(BIOS_DEBUG, "%08lx wr: %08lx rd:",
					start + addr, value);
			if (failed)
				printk(BIOS_DEBUG, " %08lx!", value2);
			else
				printk(BIOS_DEBUG, " %08lx ", value2);
			if ((addr & 0x0f) == 0xc)
				printk(BIOS_DEBUG, "\n");
		}
	}
	if (failures) {
		post_code(0xea);
		printk(BIOS_DEBUG, "\nDRAM did _NOT_ verify!\n");
		return 1;
	}
	printk(BIOS_DEBUG, "\nDRAM range verified.\n");
	return 0;
}


void ram_check(unsigned long start, unsigned long stop)
{
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk(BIOS_DEBUG, "Testing DRAM at: %08lx\n", start);
	if (ram_bitset_nodie(start))
		die("DRAM ERROR");
	printk(BIOS_DEBUG, "Done.\n");
}


int ram_check_nodie(unsigned long start, unsigned long stop)
{
	int ret;
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk(BIOS_DEBUG, "Testing DRAM at : %08lx\n", start);

	ret = ram_bitset_nodie(start);
	printk(BIOS_DEBUG, "Done.\n");
	return ret;
}

int ram_check_noprint_nodie(unsigned long start, unsigned long stop)
{
	unsigned long addr, value, value2;
	unsigned short int idx;
	unsigned char failed, failures;

	for (idx = 0; idx < 0x400; idx += 4) {
		test_pattern(idx, &addr, &value);
		write_phys(start + addr, value);
	}

	/* Make sure we don't read before we wrote */
	phys_memory_barrier();

	failures = 0;
	for (idx = 0; idx < 0x400; idx += 4) {
		test_pattern(idx, &addr, &value);
		value2 = read_phys(start + addr);

		failed = (value2 != value);
		failures |= failed;
	}
	return failures;
}

static void __quick_ram_check(uintptr_t dst)
{
	int fail = 0;
	u32 backup;
	backup = read_phys(dst);
	write_phys(dst, 0x55555555);
	phys_memory_barrier();
	if (read_phys(dst) != 0x55555555)
		fail = 1;
	write_phys(dst, 0xaaaaaaaa);
	phys_memory_barrier();
	if (read_phys(dst) != 0xaaaaaaaa)
		fail = 1;
	write_phys(dst, 0x00000000);
	phys_memory_barrier();
	if (read_phys(dst) != 0x00000000)
		fail = 1;
	write_phys(dst, 0xffffffff);
	phys_memory_barrier();
	if (read_phys(dst) != 0xffffffff)
		fail = 1;

	write_phys(dst, backup);
	if (fail) {
		post_code(0xea);
		die("RAM INIT FAILURE!\n");
	}
	phys_memory_barrier();
}

void quick_ram_check(void)
{
	__quick_ram_check(0x100000);
}
