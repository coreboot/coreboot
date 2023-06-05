/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_X86_VGA_H
#define ARCH_X86_VGA_H

/* VGA MMIO and SMM ASEG share the same address range */
#define VGA_MMIO_BASE	0xa0000
#define VGA_MMIO_SIZE	0x20000
#define VGA_MMIO_LIMIT	(VGA_MMIO_BASE + VGA_MMIO_SIZE - 1)

#endif /* ARCH_X86_VGA_H */
