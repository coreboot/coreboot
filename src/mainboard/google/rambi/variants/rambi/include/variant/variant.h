/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef VARIANT_H
#define VARIANT_H

#include <stdint.h>

/*
 * RAM_ID[2:0] are on GPIO_SSUS[39:37]
 * 0b000 - 4GiB total - 2 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
 * 0b001 - 4GiB total - 2 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 * 0b010 - 2GiB total - 2 x 1GiB Micron MT41K128M16JT-125:K 1600MHz
 * 0b011 - 2GiB total - 2 x 1GiB Hynix  H5TC2G63FFR-PBA 1600MHz
 * 0b100 - 2GiB total - 1 x 2GiB Micron MT41K256M16HA-125:E 1600MHz
 * 0b101 - 2GiB total - 1 x 2GiB Hynix  H5TC4G63AFR-PBA 1600MHz
 */
static const uint32_t dual_channel_config =
	(1 << 0) | (1 << 1) | (1 << 2) | (1 << 3);

#define GPIO_SSUS_37_PAD 57
#define GPIO_SSUS_38_PAD 50
#define GPIO_SSUS_39_PAD 58

#endif
