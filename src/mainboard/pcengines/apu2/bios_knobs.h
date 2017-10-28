/*
 * This file is part of the coreboot project.

 *
 * Copyright (C) 2017 3mdeb
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _BIOS_KNOBS_H
#define _BIOS_KNOBS_H

bool check_console(void);
bool check_uartc(void);
bool check_uartd(void);
bool check_ehci0(void);
bool check_mpcie2_clk(void);


#endif
