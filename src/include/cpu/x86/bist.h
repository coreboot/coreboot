#ifndef CPU_X86_BIST_H
#define CPU_X86_BIST_H

static void report_bist_failure(u32 bist)
{
	if (bist != 0) {
#if CONFIG_USE_PRINTK_IN_CAR
                printk_emerg("BIST failed: %08x", bist);
#else
		print_emerg("BIST failed: ");
		print_emerg_hex32(bist);
#endif
		die("\r\n");

	}
}

#endif /* CPU_Xf86_BIST_H */
