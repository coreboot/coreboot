#ifndef CPU_X86_BIST_H
#define CPU_X86_BIST_H

static void report_bist_failure(u32 bist)
{
	if (bist != 0) {
                printk(BIOS_EMERG, "BIST failed: %08x", bist);
		die("\n");
	}
}

#endif /* CPU_Xf86_BIST_H */
