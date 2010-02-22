static void write_phys(unsigned long addr, unsigned long value)
{
#if CONFIG_HAVE_MOVNTI
	asm volatile(
		"movnti %1, (%0)"
		: /* outputs */
		: "r" (addr), "r" (value) /* inputs */
#ifndef __GNUC__
		: /* clobbers */
#endif
		);
#else
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	*ptr = value;
#endif
}

static unsigned long read_phys(unsigned long addr)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	return *ptr;
}

static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/* 
	 * Fill.
	 */
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("DRAM fill: 0x%08lx-0x%08lx\r\n", start, stop);
#else
	print_debug("DRAM fill: ");
	print_debug_hex32(start);
	print_debug("-");
	print_debug_hex32(stop);
	print_debug("\r\n");
#endif
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if (!(addr & 0xfffff)) {
#if CONFIG_USE_PRINTK_IN_CAR
			printk_debug("%08lx \r", addr);
#else
			print_debug_hex32(addr);
			print_debug(" \r");
#endif
		}
		write_phys(addr, addr);
	};
	/* Display final address */
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("%08lx\r\nDRAM filled\r\n", addr);
#else
	print_debug_hex32(addr);
	print_debug("\r\nDRAM filled\r\n");
#endif
}

static void ram_verify(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	int i = 0;
	/* 
	 * Verify.
	 */
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("DRAM verify: 0x%08lx-0x%08lx\r\n", start, stop);
#else
	print_debug("DRAM verify: ");
	print_debug_hex32(start);
	print_debug_char('-');
	print_debug_hex32(stop);
	print_debug("\r\n");
#endif
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if (!(addr & 0xfffff)) {
#if CONFIG_USE_PRINTK_IN_CAR
			printk_debug("%08lx \r", addr);
#else
			print_debug_hex32(addr);
			print_debug(" \r");
#endif
		}
		value = read_phys(addr);
		if (value != addr) {
			/* Display address with error */
#if CONFIG_USE_PRINTK_IN_CAR
			printk_err("Fail: @0x%08lx Read value=0x%08lx\r\n", addr, value);
#else
			print_err("Fail: @0x");
			print_err_hex32(addr);
			print_err(" Read value=0x");
			print_err_hex32(value);
			print_err("\r\n");
#endif
			i++;
			if(i>256) {
#if CONFIG_USE_PRINTK_IN_CAR
				printk_debug("Aborting.\n\r");
#else
				print_debug("Aborting.\n\r");
#endif
				break;
			}
		}
	}
	/* Display final address */
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("%08lx", addr);
#else
	print_debug_hex32(addr);
#endif

	if (i) {
#if CONFIG_USE_PRINTK_IN_CAR
		printk_debug("\r\nDRAM did _NOT_ verify!\r\n");
#else
		print_debug("\r\nDRAM did _NOT_ verify!\r\n");
#endif
		die("DRAM ERROR");
	}
	else {
#if CONFIG_USE_PRINTK_IN_CAR
		printk_debug("\r\nDRAM range verified.\r\n");
#else
		print_debug("\r\nDRAM range verified.\r\n");
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
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("Testing DRAM : %08lx - %08lx\r\n", start, stop);
#else
	print_debug("Testing DRAM : ");
	print_debug_hex32(start);
	print_debug("-");	
	print_debug_hex32(stop);
	print_debug("\r\n");
#endif
	ram_fill(start, stop);
	ram_verify(start, stop);
#if CONFIG_USE_PRINTK_IN_CAR
	printk_debug("Done.\r\n");
#else
	print_debug("Done.\r\n");
#endif
}

