#ifndef CPU_X86_BIST_H
#define CPU_X86_BIST_H

static void report_bist_failure(unsigned long bist)
{
	if (bist != 0) {
		print_emerg("BIST failed: ");
		print_emerg_hex32(bist);
		die("\r\n");
	}
}

#endif /* CPU_Xf86_BIST_H */
