/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ARCH_VIRTUAL_H
#define _ARCH_VIRTUAL_H

/* virtual_offset has to be declared if used */
extern unsigned long virtual_offset;

#define virt_to_phys(virt) ((unsigned long)(virt) + virtual_offset)
#define phys_to_virt(phys) ((void *)((unsigned long)(phys) - virtual_offset))

#define virt_to_bus(addr) virt_to_phys(addr)
#define bus_to_virt(addr) phys_to_virt(addr)

#endif
