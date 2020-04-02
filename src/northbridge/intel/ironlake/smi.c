/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define __SIMPLE_DEVICE__

#include <types.h>
#include <device/device.h>
#include <device/pci_ops.h>
#include "ironlake.h"

#include <cpu/intel/smm_reloc.h>

void northbridge_write_smram(u8 smram)
{
	pci_write_config8(PCI_DEV(QUICKPATH_BUS, 0, 1), QPD0F1_SMRAM, smram);
}
