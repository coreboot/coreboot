/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

#ifndef SOUTHBRIDGE_VIA_VT8237R_VT8237R_H
#define SOUTHBRIDGE_VIA_VT8237R_VT8237R_H

/* Static resources for the VT8237R southbridge. */

#define VT8237R_APIC_ID		0x2
#define VT8237R_ACPI_IO_BASE	0x500
#define VT8237R_SMBUS_IO_BASE	0x400
/* 0x0 disabled, 0x2 reserved, 0xf = IRQ15 */
#define VT8237R_ACPI_IRQ	0x9
#define VT8237R_HPET_ADDR	0xfed00000ULL
#define VT8237R_APIC_BASE	0xfec00000ULL

#endif
