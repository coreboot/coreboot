/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <mainboard/emulation/qemu-i440fx/memory.h>
#include <mainboard/emulation/qemu-i440fx/fw_cfg.h>

#include "q35.h"

/* QEMU-specific register */
#define EXT_TSEG_MBYTES	0x50

void smm_region(uintptr_t *start, size_t *size)
{
	uint8_t esmramc = pci_read_config8(HOST_BRIDGE, ESMRAMC);

	switch ((esmramc & TSEG_SZ_MASK) >> 1) {
	case 0:
		*size = 1 * MiB;
		break;
	case 1:
		*size = 2 * MiB;
		break;
	case 2:
		*size = 8 * MiB;
		break;
	default:
		*size = pci_read_config16(HOST_BRIDGE, EXT_TSEG_MBYTES) * MiB;
	}

	*start = qemu_get_memory_size() * KiB - *size;
	printk(BIOS_SPEW, "SMM_BASE: 0x%08lx, SMM_SIZE: %zu MiB\n", *start, *size / MiB);
}
