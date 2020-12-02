/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <mainboard/emulation/qemu-i440fx/memory.h>
#include <mainboard/emulation/qemu-i440fx/fw_cfg.h>

#define EXT_TSEG_MBYTES	0x50

#define SMRAMC	0x9d
#define G_SMRAME	(1 << 3)
#define D_LCK		(1 << 4)
#define D_CLS		(1 << 5)
#define D_OPEN		(1 << 6)
#define ESMRAMC	0x9e
#define T_EN		(1 << 0)
#define TSEG_SZ_MASK	(3 << 1)
#define H_SMRAME	(1 << 7)

void smm_region(uintptr_t *start, size_t *size)
{
	uint8_t esmramc = pci_read_config8(PCI_DEV(0, 0, 0), ESMRAMC);

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
		*size = pci_read_config16(PCI_DEV(0, 0, 0), EXT_TSEG_MBYTES) * MiB;
	}

	*start = qemu_get_memory_size() * KiB - *size;
	printk(BIOS_SPEW, "SMM_BASE: 0x%08lx, SMM_SIZE: %zu MiB\n", *start, *size / MiB);
}
