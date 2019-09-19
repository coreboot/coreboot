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

#include <mainboard.h>
#include <device/device.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>

void variant_mainboard_init(struct device *dev)
{
	/* TODO:
	* Find out why the polarities from gpio.h gets overwritten by FSP.
	* This sets irq polarity to the same values as vendor
	* but I do not know if this is really needed....
	*/
	itss_set_irq_polarity(33, 0);
	itss_set_irq_polarity(34, 0);

	// TODO: NMI; is this needed? vendor sets it
	pcr_write32(0xae, 0x01e4, 0x00000004);
	pcr_write32(0xae, 0x01e8, 0x00000040);
}
