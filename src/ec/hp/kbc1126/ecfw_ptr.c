/* SPDX-License-Identifier: GPL-2.0-only */

#include <endian.h>
#include "ecfw_ptr.h"

/*
 * Address info for EC SMSC KBC1098/KBC1126 to find their firmware blobs,
 * linked to CONFIG_ECFW_PTR_ADDR via src/arch/x86/bootblock.ld
 */
__attribute__((used, __section__(".ecfw_ptr")))
const struct ecfw_ptr ecfw_ptr = {
	.fw1.off = cpu_to_be16((uint16_t)(CONFIG_KBC1126_FW1_OFFSET >> 8)),
	.fw1.inv = cpu_to_be16((uint16_t)~(CONFIG_KBC1126_FW1_OFFSET >> 8)),
	.fw2.off = cpu_to_be16((uint16_t)(CONFIG_KBC1126_FW2_OFFSET >> 8)),
	.fw2.inv = cpu_to_be16((uint16_t)~(CONFIG_KBC1126_FW2_OFFSET >> 8)),
};
