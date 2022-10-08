/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_rom.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <stdint.h>

void map_oprom_vendev_rev(u32 *vendev, u8 *rev)
{
	if (*vendev != PICASSO_VBIOS_VID_DID)
		return;

	/* Check if the RV2 video bios needs to be used instead of the RV1/PCO one */
	if (soc_is_raven2()) {
		printk(BIOS_NOTICE, "Using RV2 VBIOS.\n");
		*vendev = RAVEN2_VBIOS_VID_DID;
		*rev = RAVEN2_VBIOS_REV;
	} else {
		printk(BIOS_NOTICE, "Using RV1/PCO VBIOS.\n");
		*rev = PICASSO_VBIOS_REV;
	}
}
