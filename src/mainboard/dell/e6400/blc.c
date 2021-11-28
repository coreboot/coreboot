/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <northbridge/intel/gm45/gm45.h>

/*
 * This contains a list of panel IDs and a known well working
 * backlight PWM frequency.
 */
static const struct blc_pwm_t blc_entries[] = {
	/*
	 * Not sure if that's just a corrupt byte or just something
	 * weird about the EDID of the panel in my system. Also, the
	 * vendor firmware sets the pwm frequency to a rather high
	 * value compared to other GM45 systems in the tree.
	 */
	{"G022H\200141WX5", 12315},
};

int get_blc_values(const struct blc_pwm_t **entries)
{
	*entries = blc_entries;
	return ARRAY_SIZE(blc_entries);
}
