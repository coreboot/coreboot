#include <smp/node.h>
#include <arch/bootblock_common.h>
#include <pc80/mc146818rtc.h>
#include <halt.h>

static const char *get_fallback(const char *stagelist) {
	while (*stagelist) stagelist++;
	return ++stagelist;
}

static void main(unsigned long bist)
{
	u8 boot_mode;
	const char *default_filenames =
		"normal/romstage\0fallback/romstage";

	if (boot_cpu()) {
		bootblock_mainboard_init();

#if CONFIG_USE_OPTION_TABLE
		sanitize_cmos();
#endif
		boot_mode = do_normal_boot();
	} else {

		/* Questionable single byte read from CMOS.
		 * Do not add any other CMOS access in the
		 * bootblock for AP CPUs.
		 */
		boot_mode = boot_use_normal(cmos_read(RTC_BOOT_BYTE));
	}

	char *normal_candidate = (char *)walkcbfs("coreboot-stages");

	if (!normal_candidate)
		normal_candidate = default_filenames;

	unsigned long entry;

	if (boot_mode) {
		entry = findstage(normal_candidate);
		if (entry)
			call(entry, bist);
	}

	entry = findstage(get_fallback(normal_candidate));
	if (entry)
		call(entry, bist);

	/* duh. we're stuck */
	halt();
}
