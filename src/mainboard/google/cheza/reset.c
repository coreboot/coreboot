/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <ec/google/chromeec/ec.h>
#include <reset.h>

/* Can't do a "real" reset before the PMIC is initialized in QcLib (romstage),
   but this works well enough for our purposes. */
void do_board_reset(void)
{
	google_chromeec_reboot(0, EC_REBOOT_COLD, 0);
}
