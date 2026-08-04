/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>

void mainboard_post(uint8_t value)
{
	google_chromeec_post(value);
}
