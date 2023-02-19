/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_rom.h>
#include <soc/cpu.h>
#include <stdint.h>

u32 map_oprom_vendev(u32 vendev)
{
	switch (vendev) {
	case PHOENIX2_VBIOS_VID_DID:
		return PHOENIX2_GPU_VID_DID;
	}

	return vendev;
}
