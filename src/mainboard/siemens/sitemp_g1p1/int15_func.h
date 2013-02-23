/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

typedef struct {
        u8 func00_LCD_panel_id;      // Callback Sub-Function 00h - Get LCD Panel ID
		u8 func02_set_expansion;
        u8 func05_TV_standard;       // Callback Sub-Function 05h - Select Boot-up TV Standard
		u16 func80_sysinfo_table;
}INT15_regs;

typedef struct {
        INT15_regs        regs;
}INT15_function_extensions;

extern void install_INT15_function_extensions(INT15_function_extensions *);
