/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <mainboard/emulation/qemu-i440fx/memory.h>
#include <mainboard/emulation/qemu-i440fx/fw_cfg.h>

#include "q35.h"

static uint32_t encode_pciexbar_length(void)
{
	switch (CONFIG_MMCONF_BUS_NUMBER) {
		case 256: return 0 << 1;
		case 128: return 1 << 1;
		case  64: return 2 << 1;
		default:  return dead_code_t(uint32_t);
	}
}

uint32_t make_pciexbar(void)
{
	return CONFIG_MMCONF_BASE_ADDRESS | encode_pciexbar_length() | 1;
}

/* Check that MCFG is active. If it's not, QEMU was started for machine PC */
void mainboard_machine_check(void)
{
	if (pci_read_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO) != make_pciexbar())
		die("You must run qemu for machine Q35 (-M q35)");
}

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
