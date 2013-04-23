/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

#ifndef __I386_ARCH_IOAPIC_H
#define __I386_ARCH_IOAPIC_H

#define IO_APIC_ADDR	0xfec00000
#define IO_APIC_INTERRUPTS 24

#ifndef __ACPI__
#define ALL		(0xff << 24)
#define NONE		(0)
#define DISABLED	(1 << 16)
#define ENABLED		(0 << 16)
#define TRIGGER_EDGE	(0 << 15)
#define TRIGGER_LEVEL	(1 << 15)
#define POLARITY_HIGH	(0 << 13)
#define POLARITY_LOW	(1 << 13)
#define PHYSICAL_DEST	(0 << 11)
#define LOGICAL_DEST	(1 << 11)
#define ExtINT		(7 << 8)
#define NMI		(4 << 8)
#define SMI		(2 << 8)
#define INT		(1 << 8)

u32 io_apic_read(u32 ioapic_base, u32 reg);
void io_apic_write(u32 ioapic_base, u32 reg, u32 value);
void set_ioapic_id(u32 ioapic_base, u8 ioapic_id);
void setup_ioapic(u32 ioapic_base, u8 ioapic_id);
void clear_ioapic(u32 ioapic_base);
#endif

#endif
