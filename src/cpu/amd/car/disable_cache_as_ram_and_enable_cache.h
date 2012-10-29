/*
 * This file is part of the coreboot project.
 *
 * original idea yhlu 6.2005 (assembler code)
 *
 * Copyright (C) 2010 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 * be warned, this file will be used other cores and core 0 / node 0
 */

extern void disable_cache_as_ram(void); /* cache_as_ram.inc */
static inline __attribute__((always_inline)) void disable_cache_as_ram_and_enable_cache(void)
{
	disable_cache_as_ram();
	enable_cache();
}
