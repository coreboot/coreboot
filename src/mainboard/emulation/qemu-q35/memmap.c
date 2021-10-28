/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <assert.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_ops.h>
#include <mainboard/emulation/qemu-i440fx/memory.h>
#include <mainboard/emulation/qemu-i440fx/fw_cfg.h>
#include <cpu/intel/smm_reloc.h>

#include "q35.h"

static uint32_t encode_pciexbar_length(void)
{
	switch (CONFIG_ECAM_MMCONF_BUS_NUMBER) {
		case 256: return 0 << 1;
		case 128: return 1 << 1;
		case  64: return 2 << 1;
		default:  return dead_code_t(uint32_t);
	}
}

uint32_t make_pciexbar(void)
{
	return CONFIG_ECAM_MMCONF_BASE_ADDRESS | encode_pciexbar_length() | 1;
}

/* Check that MCFG is active. If it's not, QEMU was started for machine PC */
void mainboard_machine_check(void)
{
	if (pci_read_config32(HOST_BRIDGE, D0F0_PCIEXBAR_LO) != make_pciexbar())
		die("You must run qemu for machine Q35 (-M q35)");
}

/* QEMU-specific register */
#define EXT_TSEG_MBYTES	0x50
#define SMRAMC	0x9d
#define C_BASE_SEG	((0 << 2) | (1 << 1) | (0 << 0))
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

void smm_lock(void)
{
	/*
	 * LOCK the SMM memory window and enable normal SMM.
	 * After running this function, only a full reset can
	 * make the SMM registers writable again.
	 */
	printk(BIOS_DEBUG, "Locking SMM.\n");

	if (CONFIG(SMM_TSEG))
		pci_or_config8(PCI_DEV(0, 0, 0), ESMRAMC, T_EN);
	pci_write_config8(PCI_DEV(0, 0, 0), SMRAMC, D_LCK | G_SMRAME | C_BASE_SEG);
}

void smm_open_aseg(void)
{
	pci_write_config8(PCI_DEV(0, 0, 0), SMRAMC, G_SMRAME | C_BASE_SEG | D_OPEN);
}
