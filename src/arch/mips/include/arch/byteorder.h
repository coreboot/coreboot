/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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
 * Foundation, Inc.
 */

#ifndef __MIPS_ARCH_BYTEORDER_H
#define __MIPS_ARCH_BYTEORDER_H

#ifndef __ORDER_LITTLE_ENDIAN__
#errror "What endian are you!?"
#endif

#define __LITTLE_ENDIAN 1234

#endif /* __MIPS_ARCH_BYTEORDER_H */
