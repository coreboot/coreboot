/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard.h>
#include <device/device.h>
#include <intelblocks/pcr.h>
#include <soc/pcr_ids.h>

void variant_mainboard_init(struct device *dev)
{
	// TODO: NMI; is this needed? vendor sets it
	pcr_write32(0xae, 0x01e4, 0x00000004);
	pcr_write32(0xae, 0x01e8, 0x00000040);
}
