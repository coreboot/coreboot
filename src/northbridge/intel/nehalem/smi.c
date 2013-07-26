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
