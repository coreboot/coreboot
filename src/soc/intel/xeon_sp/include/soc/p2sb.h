/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>

/*
 * Currently all known xeon-sp CPUs use C620 PCH. These definitions
 * come from C620 datasheet (Intel Doc #336067-007US)
 */

#define HPTC_OFFSET			0x60
#define HPTC_ADDR_ENABLE_BIT		(1 << 7)
#define PCH_P2SB_EPMASK0		0xb0
#define P2SB_SIZE			(16 * MiB)

#define P2SBC				0xe0
#define SBILOCK				(1 << 31)
