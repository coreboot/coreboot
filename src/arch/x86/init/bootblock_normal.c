#include <bootblock_common.h>
#include <pc80/mc146818rtc.h>

static void main(unsigned long bist)
{
	unsigned long entry;
	int bsp_cpu = boot_cpu();
	int boot_mode;

	/* Mainboard-specific early init. */
	init_mainboard(bsp_cpu);

	if (bsp_cpu) {
#if CONFIG_USE_OPTION_TABLE
		sanitize_cmos();
#endif
		boot_mode = do_normal_boot();
	} else {

		/* Questionable single byte read from CMOS.
		 * Do not add any other CMOS access in the
		 * bootblock for AP CPUs.
		 */
		boot_mode = last_boot_normal();
	}

	if (boot_mode)
		entry = findstage("normal/romstage");
	else
		entry = findstage("fallback/romstage");

	if (entry) call(entry, bist);

	/* run fallback if normal can't be found */
	entry = findstage("fallback/romstage");
	if (entry) call(entry, bist);

	/* duh. we're stuck */
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

