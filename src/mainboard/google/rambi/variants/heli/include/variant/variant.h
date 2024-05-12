/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

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

#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58
#define GPIO_SSUS_40_PAD 52

#endif
