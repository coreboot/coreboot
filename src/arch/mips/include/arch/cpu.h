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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __MIPS_ARCH_CPU_H
#define __MIPS_ARCH_CPU_H

#define asmlinkage

#ifndef __PRE_RAM__

#include <device/device.h>

struct cpu_driver {
	struct device_operations *ops;
	struct cpu_device_id *id_table;
};

struct thread;

struct cpu_info {
	device_t cpu;
	unsigned long index;
};

#endif /* !__PRE_RAM__ */

#endif /* __MIPS_ARCH_CPU_H */
