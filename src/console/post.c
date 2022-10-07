/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <stdint.h>

/* Write POST information */
void __weak arch_post_code(uint8_t value) { }
void __weak soc_post_code(uint8_t value) { }

/* Some mainboards have very nice features beyond just a simple display.
 * They can override this function.
 */
void __weak mainboard_post(uint8_t value) { }

void post_code(uint8_t value)
{
	if (!CONFIG(NO_POST)) {
		/* Assume this to be the most reliable and simplest type
		   for displaying POST so keep it first. */
		arch_post_code(value);

		soc_post_code(value);

		if (CONFIG(CONSOLE_POST))
			printk(BIOS_INFO, "POST: 0x%02x\n", value);

		mainboard_post(value);
	}
}
