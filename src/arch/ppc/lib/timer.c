/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <ppc.h>
#include <timer.h>
#include <clock.h>

unsigned long get_hz(void)
{
	return get_timer_freq();
}

unsigned long ticks_since_boot(void)
{
	extern unsigned long _get_ticks(void);
	return _get_ticks();
}

void udelay(int usecs)
{
	extern void _wait_ticks(unsigned long);
	unsigned long ticksperusec = get_hz() / 1000000;

	_wait_ticks(ticksperusec * usecs);
}
