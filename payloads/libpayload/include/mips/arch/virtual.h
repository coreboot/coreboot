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
 */

#ifndef __MIPS_ARCH_VIRTUAL_H
#define __MIPS_ARCH_VIRTUAL_H

#define KSEG0_BASE		0x80000000
#define KSEG1_BASE		0xA0000000

#define kseg0_to_phys(virt)	((unsigned long)(virt) - KSEG0_BASE)
#define phys_to_kseg0(phys)	((void *)((unsigned long)(phys) + KSEG0_BASE))

#define kseg1_to_phys(virt)	((unsigned long)(virt) - KSEG1_BASE)
#define phys_to_kseg1(phys)	((void *)((unsigned long)(phys) + KSEG1_BASE))

#define virt_to_phys(virt)	((unsigned long)(virt))
#define phys_to_virt(phys)	((void *)(unsigned long)(phys))

#define virt_to_bus(virt)	kseg1_to_phys(virt)
#define bus_to_virt(phys)	phys_to_kseg1(phys)

#endif
