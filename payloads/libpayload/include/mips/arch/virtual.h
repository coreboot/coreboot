/*
 * This file is part of the libpayload project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __MIPS_ARCH_VIRTUAL_H
#define __MIPS_ARCH_VIRTUAL_H

extern unsigned long virt_to_phys_offset;
extern unsigned long virt_to_bus_offset;

#define virt_to_phys(virt)	((unsigned long) (virt) + virt_to_phys_offset)
#define phys_to_virt(phys)	((void *) ((unsigned long) (phys) -	\
				virt_to_phys_offset))

#define virt_to_bus(virt)	((unsigned long) (virt) + virt_to_bus_offset)
#define bus_to_virt(phys)	((void *) ((unsigned long) (phys) -	\
				virt_to_bus_offset))

#endif
