/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
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

#include <types.h>
#include <io.h>

/**
 * Output a POST code (Power-on-self-test code) on port 0x80.
 *
 * We're hardcoding 0x80 as port where we output the POST code, as pretty
 * much all of today's computers use that one. However, in the past (pre-486
 * era or so) other ports were sometimes used, too.
 *
 * @param value The 8 bit value to output as POST code.
 */
void post_code(u8 value)
{
	outb(value, 0x80);
}
