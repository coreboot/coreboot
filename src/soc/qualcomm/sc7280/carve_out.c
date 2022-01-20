/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <device/mmio.h>

#define MODEM_ONLY 0x004c5445

bool soc_modem_carve_out(void **start, void **end)
{
	uint32_t modem_id = read32(_modem_id);

	switch (modem_id) {
	case MODEM_ONLY:
		*start = _dram_modem;
		*end = _edram_modem;
		return true;
	default:
		return false;
	}
}
