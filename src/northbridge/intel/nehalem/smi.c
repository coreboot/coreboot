/*
 * This file is part of the coreboot project.
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

#define __SIMPLE_DEVICE__

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/pci.h>
#include "nehalem.h"

#include <cpu/intel/smm/gen1/smi.h>

void northbridge_write_smram(u8 smram)
{
	pci_write_config8(PCI_DEV(QUICKPATH_BUS, 0, 1), QPD0F1_SMRAM, smram);
}
