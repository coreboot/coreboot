/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/stages.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <device/mmio.h>

#define MODEM_ID_LTE 0x004c5445
#define MODEM_ID_WIFI 0x57494649

bool soc_modem_carve_out(void **start, void **end)
{
	uint32_t modem_id = read32(_modem_id);

	switch (modem_id) {
	case MODEM_ID_LTE:
		*start = _dram_modem_wifi_only;
		*end = _edram_modem_extra;
		return true;
	case MODEM_ID_WIFI:
		*start = _dram_modem_wifi_only;
		*end = _edram_modem_wifi_only;
		return true;
	default:
		return false;
	}
}
