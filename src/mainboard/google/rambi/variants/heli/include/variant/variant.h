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

#ifndef VARIANT_H
#define VARIANT_H

/*
 * RAM_ID[3:0] are on GPIO_SSUS[40:37]
 * RAM_ID	Vendor	Vendor_PN	Freq	Size	Total_size	channel
 * 0b0011	Hynix	H5TC4G63AFR-PBA	1600MHZ	4Gb	2GB	single-channel
 * 0b0100	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	2GB	single-channel
 * 0b0101	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	2GB	single-channel
 * 0b0110	Hynix	H5TC4G63CFR-PBA	1600MHZ	4Gb	4GB	dual-channel
 * 0b0111	Samsung	K4B4G1646Q-HYK0	1600MHZ	4Gb	4GB	dual-channel
 */

static const uint32_t dual_channel_config =
	(1 << 6) | (1 << 7);

#define SPD_SIZE 256
#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58
#define GPIO_SSUS_40_PAD 52

#endif
