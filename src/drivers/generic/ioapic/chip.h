/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#ifndef DRIVERS_GENERIC_IOAPIC_CHIP_H
#define DRIVERS_GENERIC_IOAPIC_CHIP_H

typedef struct drivers_generic_ioapic_config {
	u32 version;
	u8 apicid;
	u8 irq_on_fsb;
	u8 enable_virtual_wire;
	u8 have_isa_interrupts;
	u32 base;
} ioapic_config_t;

#endif
