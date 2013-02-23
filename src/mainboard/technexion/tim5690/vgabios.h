/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
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

/* AMD Chipset */
#define AMD_RS690_INT15 0x4E08

typedef struct __rs690_int15_regs__
{
        u8 fun00_panel_id;      // Callback Sub-Function 00h - Get LCD Panel ID
        u8 fun05_tv_standard;   // Callback Sub-Function 05h - Select Boot-up TV Standard
}rs690_int15_regs;

typedef struct __rs690_vbios_regs__
{
        rs690_int15_regs        int15_regs;
}rs690_vbios_regs;

/* Initialization VBIOS function */
extern void vgabios_init(rs690_vbios_regs *vbios_regs);
