/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

/*
 * RAM_ID[2:0] are on GPIO_SSUS[39:37]
 * 0b0000 - 4GiB total - 2 x 2GiB Micron  MT41K256M16HA-125:E 1600MHz
 * 0b0001 - 4GiB total - 2 x 2GiB Hynix   H5TC4G63AFR-PBA 1600MHz
 * 0b0010 - 2GiB total - 2 x 1GiB Micron  MT41K128M16JT-125:K 1600MHz
 * 0b0011 - 2GiB total - 2 x 1GiB Hynix   H5TC2G63FFR-PBA 1600MHz
 * 0b0100 - 2GiB total - 2 x 1GiB Samsung K4B2G1646Q-BYK0 1600MHz
 * 0b0101 - 2GiB total - 1 x 2GiB Hynix   H5TC4G63AFR-PBA 1600MHz
 * 0b0110 - 4GiB total - 2 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 * 0b0111 - 4GiB total - 2 x 2GiB Elpida  EDJ4216EFBG-GNL-F 1600MHz
 * 0b1000 - 2GiB total - 1 x 2GiB Micron  MT41K256M16HA-125:E 1600MHz
 * 0b1001 - 2GiB total - 1 x 2GiB Elpida  EDJ4216EFBG-GNL-F 1600MHz
 * 0b1010 - 2GiB total - 1 x 2GiB Samsung K4B4G1646Q-HYK0 1600MHz
 */

static const uint32_t dual_channel_config =
	(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3) |
	(1 << 4) |	      (1 << 6) | (1 << 7);

#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58
#define GPIO_SSUS_40_PAD 52
#define GPIO_SSUS_40_PAD_USE_PULLDOWN

#endif
