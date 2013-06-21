#include <bootblock_common.h>
#include <pc80/mc146818rtc.h>

int boot_mode = 0;

static const char *get_fallback(const char *stagelist) {
	while (*stagelist) stagelist++;
	return ++stagelist;
}

static void main(unsigned long bist)
{
	unsigned long entry;
	const char *default_filenames = "normal/romstage\0fallback/romstage";

	/* I hope that the BSP CPU is started first
	 * and reach this point first... */
	if (boot_cpu()) {
		bootblock_mainboard_init();

#if CONFIG_USE_OPTION_TABLE
		sanitize_cmos();
#endif
		boot_mode = get_and_reset_boot();
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

