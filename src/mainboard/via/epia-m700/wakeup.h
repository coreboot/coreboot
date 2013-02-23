/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#ifndef WAKEUP_H
#define WAKEUP_H

#define WAKE_SPECIAL_AREA	0xE0000
#define WAKE_SPECIAL_SIZE	0x1000
#define WAKE_THUNK16_ADDR	(WAKE_SPECIAL_AREA + 0x200)
#define WAKE_THUNK16_GDT	(WAKE_SPECIAL_AREA + 0x300)
#define WAKE_THUNK16_XDTR	(WAKE_SPECIAL_AREA + 0x350)
#define WAKE_MEM_INFO		(WAKE_SPECIAL_AREA + 0x400)
#define WAKE_RECOVER1M_CODE	(WAKE_SPECIAL_AREA + 0x500)
#define WAKE_THUNK16_STACK	(WAKE_SPECIAL_AREA + 0xf00)

#endif
