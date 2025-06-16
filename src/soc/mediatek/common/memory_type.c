/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <commonlib/bsd/mem_chip_info.h>
#include <soc/emi.h>

enum mem_chip_type map_to_lpddr_dram_type(uint16_t lpddr_type)
{
	switch (lpddr_type) {
	case TYPE_LPDDR4X:
		return MEM_CHIP_LPDDR4X;
	case TYPE_LPDDR5:
		return MEM_CHIP_LPDDR5;
	case TYPE_LPDDR5X:
		return MEM_CHIP_LPDDR5X;
	default:
		return MEM_CHIP_UNDEFINED;
	}
}
