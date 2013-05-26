/*
 * Copyright (C) 2013 Google Inc.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <console/console.h>
#include <delay.h>
#include <timer.h>

void init_timer(void)
{
}

/* delay x useconds */
void udelay(unsigned usec)
{
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_usecs(&end, usec);

	if (mono_time_after(&current, &end)) {
		printk(BIOS_EMERG, "udelay: 0x%08x is impossibly large\n",
				usec);
		/* There's not much we can do if usec is too big. Use a long,
		 * paranoid delay value and hope for the best... */
		end = current;
		mono_time_add_usecs(&end, USECS_PER_SEC);
	}

	while (mono_time_before(&current, &end))
		timer_monotonic_get(&current);
}
