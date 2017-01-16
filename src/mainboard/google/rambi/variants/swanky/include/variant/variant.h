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
 * RAM_ID[2:0] are on GPIO_SSUS[39:37]
 * 0b000 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 * 0b001 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 * 0b010 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 * 0b011 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 */

static const uint32_t dual_channel_config =
	(1 << 2) | (1 << 3);

#define SPD_SIZE 256
#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58

#endif
