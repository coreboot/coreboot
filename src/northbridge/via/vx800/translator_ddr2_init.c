/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#define CB_SUCCESS		0x0
#define CB_INVALID_PARAMETER	0x2
#define CB_NOT_READY		0x6
#define CB_DEVICE_ERROR		0x7
#define TRUE  1
#define FALSE 0

typedef int8_t INT8;
typedef unsigned long uintn_t;
typedef uintn_t UINTN;
typedef long intn_t;
typedef intn_t INTN;
typedef UINTN CB_STATUS;
typedef uint8_t BOOLEAN;
