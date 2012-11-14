#include <bootblock_common.h>
#include <pc80/mc146818rtc.h>

static const char *get_fallback(const char *stagelist) {
	while (*stagelist) stagelist++;
	return ++stagelist;
}

static void main(unsigned long bist)
{
	unsigned long entry;
	int boot_mode;
	const char *default_filenames = "normal/romstage\0fallback/romstage";

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
		boot_mode = last_boot_normal();
	}

	char *filenames = (char *)walkcbfs("coreboot-stages");
	if (!filenames) {
		filenames = default_filenames;
	}
	char *normal_candidate = filenames;

	if (boot_mode)
		entry = findstage(normal_candidate);
	else
		entry = findstage(get_fallback(normal_candidate));

	if (entry) call(entry, bist);

	/* run fallback if normal can't be found */
	entry = findstage(get_fallback(normal_candidate));
	if (entry) call(entry, bist);

	/* duh. we're stuck */
	asm volatile ("1:\n\thlt\n\tjmp 1b\n\t");
}

