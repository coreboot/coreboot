/*
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#ifndef MAINBOARD_H
#define MAINBOARD_H
enum {
	SNOW_IS_NOT_WAKEUP,  // A normal boot (not suspend/resume).
	SNOW_WAKEUP_DIRECT,  // A wake up event that can be resumed any time.
	SNOW_WAKEUP_NEED_CLOCK_RESET,  // A wake up event that must be resumed
					// only after clock and memory
					// controllers are re-initialized.
};

int snow_get_wakeup_state(void);
void snow_wakeup(void);

#endif	/* MAINBOARD_H */
